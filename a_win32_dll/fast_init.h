#pragma once

#include <memory>
#include <vector>
#include <map>
#include <string>
#include <optional>
#include <functional>

#include <windows.devices.bluetooth.advertisement.h>
#include <wrl/client.h>

namespace device
{
    typedef DWORD SystemErrorCode;

    class BluetoothAdvertisement
    {
    public:
        // Possible types of error raised while registering or unregistering
        // advertisements.
        enum ErrorCode {
            ERROR_UNSUPPORTED_PLATFORM,  // Bluetooth advertisement not supported on
            // current platform.
            ERROR_ADVERTISEMENT_ALREADY_EXISTS,  // An advertisement is already
            // registered.
            ERROR_ADVERTISEMENT_DOES_NOT_EXIST,  // Unregistering an advertisement which
            // is not registered.
            ERROR_ADVERTISEMENT_INVALID_LENGTH,  // Advertisement is not of a valid
            // length.
            ERROR_STARTING_ADVERTISEMENT,  // Error when starting the advertisement
            // through a platform API.
            ERROR_RESET_ADVERTISING,       // Error while resetting advertising.
            ERROR_ADAPTER_POWERED_OFF,     // Error because the adapter is off
            INVALID_ADVERTISEMENT_ERROR_CODE
        };

        // Type of advertisement.
        enum AdvertisementType {
            // This advertises with the type set to ADV_NONCONN_IND, which indicates
            // to receivers that our device is not connectable.
            ADVERTISEMENT_TYPE_BROADCAST,
            // This advertises with the type set to ADV_IND or ADV_SCAN_IND, which
            // indicates to receivers that our device is connectable.
            ADVERTISEMENT_TYPE_PERIPHERAL
        };

        using UUIDList = std::vector<std::string>;
        using ManufacturerData = std::map<uint16_t, std::vector<uint8_t>>;
        using ServiceData = std::map<std::string, std::vector<uint8_t>>;
        using ScanResponseData = std::map<uint8_t, std::vector<uint8_t>>;

        // Structure that holds the data for an advertisement.
        class Data {
        public:
            explicit Data(AdvertisementType type);

            Data(const Data&) = delete;
            Data& operator=(const Data&) = delete;

            ~Data();

            AdvertisementType type() { return type_; }

            std::optional<UUIDList> service_uuids() {
                return pass_value(service_uuids_);
            }
            std::optional<ManufacturerData> manufacturer_data() {
                return pass_value(manufacturer_data_);
            }
            std::optional<UUIDList> solicit_uuids() {
                return pass_value(solicit_uuids_);
            }
            std::optional<ServiceData> service_data() {
                return pass_value(service_data_);
            }
            std::optional<ScanResponseData> scan_response_data() {
                return pass_value(scan_response_data_);
            }

            void set_service_uuids(std::optional<UUIDList> service_uuids) {
                service_uuids_ = std::move(service_uuids);
            }
            void set_manufacturer_data(
                std::optional<ManufacturerData> manufacturer_data) {
                manufacturer_data_ = std::move(manufacturer_data);
            }
            void set_solicit_uuids(std::optional<UUIDList> solicit_uuids) {
                solicit_uuids_ = std::move(solicit_uuids);
            }
            void set_service_data(std::optional<ServiceData> service_data) {
                service_data_ = std::move(service_data);
            }
            void set_scan_response_data(
                std::optional<ScanResponseData> scan_response_data) {
                scan_response_data_ = std::move(scan_response_data);
            }

            void set_include_tx_power(bool include_tx_power) {
                include_tx_power_ = include_tx_power;
            }

        private:
            Data();

            // Passes the value along held by |from|, and restore the optional moved
            // from to nullopt.
            template <typename T>
            static std::optional<T> pass_value(std::optional<T>& from) {
                std::optional<T> value = std::move(from);
                from = std::nullopt;
                return value;
            }

            AdvertisementType type_;
            std::optional<UUIDList> service_uuids_;
            std::optional<ManufacturerData> manufacturer_data_;
            std::optional<UUIDList> solicit_uuids_;
            std::optional<ServiceData> service_data_;
            std::optional<ScanResponseData> scan_response_data_;
            bool include_tx_power_;
        };

        // Interface for observing changes to this advertisement.
        class Observer {
        public:
            virtual ~Observer() {}

            // Called when this advertisement is released and is no longer advertising.
            virtual void AdvertisementReleased(
                BluetoothAdvertisement* advertisement) = 0;
        };

