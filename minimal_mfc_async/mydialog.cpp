#include "mydialog.hpp"

// windows
#include <windows.h>
#include <winuser.h>

// cstd
#include <cassert>

// std
#include <chrono>
#include <future>

const UINT APP_STATUS_UPDATE = ::RegisterWindowMessageW(L"APP_STATUS_UPDATE");

BEGIN_MESSAGE_MAP(MyDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTONTEST, &MyDialog::OnBnClickedButtontest)
	ON_REGISTERED_MESSAGE(APP_STATUS_UPDATE, &MyDialog::UpdateData)
END_MESSAGE_MAP()


struct MyData {
	std::wstring str;
	explicit MyData(const std::wstring& str_) : str(str_) {};
};

int intensive_task(const HWND hWnd, const UINT messagevar) {
	{
		MyData data(L"Doing intensive task...");
		// SendMessage waits until the window procedure finishes processing the message.
		// PostMessage posts the message to the listening thread's queue and returns immediately, without waiting for the result
		// If we would use PostMessage, memory handling gets more complicated since we need to track who is owning the resource
		static_assert(sizeof(WPARAM) == sizeof(&data), "");
		const auto res = ::SendMessageW(hWnd, messagevar, reinterpret_cast<WPARAM>(&data), 0);
	}

	std::this_thread::sleep_for(std::chrono::seconds(2));

	{
		MyData data(L"Finished...");
		const auto res = ::SendMessageW(hWnd, messagevar, reinterpret_cast<WPARAM>(&data), 0);
	}

	return 42;
}

MyDialog::MyDialog(CWnd* pParent ) : CDialog(IDD_DIALOG1, pParent) {
}

void MyDialog::OnBnClickedButtontest() {
	c_buttontest.EnableWindow(FALSE);
	fut = std::async(std::launch::async, intensive_task, m_hWnd, APP_STATUS_UPDATE);
}


LRESULT MyDialog::UpdateData(const WPARAM wp, const LPARAM) {
	const auto data = reinterpret_cast<MyData*>(wp);
	assert(data != nullptr);
	c_buttontest.SetWindowTextW(data->str.c_str());
	return 0;
}

