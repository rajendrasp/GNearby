// Copyright 2023 Google LLC
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

#define NOMINMAX
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <windows.h>
#include <algorithm>
#include <memory>

#include <AAAMyDLL/Adapter.h>
#include <AAAMyDLL/endpointInfo.h>

using namespace nearby;
using namespace nearby::windows;

DWORD WINAPI MyThreadFunction(LPVOID lpParam);

#define MAX_THREADS 5
DWORD   dwThreadIdArray[MAX_THREADS];
HANDLE  hThreadArray[MAX_THREADS] = {};

const char* nameInEndpointIfo = "Ra";
//const char* local_fast_advertisement_service_uuid = "0000fef3-0000-1000-8000-00805f9b34fb";
const char* local_fast_advertisement_service_uuid = nullptr;

std::string connect_endpoint_id;

void ListenerEndpointFoundCB(const char* endpoint_id, const char* endpoint_info,
    size_t endpoint_info_size,
    const char* str_service_id)
{
    std::cout << "Found endpoint " << endpoint_id << " on service " << str_service_id << std::endl;

    connect_endpoint_id = endpoint_id;

    hThreadArray[0] = CreateThread(
        NULL,                   // default security attributes
        0,                      // use default stack size  
        MyThreadFunction,       // thread function name
        NULL,          // argument to thread function 
        0,                      // use default creation flags 
        &dwThreadIdArray[0]);   // returns the thread identifier 
}

void ListenerEndpointLostCB(const char* endpoint_id)
{
    std::cout << "Device lost: " << endpoint_id << std::endl;
}

void ListenerEndpointDistanceChangedCB(const char* endpoint_id,
    DistanceInfoW distance_info)
{
    std::cout << "Device distance changed: " << endpoint_id << std::endl;
}

void ResultCBMee(Status status)
{
    (void)status;  // Avoid unused parameter warning
    std::cout << "Status: " << std::endl;
}

void ListenerInitiatedCB(
    const char* endpoint_id,
    const ConnectionResponseInfoW& connection_response_info)
{
    std::cout << "Advertising initiated: " << endpoint_id << std::endl;
}

void ListenerAcceptedCB(const char* endpoint_id)
{
    std::cout << "Advertising accepted: " << endpoint_id << std::endl;
}

void ListenerRejectedCB(const char* endpoint_id, connections::Status status)
{
    std::cout << "Advertising rejected: " << endpoint_id << std::endl;
}

void ListenerDisconnectedCB(const char* endpoint_id)
{
    std::cout << "Advertising disconnected: " << endpoint_id << std::endl;
}

void ListenerBandwidthChangedCB(const char* endpoint_id, MediumW medium)
{
    std::cout << "Advertising bandwidth changed: " << endpoint_id << std::endl;
}

Core* core = nullptr;
ResultCallbackW connectResultCallback;

DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{
    ConnectionOptionsW connection_options;
    connection_options.allowed.ble = true;
    connection_options.allowed.bluetooth = true;
    connection_options.allowed.web_rtc = false;
    connection_options.allowed.wifi_lan = false;
    connection_options.allowed.wifi_direct = false;
    connection_options.remote_bluetooth_mac_address = "ac:5f:ea:38:eb:e9";
    connection_options.enforce_topology_constraints = false;
    connection_options.fast_advertisement_service_uuid = local_fast_advertisement_service_uuid;
    
    ConnectionListenerW clistener(ListenerInitiatedCB, ListenerAcceptedCB,
        ListenerRejectedCB, ListenerDisconnectedCB,
        ListenerBandwidthChangedCB);

    //auto infoo = "NearbySharing";

    auto endpointInfo = CreateEndpointInfo(nameInEndpointIfo, ShareTargetType::kLaptop);
    std::string infoo = std::string(endpointInfo->begin(), endpointInfo->end());
    ConnectionRequestInfoW info{ infoo.c_str(), infoo.length(), clistener };;

    connectResultCallback.result_cb = ResultCBMee;

    RequestConnection(core, connect_endpoint_id.c_str(), info, connection_options, connectResultCallback);
    
    while (true)
    {
        Sleep(10);
    }

    return 0;
}

