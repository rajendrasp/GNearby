#pragma once

#include "dll_config.h"
#include <memory>
#include <string>
#include <functional>

namespace nearby {
	namespace sharing {
		class NearbySharingService;
	}
}

using DeviceAddedCallback = std::function<void(std::string device_name, std::string endpoint_id)>;

namespace nearby::windows {

	extern "C" {

		class DLL_API NearbyShareAPI {
		public:
			void __stdcall InitializeNearby();

			void StartScanning();
			void StartScanning2(DeviceAddedCallback deviceAddedCallback);
			void StartAdvertising();


			nearby::sharing::NearbySharingService* nearby_sharing_service_;
		};
	}
}
