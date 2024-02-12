
#include "nearby_api.h"

#include "nearby_sharing_service_factory.h"

using namespace nearby::sharing;

namespace nearby::windows
{
	void NearbyShareAPI::InitializeNearby()
	{
		auto factory = NearbySharingServiceFactory::GetInstance();
		nearby_sharing_service_.reset(factory->CreateSharingService());
	}

	void NearbyShareAPI::StartScanning()
	{
		nearby_sharing_service_->StartScanning();
	}
	
	void NearbyShareAPI::StartAdvertising()
	{

	}


}