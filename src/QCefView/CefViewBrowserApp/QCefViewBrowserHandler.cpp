#include "stdafx.h"

#pragma region stl_headers
#include <sstream>
#include <string>
#pragma endregion stl_headers

#pragma region cef_headers
#include <include/cef_app.h>
#include <include/cef_runnable.h>
#include <include/wrapper/cef_helpers.h>
#pragma endregion cef_headers

#include <QDebug>
#include <QVariant>
#include "QCefViewBrowserHandler.h"
#include "QCefQueryHandler.h"

QCefViewBrowserHandler::QCefViewBrowserHandler(QCefView* host)
	: is_closing_(false)
	, hostWidget_(host)
	, message_router_(NULL)
	, browser_count_(0)
{
	// Create a message Handler
	cefquery_handler_ = new QCefQueryHandler(hostWidget_);
}

QCefViewBrowserHandler::~QCefViewBrowserHandler()
{
}

bool QCefViewBrowserHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
	CefProcessId source_process,
	CefRefPtr<CefProcessMessage> message)
{
	CEF_REQUIRE_UI_THREAD();
	if (message_router_->OnProcessMessageReceived(
		browser, source_process, message))
	{
		return true;
	}

	if (DispatchNotifyRequest(browser, source_process, message))
	{
		return true;
	}

	return false;
}

void QCefViewBrowserHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefContextMenuParams> params,
	CefRefPtr<CefMenuModel> model)
{
	CEF_REQUIRE_UI_THREAD();
	qDebug() << __FUNCTION__;

	std::list<_contextMenuInfo> infoList = hostWidget_->getMenuItems();
	if (infoList.size() == 0) {
		return;
	}
	model->Clear();
	infoList.sort([](const _contextMenuInfo & a, const _contextMenuInfo & b) { return a.index < b.index; });

	for (std::list<_contextMenuInfo>::iterator it = infoList.begin(); it != infoList.end();)
	{
		_contextMenuInfo info = *it;
		model->AddItem(info.command, info.lableName.toLatin1().data());
		model->SetEnabled(info.command, info.enable);
		if (++it != infoList.end())
		{
			model->AddSeparator();
		}				
	}
}

bool QCefViewBrowserHandler::OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefContextMenuParams> params,
	int command_id, EventFlags event_flags)
{
	CEF_REQUIRE_UI_THREAD();
	qDebug() << __FUNCTION__;
	std::list<_contextMenuInfo> infoList = hostWidget_->getMenuItems();
	if (infoList.size() == 0) 
	{
		return true;
	}
	else
	{
		return hostWidget_->excContextMenu(command_id);
	}

	
}

//bool QCefViewHandler::OnFileDialog(CefRefPtr<CefBrowser> browser, 
//	FileDialogMode mode, 
//	const CefString& title, 
//	const CefString& default_file_name, 
//	const std::vector<CefString>& accept_types, 
//	CefRefPtr<CefFileDialogCallback> callback)
//{
//	CEF_REQUIRE_UI_THREAD();
//
//	return false;
//}

void QCefViewBrowserHandler::OnAddressChange(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	const CefString& url)
{
	CEF_REQUIRE_UI_THREAD();
	QMetaObject::invokeMethod(hostWidget_,
		"onUrlChange",
		Qt::QueuedConnection,
		Q_ARG(const QString&, QString::fromStdString(url.ToString())));
}

void QCefViewBrowserHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
	const CefString& title)
{
	CEF_REQUIRE_UI_THREAD();
	qDebug() << __FUNCTION__;
}

bool QCefViewBrowserHandler::OnConsoleMessage(CefRefPtr<CefBrowser> browser,
	const CefString& message,
	const CefString& source,
	int line)
{
	CEF_REQUIRE_UI_THREAD();
	qDebug() << __FUNCTION__;
	if (source.empty() || message.empty())
	{
		return false;
	}
	std::string src = source.ToString();
	std::size_t found = src.find_last_of("/\\");
	if (found != std::string::npos && found < src.length() - 1)
	{
		src = src.substr(found + 1);
	}
	__noop(src, message.ToString());
	return false;
}

//void QCefViewHandler::OnBeforeDownload(CefRefPtr<CefBrowser> browser, 
//	CefRefPtr<CefDownloadItem> download_item, 
//	const CefString& suggested_name, 
//	CefRefPtr<CefBeforeDownloadCallback> cal
//	CEF_REQUIRE_UI_THREAD();lback)
//{
//	CEF_REQUIRE_UI_THREAD();
//
//}

