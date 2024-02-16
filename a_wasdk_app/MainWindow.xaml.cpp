#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include "a_win32_dll/nearby_api.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;

using namespace nearby::windows;

#define MAX_THREADS 5
DWORD   dwThreadIdArray[MAX_THREADS];
HANDLE  hThreadArray[MAX_THREADS] = {};

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

void DeviceAdded(std::string device_name, std::string endpoint_id)
{
	OutputDebugStringA(device_name.c_str());
	//OutputDebugStringA(endpoint_id.c_str());
}

namespace winrt::a_wasdk_app::implementation
{
    int32_t MainWindow::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void MainWindow::MyProperty(int32_t /* value */)
    {
		throw hresult_not_implemented();
	}

	void StartWatcher()
	{
		auto work = []() {
			NearbyShareAPI* nearby = new NearbyShareAPI();
			nearby->InitializeNearby();
			nearby->StartScanning2(DeviceAdded);

			while (true) {
				WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);
				Sleep(10);
			}
			};
		std::thread([work]() {
			work();
			}).detach();
	}

    void MainWindow::myButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        myButton().Content(box_value(L"Clicked"));

		StartWatcher();
    }
}