        BluetoothAdvertisement(const BluetoothAdvertisement&) = delete;
        BluetoothAdvertisement& operator=(const BluetoothAdvertisement&) = delete;

		using SuccessCallback = std::function<void()>;
		using ErrorCallback = std::function<void(ErrorCode)>;

		protected:

		BluetoothAdvertisement();

		// The destructor will unregister this advertisement.
		virtual ~BluetoothAdvertisement();

	};

	class BluetoothAdvertisementWinrt : public BluetoothAdvertisement
	{
	public:
		BluetoothAdvertisementWinrt();

		BluetoothAdvertisementWinrt(const BluetoothAdvertisementWinrt&) = delete;
		BluetoothAdvertisementWinrt& operator=(const BluetoothAdvertisementWinrt&) =
			delete;

		bool Initialize(
			std::unique_ptr<BluetoothAdvertisement::Data> advertisement_data);
		void Register(SuccessCallback callback, ErrorCallback error_callback);

        virtual HRESULT ActivateBluetoothLEAdvertisementInstance(
            ABI::Windows::Devices::Bluetooth::Advertisement::
            IBluetoothLEAdvertisement** instance) const;

        virtual HRESULT GetBluetoothLEAdvertisementPublisherActivationFactory(
            ABI::Windows::Devices::Bluetooth::Advertisement::
            IBluetoothLEAdvertisementPublisherFactory** factory) const;

        virtual HRESULT GetBluetoothLEManufacturerDataFactory(
            ABI::Windows::Devices::Bluetooth::Advertisement::
            IBluetoothLEManufacturerDataFactory** factory) const;

        struct PendingCallbacks {
            PendingCallbacks(SuccessCallback callback, ErrorCallback error_callback);
            ~PendingCallbacks();

            SuccessCallback callback;
            ErrorCallback error_callback;
        };

        std::unique_ptr<PendingCallbacks> pending_register_callbacks_;
        std::unique_ptr<PendingCallbacks> pending_unregister_callbacks_;

		void OnStatusChanged(
			ABI::Windows::Devices::Bluetooth::Advertisement::
			IBluetoothLEAdvertisementPublisher* publisher,
			ABI::Windows::Devices::Bluetooth::Advertisement::
			IBluetoothLEAdvertisementPublisherStatusChangedEventArgs* changed);

		Microsoft::WRL::ComPtr<ABI::Windows::Devices::Bluetooth::Advertisement::
			IBluetoothLEAdvertisementPublisher>
			publisher_;
	};

    class BluetoothAdapter
    {
    public:
        using OnceClosure = std::function<void()>;
        using ErrorCallback = OnceClosure;

        using CreateAdvertisementCallback =
            std::function<void(BluetoothAdvertisement*)>;
        using AdvertisementErrorCallback = BluetoothAdvertisement::ErrorCallback;

        static BluetoothAdapter* CreateAdapter();

        virtual void Initialize(OnceClosure callback) = 0;

        // The address of this adapter. The address format is "XX:XX:XX:XX:XX:XX",
  // where each XX is a hexadecimal number.
        virtual std::string GetAddress() const = 0;

        // The name of the adapter.
        virtual std::string GetName() const = 0;

        // The Bluetooth system name. Implementations may return an informational name
        // "BlueZ 5.54" on Chrome OS.
        //virtual std::string GetSystemName() const;

        // Set the human-readable name of the adapter to |name|. On success,
        // |callback| will be called. On failure, |error_callback| will be called.
        // TODO(crbug.com/1117654): Implement a mechanism to request this resource
        // before being able to use it.
        virtual void SetName(const std::string& name,
            OnceClosure callback,
            ErrorCallback error_callback) = 0;

        // Indicates whether the adapter is initialized and ready to use.
        virtual bool IsInitialized() const = 0;

        // Indicates whether the adapter is actually present on the system. For the
        // default adapter, this indicates whether any adapter is present. An adapter
        // is only considered present if the address has been obtained.
        virtual bool IsPresent() const = 0;

        // Indicates whether the adapter radio can be powered. Defaults to
        // IsPresent(). Currently only overridden on Windows, where the adapter can be
        // present, but we might fail to get access to the underlying radio.
        //virtual bool CanPower() const;

        // Indicates whether the adapter radio is powered.
        virtual bool IsPowered() const = 0;

        // Returns the status of the browser's Bluetooth permission status.
        //virtual PermissionStatus GetOsPermissionStatus() const;

