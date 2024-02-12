#pragma once

#include "dll_config.h"
#include "nearby_sharing_service.h"

using namespace nearby::sharing;

namespace nearby::windows {

	extern "C" {

		class DLL_API NearbyShareAPI {
		public:
			void __stdcall InitializeNearby();

			void StartScanning();
			void StartAdvertising();


			std::unique_ptr<NearbySharingService> nearby_sharing_service_;
		};
	}
}
