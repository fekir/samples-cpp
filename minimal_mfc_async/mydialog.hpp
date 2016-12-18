#pragma once

// local
#include "resource.h"

// mfc
#include <afxwin.h>
#include <Windows.h>

// std
#include <thread>
#include <string>
#include <memory>
#include <future>


class MyDialog final : public CDialog {
public:
	explicit MyDialog(CWnd* pParent = nullptr);
	~MyDialog() = default;

protected:
	CButton c_buttontest;
	std::future<int> fut;

	virtual void DoDataExchange(CDataExchange* pDX) override {
		CDialog::DoDataExchange(pDX);
		DDX_Control(pDX, IDC_BUTTONTEST, c_buttontest);
	}

	LRESULT UpdateData(const WPARAM wp, const LPARAM);

	void OnBnClickedButtontest();

	DECLARE_MESSAGE_MAP()
};
