// Copyright 2022-2023 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "nearby_connections_manager_impl.h"

#include <stdint.h>

#include <filesystem>  // NOLINT(build/c++17)
#include <functional>
#include <memory>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/meta/type_traits.h"
#include "absl/strings/string_view.h"
#include "absl/time/time.h"
#include "absl/types/span.h"
#include "internal/flags/nearby_flags.h"
#include "internal/platform/device_info.h"
#include "internal/platform/mutex_lock.h"
#include "advertisement.h"
#include "sharing/common/nearby_share_enums.h"
#include "constants.h"
//#include "sharing/flags/generated/nearby_sharing_feature_flags.h"
//#include "sharing/internal/api/bluetooth_adapter.h"
//#include "sharing/internal/base/encode.h"
//#include "sharing/internal/public/connectivity_manager.h"
//#include "sharing/internal/public/context.h"
#include "logging.h"
#include "nearby_connection_impl.h"
#include "nearby_connections_manager.h"
#include "nearby_connections_service.h"
#include "nearby_connections_types.h"
#include "transfer_manager.h"

namespace nearby {
namespace sharing {
namespace {
using ::nearby::sharing::proto::DataUsage;

constexpr char kServiceId[] = "NearbySharing";
constexpr char kFastAdvertisementServiceUuid[] =
    "0000fef3-0000-1000-8000-00805f9b34fb";
constexpr Strategy kStrategy = Strategy::kP2pPointToPoint;

const uint8_t kMinimumAdvertisementSize =
    /* Version(3 bits)|Visibility(1 bit)|Device Type(3 bits)|Reserved(1 bits)=
     */
    1 + Advertisement::kSaltSize +
    Advertisement::kMetadataEncryptionKeyHashByteSize;



std::string MediumSelectionToString(const MediumSelection& mediums) {
  std::stringstream ss;
  ss << "{";
  if (mediums.bluetooth) ss << "bluetooth ";
  if (mediums.ble) ss << "ble ";
  if (mediums.web_rtc) ss << "webrtc ";
  if (mediums.wifi_lan) ss << "wifilan ";
  ss << "}";

  return ss.str();
}

}  // namespace

NearbyConnectionsManagerImpl::NearbyConnectionsManagerImpl(std::unique_ptr<NearbyConnectionsService> nearby_connections_service)
    : nearby_connections_service_(std::move(nearby_connections_service)) {}

NearbyConnectionsManagerImpl::~NearbyConnectionsManagerImpl() {
}

void NearbyConnectionsManagerImpl::OnConnectionInitiated(
    absl::string_view endpoint_id, const ConnectionInfo& info)
{
    MutexLock lock(&mutex_);

    [[maybe_unused]] auto result =
        connection_info_map_.emplace(endpoint_id, std::move(info));

    NL_DCHECK(result.second);

    NearbyConnectionsService::PayloadListener payload_listener;

    payload_listener.payload_cb = [&](absl::string_view endpoint_id,
        Payload payload) {
            OnPayloadReceived(endpoint_id, payload);
        };

    payload_listener.payload_progress_cb =
        [&](absl::string_view endpoint_id, const PayloadTransferUpdate& update) {
        OnPayloadTransferUpdate(endpoint_id, update);
        };

    nearby_connections_service_->AcceptConnection(
        kServiceId, endpoint_id, std::move(payload_listener),
        [&, endpoint_id = std::string(endpoint_id)](ConnectionsStatus status) {
            NL_VLOG(1) << __func__ << ": Accept connection attempted to endpoint "
                << endpoint_id << " over Nearby Connections with result: "
                << ConnectionsStatusToString(status);
        });
}

void NearbyConnectionsManagerImpl::OnConnectionAccepted(
    absl::string_view endpoint_id)
{
    MutexLock lock(&mutex_);
    auto it = connection_info_map_.find(endpoint_id);
    if (it == connection_info_map_.end()) return;

    if (it->second.is_incoming_connection) {
        if (!incoming_connection_listener_) {
            // Not in advertising mode.
            Disconnect(endpoint_id);
            return;
        }

        auto result = connections_.emplace(std::string(endpoint_id),
            std::make_unique<NearbyConnectionImpl>(std::string(endpoint_id)));
        NL_DCHECK(result.second);
        incoming_connection_listener_->OnIncomingConnection(
            endpoint_id, it->second.endpoint_info, result.first->second.get());
    }
    else {
        auto it = pending_outgoing_connections_.find(endpoint_id);
        if (it == pending_outgoing_connections_.end()) {
            Disconnect(endpoint_id);
            return;
        }

        auto result = connections_.emplace(
            endpoint_id, std::make_unique<NearbyConnectionImpl>(std::string(endpoint_id)));
        NL_DCHECK(result.second);
        std::move(it->second)(result.first->second.get(), Status::kSuccess);
        pending_outgoing_connections_.erase(it);
        connect_timeout_timers_.erase(endpoint_id);
    }
}

void NearbyConnectionsManagerImpl::OnConnectionRejected(
    absl::string_view endpoint_id, Status status) {
    MutexLock lock(&mutex_);
    connection_info_map_.erase(endpoint_id);

    auto it = pending_outgoing_connections_.find(endpoint_id);
    if (it != pending_outgoing_connections_.end()) {
        std::move(it->second)(nullptr, status);
        pending_outgoing_connections_.erase(it);
        connect_timeout_timers_.erase(endpoint_id);
    }
}

void NearbyConnectionsManagerImpl::OnDisconnected(
    absl::string_view endpoint_id) {
    MutexLock lock(&mutex_);
    // Remove transfer manager.
    if (transfer_managers_.contains(endpoint_id)) {
        transfer_managers_[endpoint_id]->CancelTransfer();
        transfer_managers_.erase(endpoint_id);
    }

    Status connection_layer_status = Status::kUnknown;
    auto info_it = connection_info_map_.find(endpoint_id);
    if (info_it != connection_info_map_.end()) {
        connection_layer_status = info_it->second.connection_layer_status;
        connection_info_map_.erase(info_it);
    }

    auto it = pending_outgoing_connections_.find(endpoint_id);
    if (it != pending_outgoing_connections_.end()) {
        std::move(it->second)(nullptr, connection_layer_status);
        pending_outgoing_connections_.erase(it);
        connect_timeout_timers_.erase(endpoint_id);
    }

    connections_.erase(endpoint_id);

    //requested_bwu_endpoint_ids_.erase(endpoint_id);
    //current_upgraded_mediums_.erase(endpoint_id);
}

void NearbyConnectionsManagerImpl::OnBandwidthChanged(
    absl::string_view endpoint_id, Medium medium) {
    MutexLock lock(&mutex_);
    NL_VLOG(1) << __func__
        << ": Bandwidth changed to medium=" << static_cast<int>(medium)
        << "; endpoint_id=" << endpoint_id;

    if (transfer_managers_.contains(endpoint_id)) {
        transfer_managers_[endpoint_id]->OnMediumQualityChanged(medium);
    }

    //current_upgraded_mediums_.insert_or_assign(endpoint_id, medium);
    // TODO(crbug/1111458): Support TransferManager.
}

void NearbyConnectionsManagerImpl::OnPayloadReceived(
    absl::string_view endpoint_id, Payload& payload)
{
    MutexLock lock(&mutex_);
    NL_LOG(INFO) << "Received payload id=" << payload.id;

    [[maybe_unused]] auto result =
        incoming_payloads_.emplace(payload.id, std::move(payload));

    NL_DCHECK(result.second);
}

void NearbyConnectionsManagerImpl::OnPayloadTransferUpdate(
    absl::string_view endpoint_id, const PayloadTransferUpdate& update) {
    MutexLock lock(&mutex_);
    NL_LOG(INFO) << "Received payload transfer update id=" << update.payload_id
        << ",status=" << update.status << ",total=" << update.total_bytes
        << ",bytes_transferred=" << update.bytes_transferred
        << std::endl;

    // If this is a payload we've registered for, then forward its status to
    // the PayloadStatusListener if it still exists. We don't need to do
    // anything more with the payload.
    auto listener_it = payload_status_listeners_.find(update.payload_id);
    if (listener_it != payload_status_listeners_.end()) {
        std::weak_ptr<PayloadStatusListener> listener = listener_it->second;
        switch (update.status) {
        case PayloadStatus::kInProgress:
            break;
        case PayloadStatus::kSuccess:
        case PayloadStatus::kCanceled:
        case PayloadStatus::kFailure:
            payload_status_listeners_.erase(update.payload_id);
            break;
        }
        // Note: The listener might be invalidated, for example, if it is shared
        // with another payload in the same transfer.
        if (auto status_listener = listener.lock()) {
            status_listener->OnStatusUpdate(
                std::make_unique<PayloadTransferUpdate>(update),
                GetUpgradedMedium(endpoint_id));
        }
        return;
    }

    // If this is an incoming payload that we have not registered for, then
    // we'll treat it as a control frame (e.g. IntroductionFrame) and
    // forward it to the associated NearbyConnection.
    auto payload_it = incoming_payloads_.find(update.payload_id);
    if (payload_it == incoming_payloads_.end()) return;

    if (payload_it->second.content.type != PayloadContent::Type::kBytes) {
        NL_LOG(WARNING) << "Received unknown payload of file type. Cancelling.";
        nearby_connections_service_->CancelPayload(kServiceId, payload_it->first,
            [](Status status) {});
        return;
    }

    if (update.status != PayloadStatus::kSuccess) return;

    auto connections_it = connections_.find(endpoint_id);
    if (connections_it == connections_.end()) return;

    NL_LOG(INFO) << "Writing incoming byte message to NearbyConnection.";
    connections_it->second->WriteMessage(
        payload_it->second.content.bytes_payload.bytes);
}

std::optional<Medium> NearbyConnectionsManagerImpl::GetUpgradedMedium(
    absl::string_view endpoint_id) const {
    MutexLock lock(&mutex_);
    const auto it = current_upgraded_mediums_.find(endpoint_id);
    if (it == current_upgraded_mediums_.end()) return std::nullopt;

    return it->second;
}

void NearbyConnectionsManagerImpl::OnConnectionRequested(
    absl::string_view endpoint_id, ConnectionsStatus status) {
    MutexLock lock(&mutex_);
    auto it = pending_outgoing_connections_.find(endpoint_id);
    if (it == pending_outgoing_connections_.end()) return;
    if (status != ConnectionsStatus::kSuccess) {
        NL_LOG(ERROR) << "Failed to connect to the remote shareTarget: "
            << ConnectionsStatusToString(status);
        auto info_it = connection_info_map_.find(endpoint_id);
        if (info_it != connection_info_map_.end()) {
            info_it->second.connection_layer_status = status;
        }
        Disconnect(endpoint_id);
        return;
    }
}

void NearbyConnectionsManagerImpl::Connect(
    std::vector<uint8_t> endpoint_info, absl::string_view endpoint_id,
    std::optional<std::vector<uint8_t>> bluetooth_mac_address,
    DataUsage data_usage, TransportType transport_type,
    NearbyConnectionCallback callback)
{
  MutexLock lock(&mutex_);
  if (!nearby_connections_service_) {
    callback(nullptr, Status::kError);
    return;
  }

  if (bluetooth_mac_address.has_value() && bluetooth_mac_address->size() != 6) {
    bluetooth_mac_address.reset();
  }

  MediumSelection allowed_mediums = MediumSelection(
      /*bluetooth=*/true,
      /*ble=*/true,
      false,
      /*wifi_lan=*/false,
      /*wifi_hotspot=*/false);
  NL_VLOG(1) << __func__ << ": "
             << "data_usage=" << static_cast<int>(data_usage)
             << ", allowed_mediums="
             << MediumSelectionToString(allowed_mediums);
  
  /*[[maybe_unused]] auto result =
      pending_outgoing_connections_.emplace(endpoint_id, std::move(callback));
  NL_DCHECK(result.second);*/

  /*auto timeout_timer = context_->CreateTimer();
  timeout_timer->Start(
      kInitiateNearbyConnectionTimeout / absl::Milliseconds(1), 0,
      [&, endpoint_id]() { OnConnectionTimedOut(endpoint_id); });

  connect_timeout_timers_.emplace(endpoint_id, std::move(timeout_timer));*/

  NearbyConnectionsService::ConnectionListener connection_listener;
  connection_listener.initiated_cb =
      [&](absl::string_view endpoint_id,
          const ConnectionInfo& connection_info) {
        OnConnectionInitiated(endpoint_id, connection_info);
      };
  connection_listener.accepted_cb = [&](absl::string_view endpoint_id) {
    OnConnectionAccepted(endpoint_id);
  };
  connection_listener.rejected_cb = [&](absl::string_view endpoint_id,
                                        Status status) {
    OnConnectionRejected(endpoint_id, status);
  };
  connection_listener.disconnected_cb = [&](absl::string_view endpoint_id) {
    OnDisconnected(endpoint_id);
  };
  connection_listener.bandwidth_changed_cb = [&](absl::string_view endpoint_id,
                                                 Medium medium) {
    OnBandwidthChanged(endpoint_id, medium);
  };

  nearby_connections_service_->RequestConnection(
      kServiceId, endpoint_info, endpoint_id,
      ConnectionOptions(std::move(allowed_mediums),
                        std::move(bluetooth_mac_address),
                        /*keep_alive_interval=*/std::nullopt,
                        /*keep_alive_timeout=*/std::nullopt),
      std::move(connection_listener),
      [&, endpoint_id](ConnectionsStatus status) {
        MutexLock lock(&mutex_);
        if (status != ConnectionsStatus::kSuccess) {
          transfer_managers_.erase(endpoint_id);
        }
        OnConnectionRequested(endpoint_id, status);
      });

  // Setup transfer manager.
  if (transport_type == TransportType::kHighQuality) {
    transfer_managers_[endpoint_id] =
        std::make_unique<TransferManager>(endpoint_id);
  }
}

void NearbyConnectionsManagerImpl::Disconnect(absl::string_view endpoint_id) {
    MutexLock lock(&mutex_);
    if (!pending_outgoing_connections_.contains(endpoint_id) &&
        !connection_info_map_.contains(endpoint_id)) {
        NL_LOG(WARNING) << "No connection for endpoint " << endpoint_id;
        return;
    }

    if (disconnecting_endpoints_.contains(endpoint_id)) {
        NL_LOG(INFO) << "Another Disconnecting is running for endpoint_id "
            << endpoint_id;
        return;
    }

    disconnecting_endpoints_.insert(std::string(endpoint_id));
    nearby_connections_service_->DisconnectFromEndpoint(
        kServiceId, endpoint_id,
        [&, endpoint_id = std::string(endpoint_id)](ConnectionsStatus status) {
            NL_VLOG(1) << __func__ << ": Disconnecting from endpoint "
                << endpoint_id
                << " attempted over Nearby Connections with result: "
                << ConnectionsStatusToString(status);

            /*context_->GetTaskRunner()->PostTask([&, endpoint_id]() {
                OnDisconnected(endpoint_id);
                {
                    MutexLock lock(&mutex_);
                    disconnecting_endpoints_.erase(endpoint_id);
                }
                });*/
            NL_LOG(INFO) << "Disconnected from " << endpoint_id;
        });
}

void NearbyConnectionsManagerImpl::Send(
    absl::string_view endpoint_id, std::unique_ptr<Payload> payload,
    std::weak_ptr<PayloadStatusListener> listener) {
    MutexLock lock(&mutex_);
    if (listener.lock()) {
        RegisterPayloadStatusListener(payload->id, listener);
    }

    if (transfer_managers_.contains(endpoint_id) && payload->content.is_file()) {
        NL_LOG(INFO) << __func__ << ": Send payload " << payload->id << " to "
            << endpoint_id
            << " to transfer manager. payload is file: "
            << payload->content.is_file() << ", is bytes "
            << payload->content.is_bytes();
        transfer_managers_.at(endpoint_id)
            ->Send([&, endpoint_id = std::string(endpoint_id),
                payload_copy = *payload]() {
                    NL_LOG(INFO) << __func__ << ": Send payload " << payload_copy.id
                        << " to " << endpoint_id;
                    auto sent_payload = std::make_unique<Payload>(payload_copy);
                    SendWithoutDelay(endpoint_id, std::move(sent_payload));
                });
        transfer_managers_.at(endpoint_id)->StartTransfer();
        return;
    }

    SendWithoutDelay(endpoint_id, std::move(payload));
}

void NearbyConnectionsManagerImpl::SendWithoutDelay(
    absl::string_view endpoint_id, std::unique_ptr<Payload> payload) {
    NL_LOG(INFO) << __func__ << ": Send payload " << payload->id << " to "
        << endpoint_id;
    nearby_connections_service_->SendPayload(
        kServiceId, { std::string(endpoint_id) }, std::move(payload),
        [endpoint_id = std::string(endpoint_id)](ConnectionsStatus status) {
            NL_LOG(INFO) << __func__ << ": Sending payload to endpoint "
                << endpoint_id
                << " attempted over Nearby Connections with result: "
                << ConnectionsStatusToString(status);
        });
}

void NearbyConnectionsManagerImpl::RegisterPayloadStatusListener(
    int64_t payload_id, std::weak_ptr<PayloadStatusListener> listener) {
    MutexLock lock(&mutex_);
    payload_status_listeners_.insert_or_assign(payload_id, listener);
}

void NearbyConnectionsManagerImpl::RegisterPayloadPath(
    int64_t payload_id, const std::filesystem::path& file_path,
    ConnectionsCallback callback) {
    NL_DCHECK(!file_path.empty());

    // Create file is put into Nearby Connections, don't need to create file in
    // Nearby Sharing.
    callback(Status::kSuccess);
}

void NearbyConnectionsManagerImpl::ClearIncomingPayloads() {
    MutexLock lock(&mutex_);
    std::vector<Payload> payloads;
    for (auto& it : incoming_payloads_) {
        payloads.push_back(std::move(it.second));
        payload_status_listeners_.erase(it.first);
    }

    incoming_payloads_.clear();
}

Payload* NearbyConnectionsManagerImpl::GetIncomingPayload(int64_t payload_id) {
    MutexLock lock(&mutex_);
    auto it = incoming_payloads_.find(payload_id);
    if (it == incoming_payloads_.end()) return nullptr;

    return &it->second;
}

std::optional<std::vector<uint8_t>>
NearbyConnectionsManagerImpl::GetRawAuthenticationToken(
    absl::string_view endpoint_id) {
    MutexLock lock(&mutex_);
    auto it = connection_info_map_.find(endpoint_id);
    if (it == connection_info_map_.end()) return std::nullopt;

    return it->second.raw_authentication_token;
}

}  // namespace sharing
}  // namespace nearby
