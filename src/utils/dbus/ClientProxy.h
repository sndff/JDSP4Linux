/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp manifest.xml -a serveradaptor -p clientproxy
 *
 * qdbusxml2cpp is Copyright (C) 2020 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef CLIENTPROXY_H
#define CLIENTPROXY_H

#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface me.timschneeberger.jdsp4linux.Gui
 */
class CfTheboneJdsp4linuxGuiInterface :
	public QDBusAbstractInterface
{
	Q_OBJECT

public:
	static inline const char* staticInterfaceName()
	{ return "me.timschneeberger.jdsp4linux.Gui"; }

public:
	CfTheboneJdsp4linuxGuiInterface(const QString &        service,
	                                const QString &        path,
	                                const QDBusConnection &connection,
	                                QObject               *parent = nullptr);

	~CfTheboneJdsp4linuxGuiInterface();

public Q_SLOTS: // METHODS
	inline QDBusPendingReply<> hide()
	{
		QList<QVariant> argumentList;
		return asyncCallWithArgumentList(QStringLiteral("hide"), argumentList);
	}

	inline QDBusPendingReply<> raiseWindow()
	{
		QList<QVariant> argumentList;
		return asyncCallWithArgumentList(QStringLiteral("raiseWindow"), argumentList);
	}

	inline QDBusPendingReply<> show()
	{
		QList<QVariant> argumentList;
		return asyncCallWithArgumentList(QStringLiteral("show"), argumentList);
	}

Q_SIGNALS: // SIGNALS
};

namespace cf
{
	namespace thebone
	{
		namespace jdsp4linux
		{
			typedef ::CfTheboneJdsp4linuxGuiInterface Gui;
		}
	}
}
#endif