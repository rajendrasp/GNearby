#ifndef CORE_INTERNAL_MEDIUMS_WEBRTC_PEER_CONNECTION_OBSERVER_IMPL_H_
#define CORE_INTERNAL_MEDIUMS_WEBRTC_PEER_CONNECTION_OBSERVER_IMPL_H_

#include "core/internal/mediums/webrtc/local_ice_candidate_listener.h"
#include "platform/public/single_thread_executor.h"
#include "webrtc/api/peer_connection_interface.h"

namespace location {
namespace nearby {
namespace connections {
namespace mediums {

class ConnectionFlow;

class PeerConnectionObserverImpl : public webrtc::PeerConnectionObserver {
 public:
  PeerConnectionObserverImpl(
      ConnectionFlow* connection_flow,
      LocalIceCandidateListener local_ice_candidate_listener);
  ~PeerConnectionObserverImpl() override;

  // webrtc::PeerConnectionObserver:
  void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;
  void OnSignalingChange(
      webrtc::PeerConnectionInterface::SignalingState new_state) override;
  void OnDataChannel(
      rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) override;
  void OnIceGatheringChange(
      webrtc::PeerConnectionInterface::IceGatheringState new_state) override;
  void OnConnectionChange(
      webrtc::PeerConnectionInterface::PeerConnectionState new_state) override;
  void OnRenegotiationNeeded() override;

  void Shutdown();

 private:
  void OffloadFromSignalingThread(Runnable runnable);

  ConnectionFlow* volatile connection_flow_;
  LocalIceCandidateListener local_ice_candidate_listener_;
  SingleThreadExecutor single_threaded_signaling_offloader_;
};

}  // namespace mediums
}  // namespace connections
}  // namespace nearby
}  // namespace location

#endif  // CORE_INTERNAL_MEDIUMS_WEBRTC_PEER_CONNECTION_OBSERVER_IMPL_H_