        // Requests a change to the adapter radio power. Setting |powered| to true
        // will turn on the radio and false will turn it off. On success, |callback|
        // will be called. On failure, |error_callback| will be called.
        //
        // The default implementation is meant for platforms that don't have a
        // callback based API. It will store pending callbacks in
        // |set_powered_callbacks_| and invoke SetPoweredImpl(bool) which these
        // platforms need to implement. Pending callbacks are only run when
        // RunPendingPowerCallbacks() is invoked.
        //
        // Platforms that natively support a callback based API (e.g. BlueZ and Win)
        // should override this method and provide their own implementation instead.
        //
        // Due to an issue with non-native APIs on Windows 10, both IsPowered() and
        // SetPowered() don't work correctly when run from a x86 Chrome on a x64 CPU.
        // See https://github.com/Microsoft/cppwinrt/issues/47 for more details.
        /*virtual void SetPowered(bool powered,
            OnceClosure callback,
            ErrorCallback error_callback);*/

        // Indicates whether the adapter support the LowEnergy peripheral role.
        //virtual bool IsPeripheralRoleSupported() const;

        // Indicates whether the adapter radio is discoverable.
        virtual bool IsDiscoverable() const = 0;

        // Creates and registers an advertisement for broadcast over the LE channel.
      // The created advertisement will be returned via the success callback. An
      // advertisement can unregister itself at any time by calling its unregister
      // function.
        virtual void RegisterAdvertisement(
            std::unique_ptr<BluetoothAdvertisement::Data> advertisement_data,
            CreateAdvertisementCallback callback,
            AdvertisementErrorCallback error_callback) = 0;
    };

    class BluetoothAdapterFactory {
    public:
        using AdapterCallback =
            std::function<void(BluetoothAdapter* adapter)>;

        BluetoothAdapterFactory();
        ~BluetoothAdapterFactory();

        static BluetoothAdapterFactory* Get();

        void AdapterInitialized();

        // Returns the shared instance of the default adapter, creating and
        // initializing it if necessary. |callback| is called with the adapter
        // instance passed only once the adapter is fully initialized and ready to
        // use.
        void GetAdapter(AdapterCallback callback);

        BluetoothAdapter* adapter_;
        std::vector<AdapterCallback> adapter_callbacks_;
    };

    class ScopedClosureRunner {
    public:
        using OnceClosure = std::function<void()>;

        ScopedClosureRunner();
        explicit ScopedClosureRunner(OnceClosure closure);
        ScopedClosureRunner(ScopedClosureRunner&& other);
        // Runs the current closure if it's set, then replaces it with the closure
        // from |other|. This is akin to how unique_ptr frees the contained pointer in
        // its move assignment operator. If you need to explicitly avoid running any
        // current closure, use ReplaceClosure().
        ScopedClosureRunner& operator=(ScopedClosureRunner&& other);
        ~ScopedClosureRunner();

        explicit operator bool() const { return !!closure_; }

        // Calls the current closure and resets it, so it wont be called again.
        void RunAndReset();

        // Replaces closure with the new one releasing the old one without calling it.
        void ReplaceClosure(OnceClosure closure);

        // Releases the Closure without calling.
        OnceClosure Release();

    private:
        OnceClosure closure_;
    };

    class BluetoothAdapterWinrt : public BluetoothAdapter
    {
    public:
        std::string GetAddress() const override;
        std::string GetName() const override;
        void SetName(const std::string& name,
            OnceClosure callback,
            ErrorCallback error_callback) override;
        bool IsInitialized() const override;
        bool IsPresent() const override;
        bool CanPower() const;
        bool IsPowered() const override;
        //bool IsPeripheralRoleSupported() const override;
        bool IsDiscoverable() const override;

        void Initialize(OnceClosure init_callback) override;

        void RegisterAdvertisement(
            std::unique_ptr<BluetoothAdvertisement::Data> advertisement_data,
            CreateAdvertisementCallback callback,
            AdvertisementErrorCallback error_callback) override;

        virtual std::shared_ptr<BluetoothAdvertisementWinrt> CreateAdvertisement()
            const;

        void OnRegisterAdvertisement(BluetoothAdvertisement* advertisement,
            CreateAdvertisementCallback callback);

        void OnRegisterAdvertisementError(
            BluetoothAdvertisement* advertisement,
            AdvertisementErrorCallback error_callback,
            BluetoothAdvertisement::ErrorCode error_code);

        struct StaticsInterfaces {
            StaticsInterfaces(
                Microsoft::WRL::ComPtr<IAgileReference>,   // IBluetoothStatics
                Microsoft::WRL::ComPtr<IAgileReference>,   // IDeviceInformationStatics
                Microsoft::WRL::ComPtr<IAgileReference>);  // IRadioStatics
            StaticsInterfaces();
            StaticsInterfaces(const StaticsInterfaces&);
            ~StaticsInterfaces();

