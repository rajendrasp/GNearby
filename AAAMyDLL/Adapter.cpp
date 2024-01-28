
#include "Adapter.h"
#include "connections/core.h"

#include "connections/c/file_w.h"

namespace nearby::windows
{
	Core* InitCore(connections::ServiceControllerRouter* router)
	{
		return new nearby::connections::Core(router);
	}

    void CloseCore(Core* pCore)
    {
        if (pCore == nullptr)
        {
            return;
        }
        
        pCore->StopAllEndpoints([](Status) {});
        delete pCore;
    }

    connections::ServiceControllerRouter* InitServiceControllerRouter()
    {
        return new connections::ServiceControllerRouter();
    }

    void CloseServiceControllerRouter(
        connections::ServiceControllerRouter* pRouter)
    {
        if (pRouter != nullptr)
        {
            delete pRouter;
        }
    }

    void StartAdvertising(Core* pCore, const char* service_id,
        AdvertisingOptionsW advertising_options_w,
        ConnectionRequestInfoW info, ResultCallbackW callback)
    {
        if (pCore == nullptr)
        {
            return;
        }

        connections::ConnectionRequestInfo crInfo;

        crInfo.endpoint_info = ByteArray(info.endpoint_info);
        crInfo.listener = std::move(*(info.listener.GetImpl()));

        connections::AdvertisingOptions advertising_options;

        advertising_options.allowed.bluetooth =
            advertising_options_w.allowed.bluetooth;
        advertising_options.allowed.ble = advertising_options_w.allowed.ble;
        advertising_options.allowed.wifi_lan = advertising_options_w.allowed.wifi_lan;
        advertising_options.allowed.web_rtc = advertising_options_w.allowed.web_rtc;
        advertising_options.allowed.wifi_hotspot =
            advertising_options_w.allowed.wifi_hotspot;
        advertising_options.enable_bluetooth_listening = false;
        advertising_options.enable_webrtc_listening = false;
        advertising_options.auto_upgrade_bandwidth =
            advertising_options_w.auto_upgrade_bandwidth;
        advertising_options.enforce_topology_constraints =
            advertising_options_w.enforce_topology_constraints;
        if (advertising_options_w.fast_advertisement_service_uuid != nullptr)
        {
            advertising_options.fast_advertisement_service_uuid =
                std::string(advertising_options_w.fast_advertisement_service_uuid);
        }

        advertising_options.is_out_of_band_connection =
            advertising_options_w.is_out_of_band_connection;
        advertising_options.low_power = advertising_options_w.low_power;
        if (advertising_options_w.strategy == StrategyW::kNone)
            advertising_options.strategy = connections::Strategy::kNone;
        if (advertising_options_w.strategy == StrategyW::kP2pCluster)
            advertising_options.strategy = connections::Strategy::kP2pCluster;
        if (advertising_options_w.strategy == StrategyW::kP2pPointToPoint)
            advertising_options.strategy = connections::Strategy::kP2pPointToPoint;
        if (advertising_options_w.strategy == StrategyW::kP2pStar)
            advertising_options.strategy = connections::Strategy::kP2pStar;

        pCore->StartAdvertising(service_id, advertising_options, crInfo,
            std::move(*callback.GetImpl()));
    }

    void StopAdvertising(connections::Core* pCore, ResultCallbackW callback)
    {
        if (pCore == nullptr)
        {
            return;
        }
        pCore->StopAdvertising(std::move(*callback.GetImpl()));
    }

    void StartDiscovery(connections::Core* pCore, const char* service_id,
        DiscoveryOptionsW discovery_options_w,
        DiscoveryListenerW listener, ResultCallbackW callback)
    {
        if (pCore == nullptr)
        {
            return;
        }

        connections::DiscoveryListener discoveryListener =
            std::move(*listener.GetImpl());

        connections::DiscoveryOptions discovery_options;

        if (discovery_options_w.strategy == StrategyW::kNone)
            discovery_options.strategy = connections::Strategy::kNone;
        if (discovery_options_w.strategy == StrategyW::kP2pCluster)
            discovery_options.strategy = connections::Strategy::kP2pCluster;
        if (discovery_options_w.strategy == StrategyW::kP2pPointToPoint)
            discovery_options.strategy = connections::Strategy::kP2pPointToPoint;
        if (discovery_options_w.strategy == StrategyW::kP2pStar)
            discovery_options.strategy = connections::Strategy::kP2pStar;
        discovery_options.auto_upgrade_bandwidth =
            discovery_options_w.auto_upgrade_bandwidth;
        discovery_options.enforce_topology_constraints =
            discovery_options_w.enforce_topology_constraints;
        discovery_options.is_out_of_band_connection =
            discovery_options_w.is_out_of_band_connection;
        if (discovery_options_w.fast_advertisement_service_uuid)
        {
            discovery_options.fast_advertisement_service_uuid =
                std::string(discovery_options_w.fast_advertisement_service_uuid);
        }
        discovery_options.allowed.bluetooth = discovery_options_w.allowed.bluetooth;
        discovery_options.allowed.ble = discovery_options_w.allowed.ble;
        discovery_options.allowed.wifi_lan = discovery_options_w.allowed.wifi_lan;
        discovery_options.allowed.wifi_hotspot =
            discovery_options_w.allowed.wifi_hotspot;
        discovery_options.allowed.web_rtc = discovery_options_w.allowed.web_rtc;

        pCore->StartDiscovery(service_id, discovery_options, discoveryListener,
            std::move(*callback.GetImpl()));
    }

    void StopDiscovery(connections::Core* pCore, ResultCallbackW callback)
    {
        if (pCore == nullptr)
        {
            return;
        }
        pCore->StopDiscovery(std::move(*callback.GetImpl()));
    }
}
