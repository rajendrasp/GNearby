
#include "fast_init.h"
#include "logging.h"
#include <wrl/wrappers/corewrappers.h>
#include <wrl/event.h>
#include <roapi.h>
#include <robuffer.h>
#include <winternl.h>
#include <locale>
#include <codecvt>
#include <thread>

#include <inspectable.h>
#include <windows.foundation.h>
#include <windows.foundation.numerics.h>
#include <windows.foundation.collections.h>

namespace device {

    namespace {
        
        // In order to avoid a name clash with device::BluetoothAdapter we need this
        // auxiliary namespace.
        namespace uwp {
            using ABI::Windows::Devices::Bluetooth::BluetoothAdapter;
        }  // namespace uwp

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
        using ABI::Windows::Devices::Bluetooth::IBluetoothAdapterStatics;
        using ABI::Windows::Devices::Enumeration::IDeviceInformationStatics;
        using ABI::Windows::Devices::Radios::IRadioStatics;
        using ABI::Windows::Devices::Bluetooth::IID_IBluetoothAdapterStatics;
        using ABI::Windows::Devices::Enumeration::IID_IDeviceInformationStatics;
        using ABI::Windows::Devices::Radios::IID_IRadioStatics;
        using ABI::Windows::Foundation::IAsyncOperation;
        using ABI::Windows::Devices::Bluetooth::IBluetoothAdapter;
        using ABI::Windows::Devices::Enumeration::DeviceInformation;
        using ABI::Windows::Devices::Enumeration::IDeviceInformation;
        using ABI::Windows::Devices::Radios::Radio;
        using ABI::Windows::Devices::Enumeration::IDeviceWatcher;
        using ABI::Windows::Devices::Enumeration::IDeviceInformationUpdate;
        //using ABI::Windows::Foundation::IInspectable;

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

    BluetoothAdapterFactory::BluetoothAdapterFactory() = default;

    BluetoothAdapterFactory::~BluetoothAdapterFactory() = default;

    // static
    BluetoothAdapterFactory* BluetoothAdapterFactory::Get() {
        static BluetoothAdapterFactory factory;
        return &factory;
    }

    BluetoothAdapter* BluetoothAdapter::CreateAdapter() {
        return new BluetoothAdapterWinrt();
    }

    void BluetoothAdapterFactory::GetAdapter(AdapterCallback callback)
    {
        //DCHECK(IsBluetoothSupported());

        if (!adapter_) {
            adapter_callbacks_.push_back(std::move(callback));

            adapter_ = BluetoothAdapter::CreateAdapter();
            //adapter_ = adapter_under_initialization_->GetWeakPtr();
            adapter_->Initialize([this]() {
                AdapterInitialized();
                });
            return;
        }

        if (!adapter_->IsInitialized()) {
            adapter_callbacks_.push_back(std::move(callback));
            return;
        }

        std::move(callback)(adapter_);
    }

