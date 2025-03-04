#include "SingleInstanceMonitor.h"

#include "Log.h"
#include "dbus/ServerAdaptor.h"
#include "dbus/ClientProxy.h"

SingleInstanceMonitor::SingleInstanceMonitor(QObject* parent) : QObject(parent)
{
    QDBusConnection connection = QDBusConnection::sessionBus();

    _dbusAdapter = new GuiAdaptor(this);
    _registered  = connection.registerObject("/Gui", this) &&
            connection.registerService("me.timschneeberger.jdsp4linux.Gui");

}

SingleInstanceMonitor::~SingleInstanceMonitor()
{
    _dbusAdapter->deleteLater();
}

bool SingleInstanceMonitor::isServiceReady()
{
    if (_registered)
    {
        Log::information("SingleInstanceMonitor::isServiceReady: Service registration successful");
        return true;
    }
    else
    {
        Log::warning("SingleInstanceMonitor::isServiceReady: Service registration failed. Name already aquired by other instance");
        return false;
    }
}

bool SingleInstanceMonitor::handover()
{
    Log::information("SingleInstanceMonitor::handover: Attempting to switch to this instance...");
    auto m_dbInterface = new cf::thebone::jdsp4linux::Gui("me.timschneeberger.jdsp4linux.Gui", "/Gui",
                                                          QDBusConnection::sessionBus(), this);
    if (!m_dbInterface->isValid())
    {
        Log::error("SingleInstanceMonitor::handover: Unable to connect to other DBus instance. Continuing anyway...");
    }
    else
    {
        QDBusPendingReply<> msg = m_dbInterface->raiseWindow();

        if (msg.isError() || msg.isValid())
        {
            Log::error("SingleInstanceMonitor::handover: Other instance returned (invalid) error message. Continuing anyway...");
        }
        else
        {
            Log::information("SingleInstanceMonitor::handover: Success! Waiting for event loop to exit...");
            QTimer::singleShot(0, qApp, &QCoreApplication::quit);
            return true;
        }
    }

    return false;
}