//void QCefViewHandler::OnDownloadUpdated(CefRefPtr<CefBrowser> browser, 
//	CefRefPtr<CefDownloadItem> download_item, 
//	CefRefPtr<CefDownloadItemCallback> callback)
//{
//	CEF_REQUIRE_UI_THREAD();
//
//}

bool QCefViewBrowserHandler::OnDragEnter(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefDragData> dragData,
	CefDragHandler::DragOperationsMask mask)
{
	CEF_REQUIRE_UI_THREAD();

	return true;
}

//bool QCefViewHandler::OnRequestGeolocationPermission(CefRefPtr<CefBrowser> browser, 
//	const CefString& requesting_url, 
//	int request_id, 
//	CefRefPtr<CefGeolocationCallback> callback)
//{
//	CEF_REQUIRE_UI_THREAD();
//
//	return false;
//}

bool QCefViewBrowserHandler::OnJSDialog(CefRefPtr<CefBrowser> browser,
	const CefString& origin_url,
	const CefString& accept_lang,
	JSDialogType dialog_type,
	const CefString& message_text,
	const CefString& default_prompt_text,
	CefRefPtr<CefJSDialogCallback> callback,
	bool& suppress_message)
{
	CEF_REQUIRE_UI_THREAD();
	qDebug() << __FUNCTION__;
	return false;
}

bool QCefViewBrowserHandler::OnBeforeUnloadDialog(CefRefPtr<CefBrowser> browser,
	const CefString& message_text,
	bool is_reload,
	CefRefPtr<CefJSDialogCallback> callback)
{
	CEF_REQUIRE_UI_THREAD();
	qDebug() << __FUNCTION__;
	return false;
}

void QCefViewBrowserHandler::OnResetDialogState(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();
	qDebug() << __FUNCTION__;
}

bool QCefViewBrowserHandler::OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
	const CefKeyEvent& event,
	CefEventHandle os_event,
	bool* is_keyboard_shortcut)
{
	CEF_REQUIRE_UI_THREAD();
	if (event.type == KEYEVENT_RAWKEYDOWN && event.windows_key_code == VK_ESCAPE)
	{
		CefRefPtr<CefFrame> frame = browser->GetMainFrame();
		frame->ExecuteJavaScript("{document.webkitExitFullscreen()}", frame->GetURL(), 0);
		return true;
	}
	return false;
}

bool QCefViewBrowserHandler::OnBeforePopup(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	const CefString& target_url,
	const CefString& target_frame_name,
	CefLifeSpanHandler::WindowOpenDisposition target_disposition,
	bool user_gesture,
	const CefPopupFeatures& popupFeatures,
	CefWindowInfo& windowInfo,
	CefRefPtr<CefClient>& client,
	CefBrowserSettings& settings,
	bool* no_javascript_access) 
{
	CEF_REQUIRE_IO_THREAD(); 
	qDebug() << __FUNCTION__;
	// redirect all popup page into the source frame forcefully
	frame->LoadURL(target_url);

	// don't allow new window or tab 
	return true;
}

void QCefViewBrowserHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();
	
	if (!message_router_)
	{
		// Create the browser-side router for query handling.
		CefMessageRouterConfig config;
		config.js_query_function = QCEF_QUERY_NAME;
		config.js_cancel_function = QCEF_QUERY_CANCEL_NAME;
		message_router_ = CefMessageRouterBrowserSide::Create(config);

		// Register handlers with the router.
		message_router_->AddHandler(cefquery_handler_, false);
	}

	if (!GetBrowser())
	{
		std::lock_guard<std::mutex> lock_scope(mtx_);
		// We need to keep the main child window, but not popup windows
		main_browser_ = browser;
		main_browser_id_ = browser->GetIdentifier();

		if (hostWidget_)
		{
			QRect rc = hostWidget_->rect();
			::MoveWindow(browser->GetHost()->GetWindowHandle(),
				rc.left(), rc.top(), rc.width(), rc.height(), TRUE);
		}
		qDebug() << __FUNCTION__ << "get browser handler sucess !";
	}
	else if (browser->IsPopup())
	{
		// Add to the list of popup browsers.
		popup_browser_list_.push_back(browser);

		// Give focus to the popup browser. Perform asynchronously because the
		// parent window may attempt to keep focus after launching the popup.
		CefPostTask(TID_UI,
			NewCefRunnableMethod(browser->GetHost().get(), &CefBrowserHost::SetFocus, true));
	}
	browser_count_++;
}

