#include "pch.h"
#include "DevicesPage.xaml.h"
#if __has_include("DevicesPage.g.cpp")
#include "DevicesPage.g.cpp"
#endif

#include <winrt/Microsoft.UI.Xaml.Hosting.h>
#include "PhoneDevice.h"

using namespace winrt;

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
    using namespace Windows::Foundation;
    using namespace Windows::Foundation::Collections;
    using namespace Windows::Storage;
    using namespace Windows::Storage::Search;
    using namespace Windows::Storage::Streams;
}

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::a_wasdk_app::implementation
{
    int32_t DevicesPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void DevicesPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    // Loads collection of Photos from users Pictures library.
    void DevicesPage::OnNavigatedTo(NavigationEventArgs e)
    {
        // Load photos if they haven't previously been loaded.
        auto info1 = winrt::make<PhoneDevice>(L"Rajendra");
        auto info2 = winrt::make<PhoneDevice>(L"Aditya");
        auto info3 = winrt::make<PhoneDevice>(L"Pandu");
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

    void DevicesPage::myButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        myButton().Content(box_value(L"Clicked"));
    }

    // Device clicked event handler
    void DevicesPage::ImageGridView_ItemClick(IInspectable const sender, ItemClickEventArgs const e)
    {
        // Connect to device and send the payload
    }
}
