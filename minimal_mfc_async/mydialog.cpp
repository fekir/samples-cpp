#include "MyDialog.hpp"

// windows
#include <Windows.h>
#include <WinUser.h>

// std
#include <chrono>
#include <cassert>
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
		auto data = std::make_unique<MyData>(L"Doing intensive task...");
		// PostMessage posts the message to the listening thread's queue and returns immediately, without waiting for the result
		// We need to pay attention that does does not get deleted in the meantime -> copy or allocate on the heap
		if (!::PostMessageW(hWnd, messagevar, reinterpret_cast<WPARAM>(data.release()), 0)) {
			throw std::runtime_error("Unable to post firs message!");
		}
	}

	std::this_thread::sleep_for(std::chrono::seconds(2));

	{
		auto data = std::make_unique<MyData>(L"Finished intensive task!");
		// SendMessage waits until the window procedure finishes processing the message.
		// We could safely delete the data, in this case we are calling the same function as before
		if (!::SendMessageW(hWnd, messagevar, reinterpret_cast<WPARAM>(data.release()), 0)) {
			throw std::runtime_error("Unable to send last message!");
		}
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
	std::unique_ptr<MyData> data(reinterpret_cast<MyData*>(wp));
	c_buttontest.SetWindowTextW(data->str.c_str());
	return 0;
}