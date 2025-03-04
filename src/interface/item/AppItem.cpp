#include "AppItem.h"
#include "ui_AppItem.h"

#include "data/model/AppItemModel.h"
#include "config/AppConfig.h"
#include "utils/Log.h"

AppItem::AppItem(AppItemModel* _model, int id, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AppItem),
    id(id)
{
    ui->setupUi(this);

    // Dummy mode
    if(id == -1)
    {
        return;
    }

    model = _model;

#ifdef USE_PULSEAUDIO
    ui->blocklist->hide();
#endif

    auto node = model->findByNodeId(id);
    if(!node.has_value())
    {
        Log::warning("AppItem::ctor: AppItemModel::findByNodeId retuned nullopt");
        return;
    }

    refresh(node.value());

    ui->blocklist->setText(AppConfig::instance().get<bool>(AppConfig::AudioAppBlocklistInvert) ?
                           "Add to allowlist" : "Add to blocklist");

    connect(&AppConfig::instance(), &AppConfig::updated, this, &AppItem::onAppConfigUpdated);
    connect(model, &AppItemModel::appChanged, this, &AppItem::refresh);
    connect(ui->blocklist, &QCheckBox::toggled, this, &AppItem::setBlocked);
}

AppItem::~AppItem()
{
    if(model != nullptr) // TODO: fix occasional sigsegv on exit
        disconnect(model, &AppItemModel::appChanged, this, &AppItem::refresh);
    disconnect(&AppConfig::instance(), &AppConfig::updated, this, &AppItem::onAppConfigUpdated);
    delete ui;
}

void AppItem::onAppConfigUpdated(const AppConfig::Key& key, const QVariant& value)
{
    switch (key) {
    case AppConfig::AudioAppBlocklistInvert: {
        ui->blocklist->setText(value.toBool() ? "Add to allowlist" : "Add to blocklist");
        auto node = model->findByNodeId(id);
        if(!node.has_value())
        {
            Log::warning("AppItem::onAppConfigUpdated: AppItemModel::findByNodeId retuned nullopt");
            return;
        }

        refresh(node.value());
        break;
    }
    default:
        break;
    }
}

void AppItem::refresh(const AppNode& node)
{
    if(node.id != id)
    {
        return;
    }

    ui->name->setText(node.name);
    ui->rate->setText("Rate: " + QString::number(node.rate) + "Hz");
    ui->latency->setText("Latency: " + QString::number(node.latency * 1000, 'f', 2) + "ms");
    ui->format->setText("Format: " + node.format);
    ui->state->setText(node.state);
    ui->icon->setPixmap(QIcon::fromTheme(node.app_icon_name, QIcon(":/icons/Procedure_16x.svg")).pixmap(QSize(16, 16)));

    auto list = AppConfig::instance().get<QStringList>(AppConfig::AudioAppBlocklist);
    ui->blocklist->setChecked(list.contains(node.name));
}

void AppItem::setBlocked(bool blocked)
{
    auto node = model->findByNodeId(id);
    if(!node.has_value())
    {
        Log::error("AppItem::setBlocked: AppItemModel::findByNodeId retuned nullopt");
        return;
    }

    Log::debug("AppItem::setBlocked: '" + node.value().name + "' -> " + (blocked ? "true" : "false"));

    auto list = AppConfig::instance().get<QStringList>(AppConfig::AudioAppBlocklist);
    if(blocked)
    {
        if(!list.contains(node.value().name))
        {
            list.append(node.value().name);
        }
        else
        {
            Log::debug("AppItem::setBlocked: Already in list, ignoring");
        }
    }
    else if(list.contains(node.value().name))
    {
        list.removeAll(node.value().name);
    }
    else
    {
        Log::debug("AppItem::setBlocked: Not in list, ignoring");
    }

    AppConfig::instance().set(AppConfig::AudioAppBlocklist, list);
}
