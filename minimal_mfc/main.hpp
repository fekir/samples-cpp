#pragma once

// local files
#include "mydialog.hpp"
#include "resource.h"

// mfc
#include <afxwin.h>

// windows
#include <windows.h>

// std
#include <iostream>

class MyApp final : public CWinApp {
public:
	MyApp() {}

public:
	virtual BOOL InitInstance() override {
		if(!CWinApp::InitInstance()){
			return FALSE;
		}
		MyDialog dlg;
		m_pMainWnd = &dlg;
		const INT_PTR nResponse = dlg.DoModal();
		if (nResponse == -1) {
			std::cerr << "Warning: Dialog creation failed, so application is terminating unexpectedly.\n";
		}
		return FALSE; // close function
	}

};
