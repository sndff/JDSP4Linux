#include "ConfigContainer.h"

#include <QDebug>
#include <utility>

#include "utils/Log.h"

ConfigContainer::ConfigContainer() = default;

void ConfigContainer::setValue(const QString &key,
                               QVariant       value)
{
	map[key] = std::move(value);
}

QVariant ConfigContainer::getVariant(const QString &key,
                                     bool           silent,
                                     bool*          exists)
{
    if(exists != NULL){
        *exists = map.contains(key);
    }

    if (!map.contains(key))
	{
		if (!silent)
		{
            Log::debug(QString("ConfigContainer::getVariant: Key '%1' not found").arg(key));
		}

        return QVariant();
	}

	return map.find(key).value();
}

QString ConfigContainer::getString(const QString &key,
                                   bool           setToDefaultIfMissing,
                                   const QString &defaults)
{
	if (!map.contains(key))
	{
        Log::debug(QString("ConfigContainer::getString: Key '%1' not found").arg(key));

		if (setToDefaultIfMissing)
		{
			map[key] = defaults;
		}

		return defaults;
	}

	return getVariant(key).toString();
}

int ConfigContainer::getInt(const QString &key,
                            bool           setToDefaultIfMissing,
                            int            defaults)
{
	if (!map.contains(key))
	{
        Log::debug(QString("ConfigContainer::getInt: Key '%1' not found").arg(key));

		if (setToDefaultIfMissing)
		{
			map[key] = defaults;
		}

		return defaults;
	}

	return getVariant(key).toInt();
}

float ConfigContainer::getFloat(const QString &key,
                                bool           setToDefaultIfMissing,
                                float          defaults)
{
	if (!map.contains(key))
	{
        Log::debug(QString("ConfigContainer::getFloat: Key '%1' not found").arg(key));

		if (setToDefaultIfMissing)
		{
			map[key] = defaults;
		}

		return defaults;
	}

	return getVariant(key).toFloat();
}

bool ConfigContainer::getBool(const QString &key,
                              bool           setToDefaultIfMissing,
                              bool           defaults)
{
	if (!map.contains(key))
	{
        Log::debug(QString("ConfigContainer::getBool: Key '%1' not found").arg(key));

		if (setToDefaultIfMissing)
		{
			map[key] = defaults;
		}

		return defaults;
	}

	return getVariant(key).toBool();
}

QVariantMap ConfigContainer::getConfigMap()
{
	return map;
}

void ConfigContainer::setConfigMap(const QVariantMap &newmap)
{
	map.clear();

	for (const auto &e : newmap.keys())
	{
		map[e] = newmap.value(e);
	}
}