    void BluetoothAdapterFactory::AdapterInitialized()
    {
        // Move |adapter_under_initialization_| and |adapter_callbacks_| to avoid
        // potential re-entrancy issues while looping over the callbacks.

        BluetoothAdapter* adapter =
            std::move(adapter_);
        std::vector<AdapterCallback> callbacks = std::move(adapter_callbacks_);
        for (auto& callback : callbacks)
            std::move(callback)(adapter);
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

    /*bool BluetoothAdapterWinrt::IsPeripheralRoleSupported() const {
        if (!adapter_) {
            return false;
        }
        boolean supported = false;
        HRESULT hr = adapter_->get_IsPeripheralRoleSupported(&supported);
        if (FAILED(hr)) {
        }
        return supported;
    }*/

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

    void BluetoothAdapterWinrt::Initialize(OnceClosure init_callback)
    {
        // DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);

        // Some of the initialization work requires loading libraries and should not
        // be run on the browser main thread.


        /*base::ThreadPool::PostTaskAndReplyWithResult(
            FROM_HERE,
            { base::MayBlock(), base::TaskPriority::BEST_EFFORT,
             base::ThreadPolicy::MUST_USE_FOREGROUND },
            base::BindOnce(&BluetoothAdapterWinrt::PerformSlowInitTasks),
            base::BindOnce(&BluetoothAdapterWinrt::CompleteInitAgile,
                weak_ptr_factory_.GetWeakPtr(), std::move(init_callback)));*/

        auto work = [this, init_callback]() {
            auto agile_statics = PerformSlowInitTasks();
            CompleteInitAgile(init_callback, agile_statics);
            };

        std::thread([work]() {
            work();
            }).detach();
    }

    void BluetoothAdapterWinrt::CompleteInitAgile(OnceClosure init_callback,
        StaticsInterfaces agile_statics)
    {
        if (!agile_statics.adapter_statics ||
            !agile_statics.device_information_statics ||
            !agile_statics.radio_statics) {
            CompleteInit(std::move(init_callback), nullptr, nullptr, nullptr);
            return;
        }
        ComPtr<IBluetoothAdapterStatics> bluetooth_adapter_statics;
        HRESULT hr = agile_statics.adapter_statics->Resolve(
            IID_IBluetoothAdapterStatics, &bluetooth_adapter_statics);
        DCHECK(SUCCEEDED(hr));
        ComPtr<IDeviceInformationStatics> device_information_statics;
        hr = agile_statics.device_information_statics->Resolve(
            IID_IDeviceInformationStatics, &device_information_statics);
        DCHECK(SUCCEEDED(hr));
        ComPtr<IRadioStatics> radio_statics;
        hr = agile_statics.radio_statics->Resolve(IID_IRadioStatics, &radio_statics);
        DCHECK(SUCCEEDED(hr));

        CompleteInit(std::move(init_callback), std::move(bluetooth_adapter_statics),
            std::move(device_information_statics), std::move(radio_statics));
    }

    ScopedClosureRunner::ScopedClosureRunner() = default;

    ScopedClosureRunner::ScopedClosureRunner(OnceClosure closure)
        : closure_(std::move(closure)) {}

    ScopedClosureRunner::ScopedClosureRunner(ScopedClosureRunner&& other)
        : closure_(other.Release()) {}

    ScopedClosureRunner& ScopedClosureRunner::operator=(
        ScopedClosureRunner&& other) {
        if (this != &other) {
            RunAndReset();
            ReplaceClosure(other.Release());
        }
        return *this;
    }

    ScopedClosureRunner::~ScopedClosureRunner() {
        RunAndReset();
    }

    void ScopedClosureRunner::RunAndReset() {
        if (closure_)
            std::move(closure_)();
    }

    void ScopedClosureRunner::ReplaceClosure(OnceClosure closure) {
        closure_ = std::move(closure);
    }

    ScopedClosureRunner::OnceClosure ScopedClosureRunner::Release() {
        return std::move(closure_);
    }

    void BluetoothAdapterWinrt::CompleteInit(
        OnceClosure init_callback,
        ComPtr<IBluetoothAdapterStatics> bluetooth_adapter_statics,
        ComPtr<IDeviceInformationStatics> device_information_statics,
        ComPtr<IRadioStatics> radio_statics)
    {
        // We are wrapping |init_callback| in a ScopedClosureRunner to ensure it gets
        // run no matter how the function exits. Furthermore, we set |is_initialized_|
        // to true if adapter is still active when the callback gets run.
        ScopedClosureRunner on_init(
            [this, init_callback]() {
                    if (this)
                        this->is_initialized_ = true;
                    std::move(init_callback)();
            });

        bluetooth_adapter_statics_ = bluetooth_adapter_statics;
        device_information_statics_ = device_information_statics;
        radio_statics_ = radio_statics;

        if (!bluetooth_adapter_statics_ || !device_information_statics_ ||
            !radio_statics_) {
            return;
        }

        ComPtr<IAsyncOperation<uwp::BluetoothAdapter*>> get_default_adapter_op;
        HRESULT hr =
            bluetooth_adapter_statics_->GetDefaultAsync(&get_default_adapter_op);
        if (FAILED(hr)) {
            return;
        }

        ComPtr<IBluetoothAdapter> adapter;
        get_default_adapter_op->GetResults(adapter.GetAddressOf());

        OnGetDefaultAdapter(std::move(on_init), adapter);

        /*hr = base::win::PostAsyncResults(
            std::move(get_default_adapter_op),
            base::BindOnce(&BluetoothAdapterWinrt::OnGetDefaultAdapter,
                weak_ptr_factory_.GetWeakPtr(), std::move(on_init)));*/
    }

    // Utility to convert a character to a digit in a given base
    template <int BASE, typename CHAR>
    std::optional<uint8_t> CharToDigit(CHAR c) {
        static_assert(1 <= BASE && BASE <= 36, "BASE needs to be in [1, 36]");
        if (c >= '0' && c < '0' + std::min(BASE, 10))
            return static_cast<uint8_t>(c - '0');

        if (c >= 'a' && c < 'a' + BASE - 10)
            return static_cast<uint8_t>(c - 'a' + 10);

        if (c >= 'A' && c < 'A' + BASE - 10)
            return static_cast<uint8_t>(c - 'A' + 10);

        return std::nullopt;
    }

    using StringPiece = std::string_view;

    template <typename Char, typename OutIter>
    static bool HexStringToByteContainer(StringPiece input, OutIter output) {
        size_t count = input.size();
        if (count == 0 || (count % 2) != 0)
            return false;
        for (uintptr_t i = 0; i < count / 2; ++i) {
            // most significant 4 bits
            std::optional<uint8_t> msb = CharToDigit<16>(input[i * 2]);
            // least significant 4 bits
            std::optional<uint8_t> lsb = CharToDigit<16>(input[i * 2 + 1]);
            if (!msb || !lsb) {
                return false;
            }
            *(output++) = static_cast<Char>((*msb << 4) | *lsb);
        }
        return true;
    }

    bool HexStringToSpan(StringPiece input, std::vector<uint8_t> output)
    {
        if (input.size() / 2 != output.size())
            return false;

        return HexStringToByteContainer<uint8_t>(input, output.begin());
    }

    bool ParseBluetoothAddress(std::string_view input, std::vector<uint8_t> output)
    {
        if (output.size() != 6)
            return false;

        // Try parsing addresses that lack separators, like "1A2B3C4D5E6F".
        if (input.size() == 12)
            return HexStringToSpan(input, output);

        // Try parsing MAC address with separators like: "00:11:22:33:44:55" or
        // "00-11-22-33-44-55". Separator can be either '-' or ':', but must use the
        // same style throughout.
        if (input.size() == 17)
        {
            const char separator = input[2];
            if (separator != '-' && separator != ':')
                return false;

            std::vector<uint8_t> tmp = std::vector<uint8_t>(output.begin(), output.begin() + 1);

            return (input[2] == separator) && (input[5] == separator) &&
                (input[8] == separator) && (input[11] == separator) &&
                (input[14] == separator) &&
                HexStringToSpan(input.substr(0, 2), std::vector<uint8_t>(output.begin(), output.begin() + 1)) &&
                HexStringToSpan(input.substr(3, 2), std::vector<uint8_t>(output.begin() + 1, output.begin() + 2)) &&
                HexStringToSpan(input.substr(6, 2), std::vector<uint8_t>(output.begin() + 2, output.begin() + 3)) &&
                HexStringToSpan(input.substr(9, 2), std::vector<uint8_t>(output.begin() + 3, output.begin() + 4)) &&
                HexStringToSpan(input.substr(12, 2), std::vector<uint8_t>(output.begin() + 4, output.begin() + 5)) &&
                HexStringToSpan(input.substr(15, 2), std::vector<uint8_t>(output.begin() + 5, output.begin() + 6));
        }

        return false;
    }

    void StringAppendV(std::string* dst, const char* format, va_list ap) {
        // First try with a small fixed size buffer.
        // This buffer size should be kept in sync with StringUtilTest.GrowBoundary
        // and StringUtilTest.StringPrintfBounds.
        char stack_buf[1024];

        va_list ap_copy;
        va_copy(ap_copy, ap);

        int result = vsnprintf(stack_buf, std::size(stack_buf), format, ap_copy);
        va_end(ap_copy);

        if (result >= 0 && static_cast<size_t>(result) < std::size(stack_buf)) {
            // It fit.
            dst->append(stack_buf, static_cast<size_t>(result));
            return;
        }

        // Repeatedly increase buffer size until it fits.
        size_t mem_length = std::size(stack_buf);
        while (true) {
            if (result < 0) {
                // On Windows, vsnprintf always returns the number of characters in a
                // fully-formatted string, so if we reach this point, something else is
                // wrong and no amount of buffer-doubling is going to fix it.
                return;
            }
            else {
                // We need exactly "result + 1" characters.
                mem_length = static_cast<size_t>(result) + 1;
            }

            if (mem_length > 32 * 1024 * 1024) {
                // That should be plenty, don't try anything larger.  This protects
                // against huge allocations when using vsnprintf implementations that
                // return -1 for reasons other than overflow without setting errno.
                DLOG(WARNING) << "Unable to printf the requested string due to size.";
                return;
            }

            std::vector<char> mem_buf(mem_length);

            // NOTE: You can only use a va_list once.  Since we're in a while loop, we
            // need to make a new copy each time so we don't use up the original.
            va_copy(ap_copy, ap);
            result = vsnprintf(&mem_buf[0], mem_length, format, ap_copy);
            va_end(ap_copy);

            if ((result >= 0) && (static_cast<size_t>(result) < mem_length)) {
                // It fit.
                dst->append(&mem_buf[0], static_cast<size_t>(result));
                return;
            }
        }
    }

    std::string StringPrintf(const char* format, ...) {
        va_list ap;
        va_start(ap, format);
        std::string result;
        StringAppendV(&result, format, ap);
        va_end(ap);
        return result;
    }

    std::string StringPrintV(const char* format, va_list ap) {
        std::string result;
        StringAppendV(&result, format, ap);
        return result;
    }

    void StringAppendF(std::string* dst, const char* format, ...) {
        va_list ap;
        va_start(ap, format);
        StringAppendV(dst, format, ap);
        va_end(ap);
    }

    std::string CanonicalizeBluetoothAddress(
        const std::vector<uint8_t>& address_bytes)
    {
        return StringPrintf(
            "%02X:%02X:%02X:%02X:%02X:%02X", address_bytes[0], address_bytes[1],
            address_bytes[2], address_bytes[3], address_bytes[4], address_bytes[5]);
    }

    std::string CanonicalizeBluetoothAddress(std::string_view address)
    {
        //std::array<uint8_t, 6> bytes;
        std::vector<uint8_t> bytes(6);

        if (!ParseBluetoothAddress(address, bytes))
            return std::string();

        return CanonicalizeBluetoothAddress(bytes);
    }

    void BluetoothAdapterWinrt::OnGetDefaultAdapter(
        ScopedClosureRunner on_init,
        ComPtr<IBluetoothAdapter> adapter)
    {
        if (!adapter) {
            BLUETOOTH_LOG(ERROR) << "Getting Default Adapter failed.";
            return;
        }

        adapter_ = std::move(adapter);
        uint64_t raw_address;
        HRESULT hr = adapter_->get_BluetoothAddress(&raw_address);
        if (FAILED(hr)) {
            return;
        }

        address_ =
            CanonicalizeBluetoothAddress(StringPrintf("%012llX", raw_address));
        DCHECK(!address_.empty());

        HSTRING device_id;
        hr = adapter_->get_DeviceId(&device_id);
        if (FAILED(hr)) {
            return;
        }

        ComPtr<IAsyncOperation<DeviceInformation*>> create_from_id_op;
        hr = device_information_statics_->CreateFromIdAsync(device_id,
            &create_from_id_op);
        if (FAILED(hr)) {
            return;
        }

        ComPtr<IDeviceInformation> device_info;
        create_from_id_op->GetResults(device_info.GetAddressOf());

        OnCreateFromIdAsync(std::move(on_init), device_info);

        /*hr = base::win::PostAsyncResults(
            std::move(create_from_id_op),
            base::BindOnce(&BluetoothAdapterWinrt::OnCreateFromIdAsync,
                weak_ptr_factory_.GetWeakPtr(), std::move(on_init)));*/
    }

    std::string HSTRING_To_string(HSTRING str)
    {
        UINT32 length = 0;
        const wchar_t* buffer = ::WindowsGetStringRawBuffer(str, &length);
        std::wstring wstr(buffer, length);

        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::string utf8Input = converter.to_bytes(wstr);

        return utf8Input;
    }

    void BluetoothAdapterWinrt::OnCreateFromIdAsync(
        ScopedClosureRunner on_init,
        ComPtr<IDeviceInformation> device_information)
    {
        if (!device_information) {
            BLUETOOTH_LOG(ERROR) << "Getting Device Information failed.";
            return;
        }

        HSTRING name;
        HRESULT hr = device_information->get_Name(&name);
        if (FAILED(hr)) {
            return;
        }

        name_ = HSTRING_To_string(name);

        ComPtr<IAsyncOperation<RadioAccessStatus>> request_access_op;
        hr = radio_statics_->RequestAccessAsync(&request_access_op);
        if (FAILED(hr)) {
            return;
        }

        RadioAccessStatus radio_access_status;
        request_access_op->GetResults(&radio_access_status);

        OnRequestRadioAccess(std::move(on_init), radio_access_status);

        /*hr = base::win::PostAsyncResults(
            std::move(request_access_op),
            base::BindOnce(&BluetoothAdapterWinrt::OnRequestRadioAccess,
                weak_ptr_factory_.GetWeakPtr(), std::move(on_init)));*/
    }

    void BluetoothAdapterWinrt::OnRequestRadioAccess(
        ScopedClosureRunner on_init,
        RadioAccessStatus access_status)
    {
        radio_access_allowed_ = access_status == RadioAccessStatus_Allowed;
        if (!radio_access_allowed_) {
            // This happens if "Allow apps to control device radios" is off in Privacy
            // settings.
            BLUETOOTH_LOG(ERROR) << "RequestRadioAccessAsync failed: "
                << ToCString(access_status)
                << "Will not be able to change radio power.";
        }

        ComPtr<IAsyncOperation<Radio*>> get_radio_op;
        HRESULT hr = adapter_->GetRadioAsync(&get_radio_op);
        if (FAILED(hr)) {
            return;
        }

        ComPtr<IRadio> radioo;
        get_radio_op->GetResults(radioo.GetAddressOf());

        OnGetRadio(std::move(on_init), radioo);

        /*hr = base::win::PostAsyncResults(
            std::move(get_radio_op),
            base::BindOnce(&BluetoothAdapterWinrt::OnGetRadio,
                weak_ptr_factory_.GetWeakPtr(), std::move(on_init)));*/
    }

    HSTRING CreateHString(std::wstring str)
    {
        HSTRING hstr;
        HRESULT hr = ::WindowsCreateString(str.data(),
            static_cast<UINT32>(str.length()), &hstr);
        return hstr;
    }

    void BluetoothAdapterWinrt::OnPoweredRadioAdded(IDeviceWatcher* watcher,
        IDeviceInformation* info)
    {
        /*if (++num_powered_radios_ == 1)
            NotifyAdapterPoweredChanged(true);
        BLUETOOTH_LOG(ERROR) << "OnPoweredRadioAdded(), Number of Powered Radios: "
            << num_powered_radios_;*/
    }

    void BluetoothAdapterWinrt::OnPoweredRadioRemoved(
        IDeviceWatcher* watcher,
        IDeviceInformationUpdate* update)
    {
        /*if (--num_powered_radios_ == 0)
            NotifyAdapterPoweredChanged(false);
        BLUETOOTH_LOG(ERROR) << "OnPoweredRadioRemoved(), Number of Powered Radios: "
            << num_powered_radios_;*/
    }

    void BluetoothAdapterWinrt::OnPoweredRadiosEnumerated(IDeviceWatcher* watcher,
        IInspectable* object)
    {
        //BLUETOOTH_LOG(ERROR)
        //    << "OnPoweredRadiosEnumerated(), Number of Powered Radios: "
        //    << num_powered_radios_;
        //// Destroy the ScopedClosureRunner, triggering the contained Closure to be
        //// run. Note this may destroy |this|.
        //DCHECK(on_init_);
        //on_init_.reset();
    }

    void BluetoothAdapterWinrt::OnRadioStateChanged(IRadio* radio,
        IInspectable* object)
    {
        /*DCHECK(radio_.Get() == radio);
        RunPendingPowerCallbacks();*/

        // Deduplicate StateChanged events, which can occur twice for a single
        // power-on change.
        const bool is_powered = GetState(radio) == RadioState_On;
        if (radio_was_powered_ == is_powered) {
            return;
        }
        radio_was_powered_ = is_powered;
        //NotifyAdapterPoweredChanged(is_powered);
    }

    void BluetoothAdapterWinrt::OnGetRadio(ScopedClosureRunner on_init,
        ComPtr<IRadio> radio)
    {
        if (radio) {
            radio_ = std::move(radio);
            radio_was_powered_ = GetState(radio_.Get()) == RadioState_On;

            auto handler0 = Microsoft::WRL::Callback<
                ABI::Windows::Foundation::ITypedEventHandler<
                ABI::Windows::Devices::Radios::Radio*,
                IInspectable*>>
                ([this](IRadio* sender, IInspectable* args) {
                OnRadioStateChanged(sender, args);
                return S_OK;
                    });

            EventRegistrationToken powered_radio_added_token_;
            radio_->add_StateChanged(handler0.Get(), &powered_radio_added_token_);


            /*radio_state_changed_token_ = AddTypedEventHandler(
                radio_.Get(), &IRadio::add_StateChanged,
                base::BindRepeating(&BluetoothAdapterWinrt::OnRadioStateChanged,
                    weak_ptr_factory_.GetWeakPtr()));*/

            /*if (!radio_state_changed_token_)
                BLUETOOTH_LOG(ERROR) << "Adding Radio State Changed Handler failed.";*/
            return;
        }

        // This happens within WoW64, due to an issue with non-native APIs.
        BLUETOOTH_LOG(ERROR)
            << "Getting Radio failed. Chrome will be unable to change the power "
            "state by itself.";

        // Attempt to create a DeviceWatcher for powered radios, so that querying
        // the power state is still possible.
        auto aqs_filter = CreateHString(kPoweredRadiosAqsFilter);
        HRESULT hr = device_information_statics_->CreateWatcherAqsFilter(
            aqs_filter, &powered_radio_watcher_);

        if (FAILED(hr)) {
            return;
        }

        auto handler1 = Microsoft::WRL::Callback<
            ABI::Windows::Foundation::ITypedEventHandler<
            ABI::Windows::Devices::Enumeration::DeviceWatcher*,
            ABI::Windows::Devices::Enumeration::DeviceInformation*>>
            ([this](IDeviceWatcher* sender, IDeviceInformation* args) {
            OnPoweredRadioAdded(sender, args);
            return S_OK;
                });

        EventRegistrationToken powered_radio_added_token_;
        powered_radio_watcher_->add_Added(handler1.Get(), &powered_radio_added_token_);

        auto handler2 = Microsoft::WRL::Callback<
            ABI::Windows::Foundation::ITypedEventHandler<
            ABI::Windows::Devices::Enumeration::DeviceWatcher*,
            ABI::Windows::Devices::Enumeration::DeviceInformationUpdate*>>
            ([this](IDeviceWatcher* sender, IDeviceInformationUpdate* args) {
            OnPoweredRadioRemoved(sender, args);
            return S_OK;
                });

        EventRegistrationToken powered_radio_removed_token_;
        powered_radio_watcher_->add_Removed(handler2.Get(), &powered_radio_removed_token_);

        auto handler3 = Microsoft::WRL::Callback<
            ABI::Windows::Foundation::ITypedEventHandler<
            ABI::Windows::Devices::Enumeration::DeviceWatcher*,
            IInspectable*>>
            ([this](IDeviceWatcher* sender, IInspectable* args) {
            OnPoweredRadiosEnumerated(sender, args);
            return S_OK;
                });

        EventRegistrationToken powered_radios_enumerated_token_;
        powered_radio_watcher_->add_EnumerationCompleted(handler3.Get(), &powered_radios_enumerated_token_);

        hr = powered_radio_watcher_->Start();
        if (FAILED(hr)) {
            return;
        }

        // Store the Closure Runner. It is expected that OnPoweredRadiosEnumerated()
        // is invoked soon after.
        on_init_ = std::make_unique<ScopedClosureRunner>(std::move(on_init));
    }

    BluetoothAdapterWinrt::StaticsInterfaces::StaticsInterfaces(
        ComPtr<IAgileReference> adapter_statics_in,
        ComPtr<IAgileReference> device_information_statics_in,
        ComPtr<IAgileReference> radio_statics_in)
        : adapter_statics(std::move(adapter_statics_in)),
        device_information_statics(std::move(device_information_statics_in)),
        radio_statics(std::move(radio_statics_in)) {}

    BluetoothAdapterWinrt::StaticsInterfaces::StaticsInterfaces(
        const StaticsInterfaces& copy_from) = default;

    BluetoothAdapterWinrt::StaticsInterfaces::StaticsInterfaces() = default;

    BluetoothAdapterWinrt::StaticsInterfaces::~StaticsInterfaces() {}

    enum class ComApartmentType {
        // Uninitialized or has an unrecognized apartment type.
        NONE,
        // Single-threaded Apartment.
        STA,
        // Multi-threaded Apartment.
        MTA,
    };

    // Derived from combase.dll.
    struct OleTlsData {
        enum ApartmentFlags {
            LOGICAL_THREAD_REGISTERED = 0x2,
            STA = 0x80,
            MTA = 0x140,
        };

        uintptr_t thread_base;
        uintptr_t sm_allocator;
        DWORD apartment_id;
        DWORD apartment_flags;
        // There are many more fields than this, but for our purposes, we only care
        // about |apartment_flags|. Correctly declaring the previous types allows this
        // to work between x86 and x64 builds.
    };

    OleTlsData* GetOleTlsData() {
        TEB* teb = NtCurrentTeb();
        return reinterpret_cast<OleTlsData*>(teb->ReservedForOle);
    }

    ComApartmentType GetComApartmentTypeForThread() {
        OleTlsData* ole_tls_data = GetOleTlsData();
        if (!ole_tls_data)
            return ComApartmentType::NONE;

        if (ole_tls_data->apartment_flags & OleTlsData::ApartmentFlags::STA)
            return ComApartmentType::STA;

        if ((ole_tls_data->apartment_flags & OleTlsData::ApartmentFlags::MTA) ==
            OleTlsData::ApartmentFlags::MTA) {
            return ComApartmentType::MTA;
        }

        return ComApartmentType::NONE;
    }

    void AssertComApartmentType(ComApartmentType apartment_type)
    {
        if (apartment_type == GetComApartmentTypeForThread())
        {
            apartment_type;
        }
    }

    BluetoothAdapterWinrt::StaticsInterfaces
        BluetoothAdapterWinrt::PerformSlowInitTasks()
    {
        AssertComApartmentType(ComApartmentType::MTA);
        ComPtr<IBluetoothAdapterStatics> adapter_statics;

        auto class_id = HStringReference(RuntimeClass_Windows_Devices_Bluetooth_BluetoothAdapter);
        HRESULT hr = RoGetActivationFactory(class_id.Get(), IID_PPV_ARGS(adapter_statics.GetAddressOf()));

        if (FAILED(hr)) {
            return BluetoothAdapterWinrt::StaticsInterfaces();
        }

        ComPtr<IDeviceInformationStatics> device_information_statics;
        auto class_id2 = HStringReference(RuntimeClass_Windows_Devices_Enumeration_DeviceInformation);
        hr = RoGetActivationFactory(class_id2.Get(), IID_PPV_ARGS(device_information_statics.GetAddressOf()));
        
        if (FAILED(hr)) {
            return BluetoothAdapterWinrt::StaticsInterfaces();
        }

        ComPtr<IRadioStatics> radio_statics;
        auto class_id3 = HStringReference(RuntimeClass_Windows_Devices_Radios_Radio);
        hr = RoGetActivationFactory(class_id3.Get(), IID_PPV_ARGS(radio_statics.GetAddressOf()));

        if (FAILED(hr)) {
            return BluetoothAdapterWinrt::StaticsInterfaces();
        }

        return GetAgileReferencesForStatics(std::move(adapter_statics),
            std::move(device_information_statics),
            std::move(radio_statics));
    }

    HMODULE LoadLibraryHelper(std::wstring file)
    {
        HMODULE module_handle = nullptr;

        // LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR flag is needed to search the library
        // directory as the library may have dependencies on DLLs in this
        // directory.
        module_handle = ::LoadLibraryExW(
            file.c_str(), nullptr,
            LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
        // If LoadLibraryExW succeeds, log this metric and return.
        if (module_handle) {
            return module_handle;
        }
    }

    void* GetFunctionPointer(HMODULE library, std::string name)
    {
        return reinterpret_cast<void*>(GetProcAddress(library, name.c_str()));
    }

    BluetoothAdapterWinrt::StaticsInterfaces
        BluetoothAdapterWinrt::GetAgileReferencesForStatics(
            ComPtr<IBluetoothAdapterStatics> adapter_statics,
            ComPtr<IDeviceInformationStatics> device_information_statics,
            ComPtr<IRadioStatics> radio_statics)
    {
        HMODULE ole32_library = LoadLibraryHelper(L"Ole32.dll");

        auto ro_get_agile_reference =
            reinterpret_cast<decltype(&::RoGetAgileReference)>(
                GetFunctionPointer(ole32_library, "RoGetAgileReference"));

        ComPtr<IAgileReference> adapter_statics_agileref;
        HRESULT hr = ro_get_agile_reference(
            AGILEREFERENCE_DEFAULT,
            ABI::Windows::Devices::Bluetooth::IID_IBluetoothAdapterStatics,
            adapter_statics.Get(), &adapter_statics_agileref);
        if (FAILED(hr))
            return StaticsInterfaces();

        ComPtr<IAgileReference> device_information_statics_agileref;
        hr = ro_get_agile_reference(
            AGILEREFERENCE_DEFAULT,
            ABI::Windows::Devices::Enumeration::IID_IDeviceInformationStatics,
            device_information_statics.Get(), &device_information_statics_agileref);
        if (FAILED(hr))
            return StaticsInterfaces();

        ComPtr<IAgileReference> radio_statics_agileref;
        hr = ro_get_agile_reference(AGILEREFERENCE_DEFAULT,
            ABI::Windows::Devices::Radios::IID_IRadioStatics,
            radio_statics.Get(), &radio_statics_agileref);
        if (FAILED(hr))
            return StaticsInterfaces();

        return StaticsInterfaces(std::move(adapter_statics_agileref),
            std::move(device_information_statics_agileref),
            std::move(radio_statics_agileref));
    }
}