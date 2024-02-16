#pragma once

#include "DevicesPage.g.h"

namespace winrt::a_wasdk_app::implementation
{
    struct DevicesPage : DevicesPageT<DevicesPage>
    {
        DevicesPage()
            :m_devices(winrt::single_threaded_observable_vector<IInspectable>())
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }

        int32_t MyProperty();
        void MyProperty(int32_t value);

        Windows::Foundation::Collections::IVector<Windows::Foundation::IInspectable> Devices() const
        {
            return m_devices;
        }

        void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs);

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
    };
}

namespace winrt::a_wasdk_app::factory_implementation
{
    struct DevicesPage : DevicesPageT<DevicesPage, implementation::DevicesPage>
    {
    };
}
