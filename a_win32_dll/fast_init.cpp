
#include "fast_init.h"
#include "logging.h"
#include <wrl/wrappers/corewrappers.h>
#include <wrl/event.h>
#include <roapi.h>
#include <robuffer.h>

#include <windows.foundation.h>
#include <windows.foundation.numerics.h>
#include <windows.foundation.collections.h>

namespace device {

    namespace {

        using ABI::Windows::Devices::Bluetooth::BluetoothError;
        using ABI::Windows::Devices::Bluetooth::BluetoothError_NotSupported;
        using ABI::Windows::Devices::Bluetooth::BluetoothError_RadioNotAvailable;
        using ABI::Windows::Devices::Bluetooth::Advertisement::
            BluetoothLEAdvertisementPublisherStatus;
        using ABI::Windows::Devices::Bluetooth::Advertisement::
            BluetoothLEAdvertisementPublisherStatus_Aborted;
        using ABI::Windows::Devices::Bluetooth::Advertisement::
            BluetoothLEAdvertisementPublisherStatus_Started;
        using ABI::Windows::Devices::Bluetooth::Advertisement::
            BluetoothLEAdvertisementPublisherStatus_Stopped;
        using ABI::Windows::Devices::Bluetooth::Advertisement::
            BluetoothLEManufacturerData;
        using ABI::Windows::Devices::Bluetooth::Advertisement::
            IBluetoothLEAdvertisement;
        using ABI::Windows::Devices::Bluetooth::Advertisement::
            IBluetoothLEAdvertisementPublisher;
        using ABI::Windows::Devices::Bluetooth::Advertisement::
            IBluetoothLEAdvertisementPublisherFactory;
        using ABI::Windows::Devices::Bluetooth::Advertisement::
            IBluetoothLEAdvertisementPublisherStatusChangedEventArgs;
        using ABI::Windows::Devices::Bluetooth::Advertisement::
            IBluetoothLEManufacturerData;
        using ABI::Windows::Devices::Bluetooth::Advertisement::
            IBluetoothLEManufacturerDataFactory;
        using ABI::Windows::Devices::Radios::RadioAccessStatus;
        using ABI::Windows::Devices::Radios::RadioAccessStatus_Allowed;
        using ABI::Windows::Devices::Radios::RadioAccessStatus_DeniedBySystem;
        using ABI::Windows::Devices::Radios::RadioAccessStatus_DeniedByUser;
        using ABI::Windows::Devices::Radios::RadioAccessStatus_Unspecified;
        using ABI::Windows::Devices::Radios::RadioState;
        using ABI::Windows::Devices::Radios::RadioState_Off;
        using ABI::Windows::Devices::Radios::RadioState_On;
        using ABI::Windows::Devices::Radios::RadioState_Unknown;

        using ABI::Windows::Devices::Radios::IRadio;

        using ABI::Windows::Foundation::Collections::IVector;
        using ABI::Windows::Storage::Streams::IBuffer;
        using Microsoft::WRL::ComPtr;
        using Microsoft::WRL::Wrappers::HStringReference;

        void RemoveStatusChangedHandler(IBluetoothLEAdvertisementPublisher* publisher,
            EventRegistrationToken token) {
            HRESULT hr = publisher->remove_StatusChanged(token);
            if (FAILED(hr)) {
            }
        }

    }  // namespace

    using IBuffer = ABI::Windows::Storage::Streams::IBuffer;

    HRESULT GetPointerToBufferData(IBuffer* buffer, uint8_t** out, UINT32* length) {
        *out = nullptr;

        Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess>
            buffer_byte_access;
        HRESULT hr = buffer->QueryInterface(IID_PPV_ARGS(&buffer_byte_access));
        if (FAILED(hr))
            return hr;

        hr = buffer->get_Length(length);
        if (FAILED(hr))
            return hr;

        // Lifetime of the pointing buffer is controlled by the buffer object.
        return buffer_byte_access->Buffer(out);
    }

