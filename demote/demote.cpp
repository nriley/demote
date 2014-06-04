// demote, aka "scratch"
// from http://blogs.msdn.com/b/oldnewthing/archive/2013/11/18/10468726.aspx

#include "stdafx.h"
#include <shlobj.h>
#include <atlbase.h>

void FindDesktopFolderView(REFIID riid, void **ppv)
{
 CComPtr<IShellWindows> spShellWindows;
 spShellWindows.CoCreateInstance(CLSID_ShellWindows);

 CComVariant vtLoc(CSIDL_DESKTOP);
 CComVariant vtEmpty;
 long lhwnd;
 CComPtr<IDispatch> spdisp;
 spShellWindows->FindWindowSW(
     &vtLoc, &vtEmpty,
     SWC_DESKTOP, &lhwnd, SWFO_NEEDDISPATCH, &spdisp);

 CComPtr<IShellBrowser> spBrowser;
 CComQIPtr<IServiceProvider>(spdisp)->
     QueryService(SID_STopLevelBrowser,
                  IID_PPV_ARGS(&spBrowser));

 CComPtr<IShellView> spView;
 spBrowser->QueryActiveShellView(&spView);

 spView->QueryInterface(riid, ppv);
}

void GetDesktopAutomationObject(REFIID riid, void **ppv)
{
 CComPtr<IShellView> spsv;
 FindDesktopFolderView(IID_PPV_ARGS(&spsv));
 CComPtr<IDispatch> spdispView;
 spsv->GetItemObject(SVGIO_BACKGROUND, IID_PPV_ARGS(&spdispView));
 spdispView->QueryInterface(riid, ppv);
}

void ShellExecuteFromExplorer(
    PCWSTR pszFile,
    PCWSTR pszParameters = nullptr,
    PCWSTR pszDirectory  = nullptr,
    PCWSTR pszOperation  = nullptr,
    int nShowCmd         = SW_SHOWNORMAL)
{
 CComPtr<IShellFolderViewDual> spFolderView;
 GetDesktopAutomationObject(IID_PPV_ARGS(&spFolderView));
 CComPtr<IDispatch> spdispShell;
 spFolderView->get_Application(&spdispShell);

 CComQIPtr<IShellDispatch2>(spdispShell)
    ->ShellExecute(CComBSTR(pszFile),
                   CComVariant(pszParameters ? pszParameters : L""),
                   CComVariant(pszDirectory ? pszDirectory : L""),
                   CComVariant(pszOperation ? pszOperation : L""),
                   CComVariant(nShowCmd));
}

class CCoInitialize {
public:
 CCoInitialize() : m_hr(CoInitialize(NULL)) { }
 ~CCoInitialize() { if (SUCCEEDED(m_hr)) CoUninitialize(); }
 operator HRESULT() const { return m_hr; }
 HRESULT m_hr;
};

int _tmain(int argc, _TCHAR* argv[])
{
 if (argc < 2) return 0;

 CCoInitialize init;
 ShellExecuteFromExplorer(
    argv[1],
    argc >= 3 ? argv[2] : L"",
    argc >= 4 ? argv[3] : L"",
    argc >= 5 ? argv[4] : L"",
    argc >= 6 ? _wtoi(argv[5]) : SW_SHOWNORMAL);

 return 0;
}

