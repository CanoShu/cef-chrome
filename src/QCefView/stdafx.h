#include <windows.h>
#include <tchar.h>

#include "../public/Common.h"
#include <QDebug>

#if defined(_DEBUG) || defined(DEBUG)
#include <QDebug>
#define QLOG(x)	qDebug() << x
#else
#define QLOG(x)	__noop(x)
#endif

//#define QCEF_QUERY_NAME						"QCefQuery"
//#define QCEF_QUERY_CANCEL_NAME				"QCefQueryCancel"
//#define QCEF_OBJECT_NAME					"QCefClient"
//#define INVOKEMETHOD_NOTIFY_MESSAGE			"NotifyRequestMessage"
//#define TRIGGEREVENT_NOTIFY_MESSAGE			"TriggerEventMessage"