    HRESULT CreateIBufferFromData(const uint8_t* data,
        UINT32 length,
        Microsoft::WRL::ComPtr<IBuffer>* buffer) {
        *buffer = nullptr;

        Microsoft::WRL::ComPtr<ABI::Windows::Storage::Streams::IBufferFactory>
            buffer_factory;
        auto class_id = HStringReference(RuntimeClass_Windows_Storage_Streams_Buffer);
        HRESULT hr = RoGetActivationFactory(class_id .Get(), IID_PPV_ARGS(buffer_factory.GetAddressOf()));
        if (FAILED(hr))
            return hr;

        Microsoft::WRL::ComPtr<IBuffer> internal_buffer;
        hr = buffer_factory->Create(length, &internal_buffer);
        if (FAILED(hr))
            return hr;

        hr = internal_buffer->put_Length(length);
        if (FAILED(hr))
            return hr;

        uint8_t* p_buffer_data;
        hr = GetPointerToBufferData(internal_buffer.Get(), &p_buffer_data, &length);
        if (FAILED(hr))
            return hr;

        memcpy(p_buffer_data, data, length);

        *buffer = std::move(internal_buffer);

        return S_OK;
    }

    BluetoothAdvertisement::Data::Data(AdvertisementType type)
        : type_(type), include_tx_power_(false) {
    }

    BluetoothAdvertisement::Data::~Data() = default;

    BluetoothAdvertisement::Data::Data()
        : type_(ADVERTISEMENT_TYPE_BROADCAST), include_tx_power_(false) {
    }

    /*void BluetoothAdvertisement::AddObserver(
        BluetoothAdvertisement::Observer* observer) {
        CHECK(observer);
        observers_.AddObserver(observer);
    }

    void BluetoothAdvertisement::RemoveObserver(
        BluetoothAdvertisement::Observer* observer) {
        CHECK(observer);
        observers_.RemoveObserver(observer);
    }*/

    BluetoothAdvertisement::BluetoothAdvertisement() = default;
    BluetoothAdvertisement::~BluetoothAdvertisement() = default;

    BluetoothAdvertisementWinrt::BluetoothAdvertisementWinrt() {}

    HRESULT
        BluetoothAdvertisementWinrt::ActivateBluetoothLEAdvertisementInstance(
            IBluetoothLEAdvertisement** instance) const {
        auto advertisement_hstring = HStringReference(RuntimeClass_Windows_Devices_Bluetooth_Advertisement_BluetoothLEAdvertisement);
        
        ComPtr<IInspectable> inspectable;
        HRESULT hr =
            ::RoActivateInstance(advertisement_hstring.Get(), &inspectable);
        if (FAILED(hr)) {
            return hr;
        }

        ComPtr<IBluetoothLEAdvertisement> advertisement;
        hr = inspectable.As(&advertisement);
        if (FAILED(hr)) {
            return hr;
        }

        return advertisement.CopyTo(instance);
    }

    HRESULT
        BluetoothAdvertisementWinrt::GetBluetoothLEManufacturerDataFactory(
            IBluetoothLEManufacturerDataFactory** factory) const
    {
        auto class_id = HStringReference(RuntimeClass_Windows_Devices_Bluetooth_Advertisement_BluetoothLEManufacturerData);
        return ::RoGetActivationFactory(class_id.Get(), IID_PPV_ARGS(factory));
    }

    HRESULT
        BluetoothAdvertisementWinrt::
        GetBluetoothLEAdvertisementPublisherActivationFactory(
            IBluetoothLEAdvertisementPublisherFactory** factory) const
    {
        auto class_id = HStringReference(RuntimeClass_Windows_Devices_Bluetooth_Advertisement_BluetoothLEAdvertisementPublisher);
        return RoGetActivationFactory(class_id.Get(), IID_PPV_ARGS(factory));
    }

