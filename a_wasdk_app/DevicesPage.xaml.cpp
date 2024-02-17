#include "pch.h"
#include "DevicesPage.xaml.h"
#if __has_include("DevicesPage.g.cpp")
#include "DevicesPage.g.cpp"
#endif

#include <winrt/Microsoft.UI.Xaml.Hosting.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include "winrt/Windows.Storage.Pickers.h"
#include "winrt/Windows.Storage.Pickers.Provider.h"
#include "PhoneDevice.h"
#include "MainWindow.xaml.h"
#include <microsoft.ui.xaml.window.h>
#include <Shobjidl.h>

#include "a_win32_dll/nearby_api.h"

extern winrt::a_wasdk_app::implementation::MainWindow* g_mainWindow;

using namespace winrt;
using namespace nearby::windows;
using namespace Windows::Storage::Pickers;

namespace winrt
{
    using namespace Microsoft::UI::Composition;
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::UI::Xaml::Controls;
    using namespace Microsoft::UI::Xaml::Data;
    using namespace Microsoft::UI::Xaml::Hosting;
    using namespace Microsoft::UI::Xaml::Media::Animation;
    using namespace Microsoft::UI::Xaml::Media::Imaging;
    using namespace Microsoft::UI::Xaml::Navigation;
    //using namespace Windows::Foundation;
    using namespace Windows::Foundation::Collections;
    using namespace Windows::Storage;
    using namespace Windows::Storage::Search;
}

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

winrt::a_wasdk_app::implementation::DevicesPage* g_page = nullptr;
NearbyShareAPI* g_nearby = nullptr;

void PhoneDeviceAdded(std::string device_name, std::string endpoint_id)
{
    OutputDebugStringA(device_name.c_str());
    //OutputDebugStringA(endpoint_id.c_str());

    std::wstring stemp = std::wstring(device_name.begin(), device_name.end());
    std::wstring endpoint(endpoint_id.begin(), endpoint_id.end());

    g_page->PopulateDevice(stemp.c_str(), endpoint.c_str());
}


namespace winrt::a_wasdk_app::implementation
{
    DevicesPage::DevicesPage()
        :m_devices(winrt::single_threaded_observable_vector<IInspectable>())
    {
        g_page = this;

        m_dispatcher = Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
        //auto dispatcherQueue = DispatcherQueue::GetForCurrentThread();

        // Xaml objects should not call InitializeComponent during construction.
        // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
    }

    int32_t DevicesPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void DevicesPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void DevicesPage::TransferProgress(int progress)
    {
        m_TransferProgress = progress;
        RaisePropertyChanged(L"TransferProgress");
    }

    // Loads collection of Photos from users Pictures library.
    void DevicesPage::OnNavigatedTo(NavigationEventArgs e)
    {
        // Load photos if they haven't previously been loaded.
        auto info1 = winrt::make<PhoneDevice>(L"Rajendra", L"LDKF");
        auto info2 = winrt::make<PhoneDevice>(L"Aditya", L"JHFG");
        auto info3 = winrt::make<PhoneDevice>(L"Pandu", L"IUTT");
        Devices().Append(info1);
        Devices().Append(info2);
        Devices().Append(info3);
    }

    // Registers property changed event handler.
    event_token DevicesPage::PropertyChanged(PropertyChangedEventHandler const& value)
    {
        return m_propertyChanged.add(value);
    }

    // Unregisters property changed event handler.
    void DevicesPage::PropertyChanged(event_token const& token)
    {
        m_propertyChanged.remove(token);
    }

    // Triggers property changed notification.
    void DevicesPage::RaisePropertyChanged(hstring const& propertyName)
    {
        m_propertyChanged(*this, PropertyChangedEventArgs(propertyName));
    }

    fire_and_forget DevicesPage::PopulateDevice(hstring device_name, hstring endpoint_id)
    {
        const auto strongThis{ get_strong() };

        m_dispatcher.TryEnqueue([this, device_name, endpoint_id]()
            {
                auto info1 = winrt::make<PhoneDevice>(device_name, endpoint_id);
                Devices().Append(info1);

                RaisePropertyChanged(L"Devices");

            });

        co_return;
    }

    fire_and_forget DevicesPage::UpdateProgress(int progress)
    {
        const auto strongThis{ get_strong() };

        m_dispatcher.TryEnqueue([this, progress]()
            {
                TransferProgress(progress);

                RaisePropertyChanged(L"Devices");

            });

        co_return;
    }

    void DevicesPage::OnTransferComplete()
    {
        const auto strongThis{ get_strong() };

        m_dispatcher.TryEnqueue([this]()
            {
                m_showProgress = false;
                m_ShowDevicesSection = false;
                m_ShowTransferComplete = true;

                RaisePropertyChanged(L"ShowProgress");
                RaisePropertyChanged(L"ShowDevicesSection");
                RaisePropertyChanged(L"ShowTransferComplete");

            });
    }

    /*Windows::Foundation::IInspectable DevicesPage::GetPhoneDevice(hstring name)
    {
        for (auto& device : Devices())
        {
            auto phone = device.as<PhoneDevice>();
            if (phone->DeviceName() == name)
            {
                return device;
            }
        }
        return nullptr;
    }*/

    void DevicesPage::StartWatcher()
    {
        auto work = []() {
            Sleep(5000);

            g_page->PopulateDevice(L"Rajendra", L"DFLK");

            };
        std::thread([work]() {
            work();
            }).detach();
    }

    void StartPhoneWatcher()
    {
        auto work = []() {
            g_nearby = new NearbyShareAPI();
            g_nearby->InitializeNearby();
            g_nearby->StartScanning2(PhoneDeviceAdded);

            while (true) {
                Sleep(100);
            }
            };
        std::thread([work]() {
            work();
            }).detach();
    }

    void StartTransfer(hstring endpointId, hstring filePath)
    {
        auto work = [filePath, endpointId]() {
            std::string file = winrt::to_string(filePath);
            std::string endpoint = winrt::to_string(endpointId);

            g_nearby->SendAttachments(endpoint, file);


            while (true) {
                Sleep(1000);
                if (g_page->TransferProgress() > 100)
                {
                    break;
                }
                g_page->UpdateProgress(g_page->TransferProgress() + 5);
            }

            g_page->OnTransferComplete();

            };
        std::thread([work]() {
            work();
            }).detach();
    }

    void DevicesPage::myButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        StartPhoneWatcher();
    }

    // Device clicked event handler
    void DevicesPage::ImageGridView_ItemClick(IInspectable const sender, ItemClickEventArgs const e)
    {
        // Connect to device and send the payload

        auto device = e.ClickedItem().as<PhoneDevice>();

        auto deviceName = device->DeviceName();
        auto endpointId = device->EndpointId();

        StartTransfer(endpointId, m_filePath.c_str());

        m_showProgress = true;
        m_ShowDevicesSection = false;
        RaisePropertyChanged(L"ShowProgress");
        RaisePropertyChanged(L"ShowDevicesSection");
    }


    fire_and_forget DevicesPage::OpenFileButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        auto file = co_await g_mainWindow->BrowseFileAsync();
        m_filePath = file.Path();
        m_fileName = file.Name();

        RaisePropertyChanged(L"FileName");
        co_return;
    }
}


