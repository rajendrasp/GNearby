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

#include<AAAMyDLL/Adapter.h>

using namespace nearby;
using namespace nearby::windows;

void ListenerEndpointFoundCB(const char* endpoint_id, const char* endpoint_info,
    size_t endpoint_info_size,
    const char* str_service_id)
{
    std::cout << "Found endpoint " << endpoint_id << " on service " << str_service_id << std::endl;
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

void ResultCB(Status status)
{
    (void)status;  // Avoid unused parameter warning
    std::cout << "Status: " << std::endl;
}

int main()
{
    auto router = InitServiceControllerRouter();
    auto core = InitCore(router);

    /*AdvertisingOptionsW a_options;
    a_options.strategy = StrategyW::kP2pCluster;
    a_options.device_info = "connectionsd";;
    a_options.low_power = false;
    a_options.allowed.ble = true;
    a_options.allowed.web_rtc = false;

    ConnectionOptionsW con_req_info;*/

    DiscoveryOptionsW d_options;
    d_options.strategy = StrategyW::kP2pCluster;
    d_options.allowed.ble = true;
    d_options.allowed.web_rtc = false;

    DiscoveryListenerW listener(ListenerEndpointFoundCB, ListenerEndpointLostCB,
        ListenerEndpointDistanceChangedCB);

    ResultCallbackW callback;
    callback.result_cb = ResultCB;

    StartDiscovery(core, "rajendra", d_options, listener, callback);

    while (true) {
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