    bool BluetoothAdvertisementWinrt::Initialize(
        std::unique_ptr<BluetoothAdvertisement::Data> advertisement_data) {
        if (advertisement_data->service_uuids()) {
            BLUETOOTH_LOG(ERROR)
                << "Windows does not support advertising Service UUIDs.";
            return false;
        }

        if (advertisement_data->solicit_uuids()) {
            BLUETOOTH_LOG(ERROR)
                << "Windows does not support advertising Solicit UUIDs.";
            return false;
        }

        if (advertisement_data->service_data()) {
            BLUETOOTH_LOG(ERROR)
                << "Windows does not support advertising Service Data.";
            return false;
        }

        auto manufacturer_data = advertisement_data->manufacturer_data();
        if (!manufacturer_data) {
            BLUETOOTH_LOG(ERROR) << "No Manufacturer Data present.";
            return false;
        }

        ComPtr<IBluetoothLEAdvertisement> advertisement;
        HRESULT hr = ActivateBluetoothLEAdvertisementInstance(&advertisement);
        if (FAILED(hr)) {
            return false;
        }

        ComPtr<IVector<BluetoothLEManufacturerData*>> manufacturer_data_list;
        hr = advertisement->get_ManufacturerData(&manufacturer_data_list);
        if (FAILED(hr)) {
            return false;
        }

        ComPtr<IBluetoothLEManufacturerDataFactory> manufacturer_data_factory;
        hr = GetBluetoothLEManufacturerDataFactory(&manufacturer_data_factory);
        if (FAILED(hr)) {
            return false;
        }

        for (const auto& pair : *manufacturer_data) {
            uint16_t manufacturer = pair.first;
            const std::vector<uint8_t>& data = pair.second;

            ComPtr<IBuffer> buffer;
            hr = CreateIBufferFromData(data.data(), data.size(), &buffer);
            if (FAILED(hr)) {
                return false;
            }

            ComPtr<IBluetoothLEManufacturerData> manufacturer_data_entry;
            hr = manufacturer_data_factory->Create(manufacturer, buffer.Get(),
                &manufacturer_data_entry);
            if (FAILED(hr)) {
                return false;
            }

            hr = manufacturer_data_list->Append(manufacturer_data_entry.Get());
            if (FAILED(hr)) {
                return false;
            }
        }

        ComPtr<IBluetoothLEAdvertisementPublisherFactory> publisher_factory;
        hr = GetBluetoothLEAdvertisementPublisherActivationFactory(&publisher_factory);
        if (FAILED(hr)) {
            return false;
        }

        hr = publisher_factory->Create(advertisement.Get(), &publisher_);
        if (FAILED(hr)) {
            return false;
        }

        return true;
    }

    BluetoothAdvertisementWinrt::PendingCallbacks::PendingCallbacks(
        SuccessCallback callback,
        ErrorCallback error_callback)
        : callback(std::move(callback)),
        error_callback(std::move(error_callback)) {}

    BluetoothAdvertisementWinrt::PendingCallbacks::~PendingCallbacks() = default;

    void BluetoothAdvertisementWinrt::OnStatusChanged(
        IBluetoothLEAdvertisementPublisher* publisher,
        IBluetoothLEAdvertisementPublisherStatusChangedEventArgs* changed) {
        BluetoothLEAdvertisementPublisherStatus status;
        HRESULT hr = changed->get_Status(&status);
        if (FAILED(hr)) {
            return;
        }

        //BLUETOOTH_LOG(EVENT) << "Publisher Status: " << static_cast<int>(status);
        //if (status == BluetoothLEAdvertisementPublisherStatus_Stopped) {
        //    // Notify Observers.
        //    for (auto& observer : observers_)
        //        observer.AdvertisementReleased(this);
        //}

        // Return early if there is no pending action.
        if (!pending_register_callbacks_ && !pending_unregister_callbacks_)
            return;

        // Register and Unregister should never be pending at the same time.
        DCHECK(!pending_register_callbacks_ || !pending_unregister_callbacks_);

        const bool is_starting = pending_register_callbacks_ != nullptr;
        ErrorCode error_code =
            is_starting ? ERROR_STARTING_ADVERTISEMENT : ERROR_RESET_ADVERTISING;

        // Clears out pending callbacks by moving them into a local variable and runs
        // the appropriate error callback with |error_code|.
        auto run_error_cb = [&](ErrorCode error_code) {
            auto callbacks = std::move(is_starting ? pending_register_callbacks_
                : pending_unregister_callbacks_);
            std::move(callbacks->error_callback)(error_code);
            };

        if (status == BluetoothLEAdvertisementPublisherStatus_Aborted) {
            BluetoothError bluetooth_error;
            hr = changed->get_Error(&bluetooth_error);
            if (FAILED(hr)) {
                run_error_cb(error_code);
                return;
            }

            BLUETOOTH_LOG(EVENT) << "Publisher aborted: "
                << static_cast<int>(bluetooth_error);
            switch (bluetooth_error) {
            case BluetoothError_RadioNotAvailable:
                error_code = ERROR_ADAPTER_POWERED_OFF;
                break;
            case BluetoothError_NotSupported:
                error_code = ERROR_UNSUPPORTED_PLATFORM;
                break;
            default:
                break;
            }

            run_error_cb(error_code);
            return;
        }

        if (is_starting &&
            status == BluetoothLEAdvertisementPublisherStatus_Started) {
            BLUETOOTH_LOG(EVENT) << "Starting the Publisher was successful.";
            auto callbacks = std::move(pending_register_callbacks_);
            std::move(callbacks->callback)();
            return;
        }

        if (!is_starting &&
            status == BluetoothLEAdvertisementPublisherStatus_Stopped) {
            BLUETOOTH_LOG(EVENT) << "Stopping the Publisher was successful.";
            auto callbacks = std::move(pending_unregister_callbacks_);
            std::move(callbacks->callback)();
            return;
        }

        // The other states are temporary and we expect a future StatusChanged
        // event.
    }

