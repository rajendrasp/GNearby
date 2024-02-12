
#include "hardcoded.h"


using namespace nearby::sharing;
using ::nearby::sharing::proto::DataUsage;
using ::location::nearby::proto::sharing::OSType;

std::optional<std::vector<uint8_t>> GetBluetoothMacAddressForShareTarget(ShareTarget share_target)
{
    std::string mac_address =
        //certificate.unencrypted_metadata().bluetooth_mac_address();
        "sixlen";
    if (mac_address.size() != 6) {
        return std::nullopt;
    }

    return std::vector<uint8_t>(mac_address.begin(), mac_address.end());
}

std::string GetDeviceName()
{
    return "RajWindows";
}

nearby::sharing::ShareTargetType GetDeviceType()
{
    return nearby::sharing::ShareTargetType::kLaptop;
}

std::optional<std::filesystem::path> GetDownloadPath() {
    PWSTR path;
    HRESULT result =
        SHGetKnownFolderPath(FOLDERID_Downloads, KF_FLAG_DEFAULT, nullptr, &path);
    if (result == S_OK) {
        std::wstring download_path{ path };
        CoTaskMemFree(path);
        return std::filesystem::path(download_path);
    }

    CoTaskMemFree(path);
    return std::nullopt;
}

DataUsage GetDataUsage()
{
    return DataUsage::WIFI_ONLY_DATA_USAGE;
}


void RunVerification(std::function<void(OSType)> callback)
{
    callback(OSType::WINDOWS);
}

bool IsExtendedAdvertisingSupported()
{
    return true;
}