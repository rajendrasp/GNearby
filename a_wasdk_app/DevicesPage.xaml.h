#pragma once

#include "DevicesPage.g.h"

namespace winrt::a_wasdk_app::implementation
{
    struct DevicesPage : DevicesPageT<DevicesPage>
    {
        DevicesPage();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        Windows::Foundation::Collections::IVector<Windows::Foundation::IInspectable> Devices() const
        {
            return m_devices;
        }

        int TransferProgress() const
        {
            return m_TransferProgress;
        }

        void TransferProgress(int progress);

        bool ShowProgress()
        {
            return m_showProgress;
        }

        bool ShowDevicesSection()
        {
            return m_ShowDevicesSection;
        }

        bool ShowTransferComplete()
        {
            return m_ShowTransferComplete;
        }

        hstring FileName()
        {
            return m_fileName;
        }

        void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs);

        fire_and_forget PopulateDevice(hstring device_name, hstring endpoint_id);
        fire_and_forget UpdateProgress(int progress);
        void OnTransferComplete();

        void StartWatcher();

        //Windows::Foundation::IInspectable GetPhoneDevice(hstring name);

        // Event handler.
        void ImageGridView_ItemClick(Windows::Foundation::IInspectable const, Microsoft::UI::Xaml::Controls::ItemClickEventArgs const);

        // Property changed notifications.
        event_token PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const&);
        void PropertyChanged(event_token const&);

        void myButton_Click(IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);

        // Backing field for Photo collection.
        Windows::Foundation::Collections::IVector<IInspectable> m_devices{ nullptr };

        // Property changed notifications.
        void RaisePropertyChanged(hstring const&);
        event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;

        Microsoft::UI::Dispatching::DispatcherQueue m_dispatcher{ nullptr };

        std::wstring m_filePath;

        hstring m_fileName;
        int m_TransferProgress{ 0 };
        bool m_showProgress{ false };
        bool m_ShowDevicesSection{ true };
        bool m_ShowTransferComplete{ false };
        fire_and_forget OpenFileButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::a_wasdk_app::factory_implementation
{
    struct DevicesPage : DevicesPageT<DevicesPage, implementation::DevicesPage>
    {
    };
}
