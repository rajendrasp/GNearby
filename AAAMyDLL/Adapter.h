#pragma once

#include "dll_config.h"

#include "connections/c/advertising_options_w.h"
#include "connections/c/params_w.h"
#include "connections/c/discovery_options_w.h"

namespace nearby::connections {
	class Core;
	class ServiceController;
	class ServiceControllerRouter;
	class OfflineServiceController;
}  // namespace nearby::connections
namespace nearby::windows {

	extern "C" {

		using Core = connections::Core;
		using ServiceControllerRouter = connections::ServiceControllerRouter;

		// Initializes a Core instance, providing the ServiceController factory from
		// app side. If no factory is provided, it will initialize a new
		// factory creating OfflineServiceController.
		// Returns the instance handle to c# client.
		// TODO(jfcarroll): Is this method needed? The trouble is we can't
		// new up a forward declared class (OfflineServiceController). If this
		// is necessary, must find another way to implement it.
		// DLL_API Core *__stdcall InitCoreWithServiceControllerFactory(
		//    std::function<ServiceController *()> factory = []() {
		//      return new OfflineServiceController;
		//    });

		// Initializes a default Core instance.
		// Returns the instance handle to c# client.
		DLL_API Core* __stdcall InitCore(ServiceControllerRouter*);

		// Closes the core with stopping all endpoints, then free the memory.
		DLL_API void __stdcall CloseCore(Core*);

		// Initializes a default ServiceControllerRouter instance.
		// Returns the instance handle to c# client.
		DLL_API ServiceControllerRouter* __stdcall InitServiceControllerRouter();

		// Close a ServiceControllerRouter instance.
		DLL_API void __stdcall CloseServiceControllerRouter(ServiceControllerRouter*);

		// Starts advertising an endpoint for a local app.
		//
		// service_id - An identifier to advertise your app to other endpoints.
		//              This can be an arbitrary string, so long as it uniquely
		//              identifies your service. A good default is to use your
		//              app's package name.
		// advertising_options - The options for advertising.
		// info       - Connection parameters:
		// > name     - A human readable name for this endpoint, to appear on
		//              other devices.
		// > listener - A callback notified when remote endpoints request a
		//              connection to this endpoint.
		// callback - to access the status of the operation when available.
		//   Possible status codes include:
		//     Status::STATUS_OK if advertising started successfully.
		//     Status::STATUS_ALREADY_ADVERTISING if the app is already advertising.
		//     Status::STATUS_OUT_OF_ORDER_API_CALL if the app is currently
		//         connected to remote endpoints; call StopAllEndpoints first.
		DLL_API void __stdcall StartAdvertising(Core*, const char*, AdvertisingOptionsW,
			ConnectionRequestInfoW,
			ResultCallbackW);

		// Stops advertising a local endpoint. Should be called after calling
		// StartAdvertising, as soon as the application no longer needs to advertise
		// itself or goes inactive. Payloads can still be sent to connected
		// endpoints after advertising ends.
		//
		// result_cb - to access the status of the operation when available.
		//   Possible status codes include:
		//     Status::STATUS_OK if none of the above errors occurred.
		DLL_API void __stdcall StopAdvertising(Core*, ResultCallbackW);

		// Starts discovery for remote endpoints with the specified service ID.
		//
		// service_id - The ID for the service to be discovered, as specified in
		//              the corresponding call to StartAdvertising.
		// listener   - A callback notified when a remote endpoint is discovered.
		// discovery_options - The options for discovery.
		// result_cb  - to access the status of the operation when available.
		//   Possible status codes include:
		//     Status::STATUS_OK if discovery started successfully.
		//     Status::STATUS_ALREADY_DISCOVERING if the app is already
		//         discovering the specified service.
		//     Status::STATUS_OUT_OF_ORDER_API_CALL if the app is currently
		//         connected to remote endpoints; call StopAllEndpoints first.
		DLL_API void __stdcall StartDiscovery(Core*, const char*, DiscoveryOptionsW,
			DiscoveryListenerW, ResultCallbackW);

		// Stops discovery for remote endpoints, after a previous call to
		// StartDiscovery, when the client no longer needs to discover endpoints or
		// goes inactive. Payloads can still be sent to connected endpoints after
		// discovery ends.
		//
		// result_cb - to access the status of the operation when available.
		//   Possible status codes include:
		//     Status::STATUS_OK if none of the above errors occurred.
		DLL_API void __stdcall StopDiscovery(Core*, ResultCallbackW);
	}
}