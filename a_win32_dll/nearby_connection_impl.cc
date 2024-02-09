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

#include "nearby_connection_impl.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <queue>
#include <string>
#include <utility>
#include <vector>

#include "absl/strings/string_view.h"
#include "internal/platform/mutex_lock.h"
#include "logging.h"
#include "nearby_connection.h"
#include "sharing/proto/wire_format.pb.h"
#include "nearby_sharing_decoder_impl.h"


namespace nearby {
	namespace sharing {

		using FrameType = ::nearby::sharing::service::proto::V1Frame_FrameType;
		using V1Frame = ::nearby::sharing::service::proto::V1Frame;
		using Frame = ::nearby::sharing::service::proto::Frame;

		constexpr absl::Duration kReadFramesTimeout = absl::Seconds(15);

		class IncomingFramesReader : public std::enable_shared_from_this<IncomingFramesReader>
		{
		public:

			struct ReadFrameInfo {
				std::optional<nearby::sharing::service::proto::V1Frame_FrameType>
					frame_type = std::nullopt;
				std::function<void(std::optional<nearby::sharing::service::proto::V1Frame>)>
					callback = nullptr;
				std::optional<absl::Duration> timeout = std::nullopt;
			};

			RecursiveMutex mutex_2;
			NearbyConnectionImpl* connection_ = nullptr;
			std::queue<ReadFrameInfo> read_frame_info_queue_;

			// Caches frames read from NearbyConnection which are not used immediately.
			std::map<nearby::sharing::service::proto::V1Frame_FrameType,
				std::optional<nearby::sharing::service::proto::V1Frame>>
				cached_frames_;


			std::weak_ptr<IncomingFramesReader> GetWeakPtr() {
				return this->weak_from_this();
			}

			void ReadNextFrame() {
				connection_->Read(
					[&, reader = GetWeakPtr()](std::optional<std::vector<uint8_t>> bytes) {
						auto frame_reader = reader.lock();
						if (frame_reader == nullptr) {
							NL_LOG(WARNING) << "IncomingFramesReader is released before.";
							return;
						}

						OnDataReadFromConnection(std::move(bytes));
					});
			}

			void OnFrameDecoded(std::optional<Frame> frame) {
				if (!frame.has_value()) {
					ReadNextFrame();
					return;
				}

				if (frame->version() != Frame::V1) {
					ReadNextFrame();
					return;
				}

				auto v1_frame = frame->v1();
				FrameType v1_frame_type = v1_frame.type();

				const ReadFrameInfo& frame_info = read_frame_info_queue_.front();

				if (frame_info.frame_type.has_value() && *frame_info.frame_type != v1_frame_type)
				{
					NL_LOG(WARNING) << __func__ << ": Failed to read frame of type "
						<< *frame_info.frame_type << ", but got frame of type "
						<< v1_frame_type << ". Cached for later.";

					cached_frames_.insert({ v1_frame_type, std::move(v1_frame) });
					ReadNextFrame();
					return;
				}

				Done(std::move(v1_frame));
			}

			void Done(std::optional<V1Frame> frame) {
				if (read_frame_info_queue_.empty()) {
					return;
				}

				/*if (timeout_timer_ != nullptr) {
					timeout_timer_->Stop();
				}*/

				bool is_empty_frame = !frame.has_value();
				ReadFrameInfo read_frame_info = std::move(read_frame_info_queue_.front());
				read_frame_info_queue_.pop();
				read_frame_info.callback(std::move(frame));

				if (is_empty_frame) {
					// should complete all pending readers.
					while (!read_frame_info_queue_.empty()) {
						read_frame_info = std::move(read_frame_info_queue_.front());
						read_frame_info_queue_.pop();
						read_frame_info.callback(std::nullopt);
					}
					return;
				}

				if (!read_frame_info_queue_.empty()) {
					ReadFrameInfo read_frame_info = std::move(read_frame_info_queue_.front());
					read_frame_info_queue_.pop();

					if (read_frame_info.timeout.has_value()) {
						ReadFrame(*read_frame_info.frame_type,
							std::move(read_frame_info.callback), *read_frame_info.timeout);
					}
					else {
						ReadFrame(std::move(read_frame_info.callback));
					}
				}
			}

			void ReadFrame(FrameType frame_type, std::function<void(std::optional<V1Frame>)> callback,
				absl::Duration timeout)
			{
				MutexLock lock(&mutex_2);
				if (!read_frame_info_queue_.empty()) {
					ReadFrameInfo read_fame_info{ frame_type, std::move(callback), timeout };
					read_frame_info_queue_.push(std::move(read_fame_info));
					return;
				}

				// Check in the cache for frame.
				std::optional<V1Frame> cached_frame = GetCachedFrame(frame_type);
				if (cached_frame.has_value()) {
					callback(std::move(cached_frame));
					return;
				}

				ReadFrameInfo read_fame_info{ frame_type, std::move(callback), timeout };
				read_frame_info_queue_.push(std::move(read_fame_info));

				ReadNextFrame();
			}