bool QCefViewBrowserHandler::DoClose(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();

	// Closing the main window requires special handling. See the DoClose()
	// documentation in the CEF header for a detailed description of this
	// process.
	if (GetBrowserId() == browser->GetIdentifier())
	{
		// Set a flag to indicate that the window close should be allowed.
		is_closing_ = true;
	}

	// Allow the close. For windowed browsers this will result in the OS close
	// event being sent.
	return false;
}

void QCefViewBrowserHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();

	message_router_->OnBeforeClose(browser);

	if (GetBrowserId() == browser->GetIdentifier())
	{
		// if the main browser is closing, we need to close all the pop up browsers.
		CloseAllPopupBrowsers(true);

		{
			std::lock_guard<std::mutex> lock_scope(mtx_);
			// Free the browser pointer so that the browser can be destroyed
			main_browser_ = NULL;
		}
	}
	else if (browser->IsPopup())
	{
		// Remove from the browser popup list.
		BrowserList::iterator it;
		for (it = popup_browser_list_.begin();
		it != popup_browser_list_.end();
			++it)
		{
			if ((*it)->IsSame(browser))
			{
				popup_browser_list_.erase(it);
				break;
			}
		}
	}

	if (--browser_count_ == 0)
	{
		message_router_->RemoveHandler(cefquery_handler_);
		delete cefquery_handler_;
		cefquery_handler_ = NULL;
		message_router_ = NULL;
	}
}

void QCefViewBrowserHandler::OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
	bool isLoading,
	bool canGoBack,
	bool canGoForward)
{
	CEF_REQUIRE_UI_THREAD();
	if (hostWidget_)
	{
		//hostWidget_->onLoadStateChange(isLoading);
		QMetaObject::invokeMethod(hostWidget_, "onLoadStateChange", Qt::QueuedConnection, Q_ARG(bool, isLoading));
	}
}

void QCefViewBrowserHandler::OnLoadStart(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame)
{
	CEF_REQUIRE_UI_THREAD();
	if (hostWidget_)
	{
		//hostWidget_->onLoadStart();
		QMetaObject::invokeMethod(hostWidget_, "onLoadStart", Qt::QueuedConnection);
	}
}

void QCefViewBrowserHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	int httpStatusCode)
{
	CEF_REQUIRE_UI_THREAD();
	if (hostWidget_)
	{
		qDebug() << __FUNCTION__ << QString::fromStdString(frame->GetURL().ToString());
		//hostWidget_->onLoadEnd(httpStatusCode);
		QMetaObject::invokeMethod(hostWidget_, "onLoadEnd", Qt::QueuedConnection, Q_ARG(int, httpStatusCode));
	}
}

void QCefViewBrowserHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	ErrorCode errorCode,
	const CefString& errorText,
	const CefString& failedUrl)
{
	CEF_REQUIRE_UI_THREAD();
	if (errorCode == ERR_ABORTED)
	{
		return;
	}

	QString msg = QString::fromStdString(errorText.ToString());
	QString url = QString::fromStdString(failedUrl.ToString());
	QString content = QString(
		"<html><body bgcolor=\"white\">"
		"<h2>Failed to load URL: %1 </h2>"
		"<h2>Error: %2(%3)</h2>"
		"</body></html>")
		.arg(url)
		.arg(msg).
		arg(errorCode);

	frame->LoadString(content.toLatin1().data(), failedUrl);
	if (hostWidget_)
	{
		//hostWidget_->onLoadError(errorCode, msg, url);
		QMetaObject::invokeMethod(hostWidget_, "onLoadError", Qt::QueuedConnection, Q_ARG(int, errorCode), Q_ARG(const QString&, msg), Q_ARG(const QString&, url));
	}
}

bool QCefViewBrowserHandler::OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefRequest> request,
	bool is_redirect)
{
	CEF_REQUIRE_UI_THREAD();

	message_router_->OnBeforeBrowse(browser, frame);
	return false;
}

bool QCefViewBrowserHandler::OnOpenURLFromTab(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	const CefString& target_url,
	CefRequestHandler::WindowOpenDisposition target_disposition,
	bool user_gesture)
{
	CEF_REQUIRE_UI_THREAD();

	return true;	// return true to cancel this navigation.
}

CefRefPtr<CefResourceHandler> QCefViewBrowserHandler::GetResourceHandler(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefRequest> request)
{
	CEF_REQUIRE_IO_THREAD();

	return NULL;
}