    void BluetoothAdvertisementWinrt::Register(SuccessCallback callback,
        ErrorCallback error_callback) {
        // Register should only be called once during initialization.

        // Register should only be called after successful initialization.
        DCHECK(publisher_);

        auto handler = Microsoft::WRL::Callback<
            ABI::Windows::Foundation::ITypedEventHandler<
            ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementPublisher*, 
            ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementPublisherStatusChangedEventArgs*>>
            ([this](IBluetoothLEAdvertisementPublisher* sender, IBluetoothLEAdvertisementPublisherStatusChangedEventArgs* args) {
            OnStatusChanged(sender, args);
            return S_OK;
                });

        EventRegistrationToken status_changed_token_;
        (*publisher_.Get()).add_StatusChanged(handler.Get(), &status_changed_token_);

        /*if (!status_changed_token_) {
            base::SingleThreadTaskRunner::GetCurrentDefault()->PostTask(
                FROM_HERE, base::BindOnce(std::move(error_callback),
                    ERROR_STARTING_ADVERTISEMENT));
            return;
        }*/

        HRESULT hr = publisher_->Start();
        if (FAILED(hr)) {
            /*base::SingleThreadTaskRunner::GetCurrentDefault()->PostTask(
                FROM_HERE, base::BindOnce(std::move(error_callback),
                    ERROR_STARTING_ADVERTISEMENT));*/
            RemoveStatusChangedHandler(publisher_.Get(), status_changed_token_);
            //status_changed_token_.reset();
            return;
        }

        pending_register_callbacks_ = std::make_unique<PendingCallbacks>(
            std::move(callback), std::move(error_callback));
    }

    constexpr wchar_t kPoweredRadiosAqsFilter[] =
        L"System.Devices.InterfaceClassGuid:=\"{0850302A-B344-4fda-9BE9-"
        L"90576B8D46F0}\" AND "
        L"System.Devices.InterfaceEnabled:=System.StructuredQueryType.Boolean#True";

    // Utility functions to pretty print enum values.
    constexpr const char* ToCString(RadioAccessStatus access_status) {
        switch (access_status) {
        case RadioAccessStatus_Unspecified:
            return "RadioAccessStatus::Unspecified";
        case RadioAccessStatus_Allowed:
            return "RadioAccessStatus::Allowed";
        case RadioAccessStatus_DeniedByUser:
            return "RadioAccessStatus::DeniedByUser";
        case RadioAccessStatus_DeniedBySystem:
            return "RadioAccessStatus::DeniedBySystem";
        }

        return "";
    }

    RadioState GetState(IRadio* radio)
    {
        RadioState state;
        HRESULT hr = radio->get_State(&state);
        if (FAILED(hr)) {
            return RadioState_Unknown;
        }
        return state;
    }

