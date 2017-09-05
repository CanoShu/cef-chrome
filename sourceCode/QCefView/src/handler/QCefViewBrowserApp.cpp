#pragma region stl_headers
#include <string>
#pragma endregion stl_headers

#pragma region cef_headers
#include <include/cef_browser.h>
#include <include/cef_command_line.h>
#include <include/wrapper/cef_helpers.h>
#pragma endregion cef_headers

#include "QCefViewBrowserHandler.h"
#include "QCefViewBrowserApp.h"
#include "CCefCookie.h"

QCefViewBrowserApp::QCefViewBrowserApp()
{
	
}

QCefViewBrowserApp::~QCefViewBrowserApp()
{
}
//////////////////////////////////////////////////////////////////////////
void QCefViewBrowserApp::OnBeforeCommandLineProcessing(
	const CefString& process_type, 
	CefRefPtr<CefCommandLine> command_line)
{
	command_line->AppendSwitch("disable-spell-checking");
	command_line->AppendSwitch("disable-extensions");
	command_line->AppendSwitch("disable-pdf-extension");
	command_line->AppendSwitch("enable-direct-write");
	command_line->AppendSwitch("allow-file-access-from-files");
}

void QCefViewBrowserApp::OnRegisterCustomSchemes(
	CefRefPtr<CefSchemeRegistrar> registrar)
{
	RegisterCustomSchemes(registrar);
}

CefRefPtr<CefBrowserProcessHandler> QCefViewBrowserApp::GetBrowserProcessHandler()
{
	return this;
}

//////////////////////////////////////////////////////////////////////////
void QCefViewBrowserApp::OnContextInitialized()
{
	CEF_REQUIRE_UI_THREAD();
	DEBUG_FUNC();
	// create all browser delegates
	CreateBrowserDelegates(browser_delegates_);

	// Register cookieable schemes with the global cookie manager.

	CefRefPtr<CefCookieManager> manager = CCefCookie::getInstance().getManager();
	DCHECK(manager.get());
	typedef std::vector<CefString> CookiableSchemeSet;
	CookiableSchemeSet cookieable_schemes_;
	cookieable_schemes_.push_back("http");
	cookieable_schemes_.push_back("https");
	manager->SetSupportedSchemes(cookieable_schemes_, NULL);

	RegisterCustomSchemesHandlerFactories();

	BrowserDelegateSet::iterator it = browser_delegates_.begin();
	for (; it != browser_delegates_.end(); ++it)
	{
		(*it)->OnContextInitialized(this);
	}
}

void QCefViewBrowserApp::OnBeforeChildProcessLaunch(
	CefRefPtr<CefCommandLine> command_line)
{
	DEBUG_FUNC();
	BrowserDelegateSet::iterator it = browser_delegates_.begin();
	for (; it != browser_delegates_.end(); ++it)
	{
		(*it)->OnBeforeChildProcessLaunch(this, command_line);
	}
}

void QCefViewBrowserApp::OnRenderProcessThreadCreated(
	CefRefPtr<CefListValue> extra_info)
{
	CEF_REQUIRE_IO_THREAD();
	DEBUG_FUNC();
	BrowserDelegateSet::iterator it = browser_delegates_.begin();
	for (; it != browser_delegates_.end(); ++it)
	{
		(*it)->OnRenderProcessThreadCreated(this, extra_info);
	}
}