            Microsoft::WRL::ComPtr<IAgileReference> adapter_statics;
            Microsoft::WRL::ComPtr<IAgileReference> device_information_statics;
            Microsoft::WRL::ComPtr<IAgileReference> radio_statics;
        };

        static StaticsInterfaces PerformSlowInitTasks();

        static StaticsInterfaces GetAgileReferencesForStatics(
            Microsoft::WRL::ComPtr<
            ABI::Windows::Devices::Bluetooth::IBluetoothAdapterStatics>
            adapter_statics,
            Microsoft::WRL::ComPtr<
            ABI::Windows::Devices::Enumeration::IDeviceInformationStatics>
            device_information_statics,
            Microsoft::WRL::ComPtr<ABI::Windows::Devices::Radios::IRadioStatics>
            radio_statics);

        void CompleteInitAgile(OnceClosure init_callback,
            StaticsInterfaces statics);
        void CompleteInit(
            OnceClosure init_callback,
            Microsoft::WRL::ComPtr<
            ABI::Windows::Devices::Bluetooth::IBluetoothAdapterStatics>
            bluetooth_adapter_statics,
            Microsoft::WRL::ComPtr<
            ABI::Windows::Devices::Enumeration::IDeviceInformationStatics>
            device_information_statics,
            Microsoft::WRL::ComPtr<ABI::Windows::Devices::Radios::IRadioStatics>
            radio_statics);

        void OnGetDefaultAdapter(
            ScopedClosureRunner on_init,
            Microsoft::WRL::ComPtr<
            ABI::Windows::Devices::Bluetooth::IBluetoothAdapter> adapter);

        void OnCreateFromIdAsync(
            ScopedClosureRunner on_init,
            Microsoft::WRL::ComPtr<
            ABI::Windows::Devices::Enumeration::IDeviceInformation>
            device_information);

        void OnRequestRadioAccess(
            ScopedClosureRunner on_init,
            ABI::Windows::Devices::Radios::RadioAccessStatus access_status);

        void OnGetRadio(
            ScopedClosureRunner on_init,
            Microsoft::WRL::ComPtr<ABI::Windows::Devices::Radios::IRadio> radio);

        void OnRadioStateChanged(ABI::Windows::Devices::Radios::IRadio* radio,
            IInspectable* object);

        void OnPoweredRadioAdded(
            ABI::Windows::Devices::Enumeration::IDeviceWatcher* watcher,
            ABI::Windows::Devices::Enumeration::IDeviceInformation* info);

        void OnPoweredRadioRemoved(
            ABI::Windows::Devices::Enumeration::IDeviceWatcher* watcher,
            ABI::Windows::Devices::Enumeration::IDeviceInformationUpdate* update);

        void OnPoweredRadiosEnumerated(
            ABI::Windows::Devices::Enumeration::IDeviceWatcher* watcher,
            IInspectable* object);

        std::string address_;
        std::string name_;
        bool is_initialized_ = false;
        bool radio_access_allowed_ = false;
        bool radio_was_powered_ = false;
        size_t num_powered_radios_ = 0;
        Microsoft::WRL::ComPtr<ABI::Windows::Devices::Bluetooth::IBluetoothAdapter>
            adapter_;
        Microsoft::WRL::ComPtr<ABI::Windows::Devices::Radios::IRadio> radio_;

        Microsoft::WRL::ComPtr<
            ABI::Windows::Devices::Bluetooth::IBluetoothAdapterStatics>
            bluetooth_adapter_statics_;
        Microsoft::WRL::ComPtr<
            ABI::Windows::Devices::Enumeration::IDeviceInformationStatics>
            device_information_statics_;
        Microsoft::WRL::ComPtr<ABI::Windows::Devices::Radios::IRadioStatics>
            radio_statics_;

        Microsoft::WRL::ComPtr<ABI::Windows::Devices::Enumeration::IDeviceWatcher>
            powered_radio_watcher_;

        std::vector<std::shared_ptr<BluetoothAdvertisement>> pending_advertisements_;

        std::optional<EventRegistrationToken> powered_radio_added_token_;
        std::optional<EventRegistrationToken> powered_radio_removed_token_;
        std::optional<EventRegistrationToken> powered_radios_enumerated_token_;

        std::unique_ptr<ScopedClosureRunner> on_init_;
    };
}