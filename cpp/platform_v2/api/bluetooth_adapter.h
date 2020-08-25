#ifndef PLATFORM_V2_API_BLUETOOTH_ADAPTER_H_
#define PLATFORM_V2_API_BLUETOOTH_ADAPTER_H_

#include <string>

#include "absl/strings/string_view.h"

namespace location {
namespace nearby {
namespace api {

// https://developer.android.com/reference/android/bluetooth/BluetoothAdapter.html
class BluetoothAdapter {
 public:
  virtual ~BluetoothAdapter() = default;

  // Eligible statuses of the BluetoothAdapter.
  enum class Status {
    kDisabled,
    kEnabled,
  };

  // Synchronously sets the status of the BluetoothAdapter to 'status', and
  // returns true if the operation was a success.
  virtual bool SetStatus(Status status) = 0;
  // Returns true if the BluetoothAdapter's current status is
  // Status::Value::kEnabled.
  virtual bool IsEnabled() const = 0;

  // Scan modes of a BluetoothAdapter, as described at
  // https://developer.android.com/reference/android/bluetooth/BluetoothAdapter.html#getScanMode().
  enum class ScanMode {
    kUnknown,
    kNone,
    kConnectable,
    kConnectableDiscoverable,
  };

  // https://developer.android.com/reference/android/bluetooth/BluetoothAdapter.html#getScanMode()
  //
  // Returns ScanMode::kUnknown on error.
  virtual ScanMode GetScanMode() const = 0;
  // Synchronously sets the scan mode of the adapter, and returns true if the
  // operation was a success.
  virtual bool SetScanMode(ScanMode scan_mode) = 0;

  // https://developer.android.com/reference/android/bluetooth/BluetoothAdapter.html#getName()
  // Returns an empty string on error
  virtual std::string GetName() const = 0;
  // https://developer.android.com/reference/android/bluetooth/BluetoothAdapter.html#setName(java.lang.String)
  virtual bool SetName(absl::string_view name) = 0;

  // Returns BT MAC address assigned to this adapter.
  virtual std::string GetMacAddress() const = 0;
};

}  // namespace api
}  // namespace nearby
}  // namespace location

#endif  // PLATFORM_V2_API_BLUETOOTH_ADAPTER_H_