    std::string BluetoothAdapterWinrt::GetAddress() const {
        return address_;
    }

    std::string BluetoothAdapterWinrt::GetName() const {
        return name_;
    }

    void BluetoothAdapterWinrt::SetName(const std::string& name,
        OnceClosure callback,
        ErrorCallback error_callback) {
        //NOTIMPLEMENTED();
    }

    bool BluetoothAdapterWinrt::IsInitialized() const {
        return is_initialized_;
    }

    bool BluetoothAdapterWinrt::IsPresent() const {
        // Obtaining the default adapter will fail if no physical adapter is present.
        // Thus a non-zero |adapter| implies that a physical adapter is present.
        return adapter_ != nullptr;
    }

    bool BluetoothAdapterWinrt::CanPower() const {
        return radio_ != nullptr && radio_access_allowed_;
    }

    bool BluetoothAdapterWinrt::IsPowered() const {
        // Due to an issue on WoW64 we might fail to obtain the radio in OnGetRadio().
        // This is why it can be null here.
        if (!radio_)
            return num_powered_radios_ != 0;

        return GetState(radio_.Get()) == RadioState_On;
    }

    bool BluetoothAdapterWinrt::IsPeripheralRoleSupported() const {
        if (!adapter_) {
            return false;
        }
        boolean supported = false;
        HRESULT hr = adapter_->get_IsPeripheralRoleSupported(&supported);
        if (FAILED(hr)) {
        }
        return supported;
    }

    bool BluetoothAdapterWinrt::IsDiscoverable() const {
        //NOTIMPLEMENTED();
        return false;
    }

    std::shared_ptr<BluetoothAdvertisementWinrt>
        BluetoothAdapterWinrt::CreateAdvertisement() const {
        return std::make_shared<BluetoothAdvertisementWinrt>();
    }

    void BluetoothAdapterWinrt::RegisterAdvertisement(
        std::unique_ptr<BluetoothAdvertisement::Data> advertisement_data,
        CreateAdvertisementCallback callback,
        AdvertisementErrorCallback error_callback)
    {
        auto advertisement = CreateAdvertisement();

        if (!advertisement->Initialize(std::move(advertisement_data)))
        {
            /*BLUETOOTH_LOG(ERROR) << "Failed to Initialize Advertisement.";
            ui_task_runner_->PostTask(
                FROM_HERE,
                base::BindOnce(std::move(error_callback),
                    BluetoothAdvertisement::ERROR_STARTING_ADVERTISEMENT));*/
            return;
        }

        // In order to avoid |advertisement| holding a strong reference to itself, we
        // pass only a weak reference to the callbacks, and store a strong reference
        // in |pending_advertisements_|. When the callbacks are run, they will remove
        // the corresponding advertisement from the list of pending advertisements.
        advertisement->Register([this, advertisement, callback]() {
            OnRegisterAdvertisement(advertisement.get(), std::move(callback));
            },
            [this, advertisement, error_callback](BluetoothAdvertisement::ErrorCode error_code) {
                OnRegisterAdvertisementError(advertisement.get(), std::move(error_callback), error_code);
            });
            

        pending_advertisements_.push_back(std::move(advertisement));
    }

    void BluetoothAdapterWinrt::OnRegisterAdvertisement(
        BluetoothAdvertisement* advertisement,
        CreateAdvertisementCallback callback)
    {
        //DCHECK(base::Contains(pending_advertisements_, advertisement));
        //auto wrapped_advertisement = base::WrapRefCounted(advertisement);
        //base::Erase(pending_advertisements_, advertisement);

        std::move(callback)(std::move(advertisement));
    }

    void BluetoothAdapterWinrt::OnRegisterAdvertisementError(
        BluetoothAdvertisement* advertisement,
        AdvertisementErrorCallback error_callback,
        BluetoothAdvertisement::ErrorCode error_code)
    {
        // Note: We are not DCHECKing that |pending_advertisements_| contains
        // |advertisement|, as this method might be invoked during destruction.
        //base::Erase(pending_advertisements_, advertisement);
        std::move(error_callback)(error_code);
    }
}