int main()
{
    auto router = InitServiceControllerRouter();
    core = InitCore(router);

    AdvertisingOptionsW a_options;
    a_options.strategy = StrategyW::kP2pPointToPoint;
    a_options.device_info = "connectionsd";;
    a_options.allowed.ble = true;
    a_options.allowed.bluetooth = true;
    a_options.allowed.wifi_lan = false;
    a_options.allowed.wifi_direct = false;
    a_options.allowed.wifi_hotspot = false;
    a_options.allowed.web_rtc = false;
    a_options.low_power = false;
    a_options.auto_upgrade_bandwidth = false;
    a_options.enforce_topology_constraints = true;
    a_options.enable_bluetooth_listening = true;
    

    a_options.fast_advertisement_service_uuid = local_fast_advertisement_service_uuid;

    ConnectionListenerW clistener(ListenerInitiatedCB, ListenerAcceptedCB,
        ListenerRejectedCB, ListenerDisconnectedCB,
        ListenerBandwidthChangedCB);

    //auto infoo = "NearbySharing";
    
    auto endpointInfo = CreateEndpointInfo(nameInEndpointIfo, ShareTargetType::kLaptop);
    std::string infoo = std::string(endpointInfo->begin(), endpointInfo->end());
    ConnectionRequestInfoW info{ infoo.c_str(), infoo.length(), clistener};;


    //{infoo, strlen(infoo), clistener };

    ResultCallbackW callback2;
    callback2.result_cb = ResultCBMee;

    StartAdvertising(core, "NearbySharing", a_options, info, callback2);

    DiscoveryOptionsW d_options;
    d_options.strategy = StrategyW::kP2pPointToPoint;
    d_options.allowed.ble = true;
    d_options.allowed.bluetooth = true;
    d_options.allowed.wifi_lan = false;
    d_options.allowed.wifi_direct = false;
    d_options.allowed.wifi_hotspot = false;
    d_options.allowed.web_rtc = false;
    d_options.fast_advertisement_service_uuid = "0000fef3-0000-1000-8000-00805f9b34fb";
    d_options.is_out_of_band_connection = false;

    DiscoveryListenerW dlistener(ListenerEndpointFoundCB, ListenerEndpointLostCB,
        ListenerEndpointDistanceChangedCB);

    ResultCallbackW callback;
    callback.result_cb = ResultCBMee;

    StartDiscovery(core, "NearbySharing", d_options, dlistener, callback);

    std::cout << "=====================================================================================================" << std::endl;
    std::cout << "=====================================================================================================" << std::endl;
    std::cout << "=====================================================================================================" << std::endl;

    while (true) {
        WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);
        Sleep(10);
    }



    /*auto core = Core(router.get());

    AdvertisingOptions a_options;
    a_options.strategy = Strategy::kP2pCluster;
    a_options.device_info = "connectionsd";
    a_options.enable_bluetooth_listening = true;
    a_options.low_power = false;
    a_options.allowed.ble = false;
    a_options.allowed.web_rtc = false;

    ConnectionRequestInfo con_req_info;
    con_req_info.listener.accepted_cb = [](const std::string& endpoint_id) {
        std::cout << "Accepted new endpoint: " << endpoint_id << std::endl;
        };

    core.StartAdvertising(
        "com.google.nearby.connectionsd", a_options.CompatibleOptions(),
        con_req_info, [](Status status) {
            std::cout << "Advertising result: " << status.ToString() << std::endl;
        });

    DiscoveryOptions d_options;
    d_options.strategy = Strategy::kP2pCluster;
    d_options.allowed.ble = false;
    d_options.allowed.web_rtc = false;

    DiscoveryListener d_listener;

    d_listener.endpoint_found_cb = [](const std::string& endpoint,
        const ByteArray& info,
        const std::string& service_id) {
            std::cout << "Found endpoint " << endpoint << " on service " << service_id;
        };
    core.StartDiscovery(
        "com.google.nearby.connectionsd", d_options.CompatibleOptions(),
        d_listener, [](Status status) {
            std::cout << "Discovery status: " << status.ToString() << std::endl;
        });*/

   
    return 0;
}