bool QCefViewBrowserHandler::OnQuotaRequest(CefRefPtr<CefBrowser> browser,
	const CefString& origin_url,
	int64 new_size,
	CefRefPtr<CefRequestCallback> callback)
{
	CEF_REQUIRE_IO_THREAD();

	return true;
}

void QCefViewBrowserHandler::OnProtocolExecution(CefRefPtr<CefBrowser> browser,
	const CefString& url,
	bool& allow_os_execution)
{
	CEF_REQUIRE_UI_THREAD();

}

void QCefViewBrowserHandler::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
	TerminationStatus status)
{
	CEF_REQUIRE_UI_THREAD();

	browser->GetMainFrame()->LoadURL(browser->GetMainFrame()->GetURL());

	message_router_->OnRenderProcessTerminated(browser);
}

void QCefViewBrowserHandler::setHostWidget(QCefView* host)
{
	hostWidget_ = host;
}

CefRefPtr<CefBrowser> QCefViewBrowserHandler::GetBrowser() const
{
	std::lock_guard<std::mutex> lock_scope(mtx_);
	return main_browser_;
}

int QCefViewBrowserHandler::GetBrowserId() const
{
	std::lock_guard<std::mutex> lock_scope(mtx_);
	return main_browser_id_;
}

void QCefViewBrowserHandler::CloseAllBrowsers(bool force_close)
{
	if (!CefCurrentlyOn(TID_UI))
	{
		// Execute on the UI thread.
		CefPostTask(TID_UI,
			NewCefRunnableMethod(this, &QCefViewBrowserHandler::CloseAllBrowsers, force_close));
		return;
	}

	CloseAllPopupBrowsers(force_close);

	if (main_browser_.get())
	{
		// Request that the main browser close.
		main_browser_->GetHost()->CloseBrowser(force_close);
	}
}

void QCefViewBrowserHandler::CloseAllPopupBrowsers(bool force_close)
{
	if (!CefCurrentlyOn(TID_UI))
	{
		// Execute on the UI thread.
		CefPostTask(TID_UI,
			NewCefRunnableMethod(this, &QCefViewBrowserHandler::CloseAllPopupBrowsers, force_close));
		return;
	}

	if (!popup_browser_list_.empty())
	{
		BrowserList::const_iterator it;
		for (it = popup_browser_list_.begin();
		it != popup_browser_list_.end();
			++it)
		{
			(*it)->GetHost()->CloseBrowser(force_close);
		}
	}
}

bool QCefViewBrowserHandler::IsClosing() const
{
	return is_closing_;
}

bool QCefViewBrowserHandler::TriggerEvent(const CefRefPtr<CefProcessMessage> msg)
{
	if (msg->GetName().empty())
	{
		return false;
	}

	CefRefPtr<CefBrowser> browser = GetBrowser();
	if (browser)
	{
		return browser->SendProcessMessage(PID_RENDERER, msg);
	}

	return false;
}

bool QCefViewBrowserHandler::ResponseQuery(int64_t query, 
	bool success, const CefString& response, int error)
{
	if (cefquery_handler_)
	{
		return cefquery_handler_->Response(query, success, response, error);
	}
	return false;
}