			void ReadFrame(std::function<void(std::optional<V1Frame>)> callback)
			{
				MutexLock lock(&mutex_2);
				if (!read_frame_info_queue_.empty()) {
					ReadFrameInfo read_fame_info{ std::nullopt, std::move(callback),
												 std::nullopt };
					read_frame_info_queue_.push(std::move(read_fame_info));
					return;
				}

				// Check in the cache for frame.
				std::optional<V1Frame> cached_frame = GetCachedFrame(std::nullopt);
				if (cached_frame.has_value()) {
					callback(std::move(cached_frame));
					return;
				}

				ReadFrameInfo read_fame_info{ std::nullopt, std::move(callback), std::nullopt };
				read_frame_info_queue_.push(std::move(read_fame_info));
				ReadNextFrame();
			}

			std::optional<V1Frame> GetCachedFrame(
				std::optional<nearby::sharing::service::proto::V1Frame_FrameType>
				frame_type)
			{
				//if (frame_type.has_value())
				//	NL_VLOG(1) << __func__ << ": Requested frame type - " << *frame_type;

				auto iter = frame_type.has_value() ? cached_frames_.find(*frame_type)
					: cached_frames_.begin();

				if (iter == cached_frames_.end()) return std::nullopt;

				std::optional<V1Frame> frame = std::move(iter->second);
				cached_frames_.erase(iter);
				return frame;
			}

			void OnDataReadFromConnection(std::optional<std::vector<uint8_t>> bytes)
			{
				MutexLock lock(&mutex_2);

				NearbySharingDecoderImpl decoder;
				std::unique_ptr<Frame> frame =
					decoder.DecodeFrame(*bytes);
				if (frame == nullptr) {
					NL_LOG(WARNING)
						<< __func__
						<< ": Cannot decode frame. Not currently bound to nearby process";
					return;
				}

				OnFrameDecoded(std::move(*frame));
			}
		};

		IncomingFramesReader reader;

		NearbyConnectionImpl::NearbyConnectionImpl(std::string endpoint_id)
			: endpoint_id_(endpoint_id)
		{
		}

		NearbyConnectionImpl::~NearbyConnectionImpl() {
			MutexLock lock(&mutex_);

			if (disconnect_listener_) {
				disconnect_listener_();
			}

			if (read_callback_) {
				read_callback_(std::nullopt);
			}
		}

		void NearbyConnectionImpl::ReceiveIntroduction()
		{
			reader.connection_ = this;
			reader.ReadFrame(nearby::sharing::service::proto::V1Frame::INTRODUCTION,
				[&](std::optional<nearby::sharing::service::proto::V1Frame> frame)
				{
					OnReceivedIntroduction();
				}, kReadFramesTimeout);
		}

		void NearbyConnectionImpl::OnReceivedIntroduction()
		{
			std::cout << "Received intro" << std::endl;
		}

		void NearbyConnectionImpl::Read(ReadCallback callback) {
			MutexLock lock(&mutex_);
			if (reads_.empty()) {
				read_callback_ = std::move(callback);
				return;
			}

			std::vector<uint8_t> bytes = std::move(reads_.front());
			reads_.pop();
			std::move(callback)(std::move(bytes));
		}

		void NearbyConnectionImpl::Write(std::vector<uint8_t> bytes) {
			//MutexLock lock(&mutex_);
			//Payload payload(bytes);
			//nearby_connections_manager_->Send(
			//    endpoint_id_, std::make_unique<Payload>(payload),
			//    /*listener=*/
			//    std::weak_ptr<NearbyConnectionsManager::PayloadStatusListener>());
		}

		void NearbyConnectionImpl::Close() {
			//MutexLock lock(&mutex_);
			//// As [this] therefore endpoint_id_ will be destroyed in Disconnect, make a
			//// copy of [endpoint_id] as the parameter is a const ref.
			//nearby_connections_manager_->Disconnect(endpoint_id_);
		}

		void NearbyConnectionImpl::SetDisconnectionListener(
			std::function<void()> listener) {
			MutexLock lock(&mutex_);
			disconnect_listener_ = std::move(listener);
		}

		void NearbyConnectionImpl::WriteMessage(std::vector<uint8_t> bytes) {
			MutexLock lock(&mutex_);
			if (read_callback_) {
				auto callback = std::move(read_callback_);
				read_callback_ = nullptr;
				callback(std::move(bytes));
				return;
			}

			reads_.push(std::move(bytes));
		}

	}  // namespace sharing
}  // namespace nearby
