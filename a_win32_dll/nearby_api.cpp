
#include "nearby_api.h"

#include "nearby_sharing_service_factory.h"

using namespace nearby::sharing;

namespace nearby::windows
{
	void NearbyShareAPI::InitializeNearby()
	{
		auto factory = NearbySharingServiceFactory::GetInstance();
		nearby_sharing_service_ = factory->CreateSharingService();
	}

	void NearbyShareAPI::StartScanning()
	{
		nearby_sharing_service_->StartScanning();
	}

	void NearbyShareAPI::StartScanning2(DeviceAddedCallback deviceAddedCallback)
	{
		nearby_sharing_service_->StartScanning(deviceAddedCallback);
	}
	
	void NearbyShareAPI::StartAdvertising()
	{

	}


}