bool QCefViewBrowserHandler::DispatchNotifyRequest(CefRefPtr<CefBrowser> browser,
	CefProcessId source_process,
	CefRefPtr<CefProcessMessage> message)
{
	qDebug() << __FUNCTION__ << source_process << QString::fromStdString(message->GetName().ToString());
	if (hostWidget_ && message->GetName() == INVOKEMETHOD_NOTIFY_MESSAGE)
	{
		CefRefPtr<CefListValue> messageArguments = message->GetArgumentList();
		if (messageArguments && (messageArguments->GetSize() >= 2))
		{
			int idx = 0;
			if (CefValueType::VTYPE_INT == messageArguments->GetType(idx))
			{
				int browserId = browser->GetIdentifier();
				int frameId = messageArguments->GetInt(idx++);

				if (CefValueType::VTYPE_STRING == messageArguments->GetType(idx))
				{
					CefString functionName = messageArguments->GetString(idx++);
					if (functionName == QCEF_INVOKEMETHOD)
					{
						QString method;
						if (CefValueType::VTYPE_STRING == messageArguments->GetType(idx))
						{
							#if defined(CEF_STRING_TYPE_UTF16)
							method = QString::fromWCharArray(messageArguments->GetString(idx++).c_str());
							#elif defined(CEF_STRING_TYPE_UTF8)
							method = QString::fromUtf8(messageArguments->GetString(idx++).c_str());
							#elif defined(CEF_STRING_TYPE_WIDE)
							method = QString::fromWCharArray(messageArguments->GetString(idx++).c_str());
							#endif
						}

						QVariantList arguments;
						QString qStr;
						for (idx; idx < messageArguments->GetSize(); idx++)
						{
							if (CefValueType::VTYPE_BOOL == messageArguments->GetType(idx))
							{
								arguments.push_back(QVariant::fromValue(
									messageArguments->GetBool(idx)));
							}
							else if (CefValueType::VTYPE_INT == messageArguments->GetType(idx))
							{
								arguments.push_back(QVariant::fromValue(
									messageArguments->GetInt(idx)));
							}
							else if (CefValueType::VTYPE_DOUBLE == messageArguments->GetType(idx))
							{
								arguments.push_back(QVariant::fromValue(
									messageArguments->GetDouble(idx)));
							}
							else if (CefValueType::VTYPE_STRING == messageArguments->GetType(idx))
							{
								#if defined(CEF_STRING_TYPE_UTF16)
								qStr = QString::fromWCharArray(messageArguments->GetString(idx).c_str());
								#elif defined(CEF_STRING_TYPE_UTF8)
								qStr = QString::fromUtf8(messageArguments->GetString(idx).c_str());
								#elif defined(CEF_STRING_TYPE_WIDE)
								qStr = QString::fromWCharArray(messageArguments->GetString(idx).c_str());
								#endif
								arguments.push_back(qStr);
							}
							else if (CefValueType::VTYPE_NULL == messageArguments->GetType(idx))
							{
								arguments.push_back(0);
							}
							else
							{
								// do log
								__noop(_T("QCefView"), _T("Unknow Type!"));
							}
						}

						QMetaObject::invokeMethod(hostWidget_,
							"onInvokeMethodNotify",
							Qt::QueuedConnection,
							Q_ARG(const QString, method),
							Q_ARG(const QVariantList, arguments));

						return true;
						}
					}
				}
		}
	}

	return false;
}

void QCefViewBrowserHandler::OnFullscreenModeChange(CefRefPtr<CefBrowser> browser, bool fullscreen)
{
	qDebug() << __FUNCTION__;
	HWND hwndBrowser = browser->GetHost()->GetWindowHandle();

	ShowWindow(browser->GetHost()->GetWindowHandle(), fullscreen ? SW_MAXIMIZE : SW_RESTORE);
}

void QCefViewBrowserHandler::OnStatusMessage(CefRefPtr<CefBrowser> browser, const CefString& value)
{
	qDebug() << __FUNCTION__ << QString::fromStdString(value.ToString());
	//throw std::logic_error("The method or operation is not implemented.");
}

bool QCefViewBrowserHandler::OnKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent& event, CefEventHandle os_event)
{
	qDebug() << __FUNCTION__;
	return true;
	//throw std::logic_error("The method or operation is not implemented.");
}
 
bool QCefViewBrowserHandler::OnRequestGeolocationPermission(CefRefPtr<CefBrowser> browser, const CefString& requesting_url, int request_id, CefRefPtr<CefGeolocationCallback> callback)
{
	qDebug() << __FUNCTION__;
	return true;
	//throw std::logic_error("The method or operation is not implemented.");
}

void QCefViewBrowserHandler::OnCancelGeolocationPermission(CefRefPtr<CefBrowser> browser, int request_id)
{
	//throw std::logic_error("The method or operation is not implemented.");
	qDebug() << __FUNCTION__;
}

void QCefViewBrowserHandler::OnDialogClosed(CefRefPtr<CefBrowser> browser)
{
	//throw std::logic_error("The method or operation is not implemented.");
	qDebug() << __FUNCTION__;
}

CefRequestHandler::ReturnValue QCefViewBrowserHandler::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefRequestCallback> callback)
{
	//throw std::logic_error("The method or operation is not implemented.");
	//qDebug() << __FUNCTION__;
	return RV_CONTINUE;
}

//bool QCefViewBrowserHandler::OnResourceResponse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefResponse> response)
//{
//	//throw std::logic_error("The method or operation is not implemented.");
//	qDebug() << __FUNCTION__;
//	return true;
//}

void QCefViewBrowserHandler::OnResourceLoadComplete(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefResponse> response, URLRequestStatus status, int64 received_content_length)
{
	CefRequest::HeaderMap headMaps;

	//throw std::logic_error("The method or operation is not implemented.");
	//qDebug() << __FUNCTION__ << QString::fromStdString(request->GetURL().ToString()) << received_content_length;
}
