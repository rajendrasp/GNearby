#pragma once

#include <memory>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <Shlobj.h>

#include "nearby_share_enums.h"
#include "share_target.h"
#include "sharing/proto/enums.pb.h"


using namespace nearby::sharing;
using ::nearby::sharing::proto::DataUsage;
using ::location::nearby::proto::sharing::OSType;

std::optional<std::vector<uint8_t>> GetBluetoothMacAddressForShareTarget(ShareTarget share_target);
std::string GetDeviceName();
nearby::sharing::ShareTargetType GetDeviceType();
std::optional<std::filesystem::path> GetDownloadPath();
DataUsage GetDataUsage();
void RunVerification(std::function<void(OSType)> callback);
bool IsExtendedAdvertisingSupported();