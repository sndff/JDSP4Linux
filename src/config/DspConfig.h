/*
 *  This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 *  ThePBone <tim.schneeberger(at)outlook.de> (c) 2020
 */
#ifndef DSPCONFIGWRAPPER_H
#define DSPCONFIGWRAPPER_H

#include "AppConfig.h"
#include "ConfigContainer.h"
#include "ConfigIO.h"
#include "utils/Common.h"
#include "utils/Log.h"

#include <QObject>
#include <QMetaEnum>
#include <QFileSystemWatcher>

using namespace std;

class DspConfig :
	public QObject
{
	Q_OBJECT
public:
    enum Key {
        bass_enable,
        bass_maxgain,
        compression_aggressiveness,
        compression_enable,
        compression_maxatk,
        compression_maxrel,
        convolver_enable,
        convolver_file,
        convolver_optimization_mode,
        convolver_waveform_edit,
        crossfeed_bs2b_fcut,
        crossfeed_bs2b_feed,
        crossfeed_enable,
        crossfeed_mode,
        ddc_enable,
        ddc_file,
        graphiceq_enable,
        graphiceq_param,
        reverb_bassboost,
        reverb_decay,
        reverb_delay,
        reverb_enable,
        reverb_finaldry,
        reverb_finalwet,
        reverb_lfo_spin,
        reverb_lfo_wander,
        reverb_lpf_bass,
        reverb_lpf_damp,
        reverb_lpf_input,
        reverb_lpf_output,
        reverb_osf,
        reverb_reflection_amount,
        reverb_reflection_factor,
        reverb_reflection_width,
        reverb_wet,
        reverb_width,
        liveprog_enable,
        liveprog_file,
        master_enable,
        master_limrelease,
        master_limthreshold,
        master_postgain,
        stereowide_enable,
        stereowide_level,
        tone_enable,
        tone_eq,
        tone_filtertype,
        tone_interpolation,
        tube_enable,
        tube_pregain,
    };
    Q_ENUM(Key)

    enum class Type {
        String,
        Int32,
        Float,
        Boolean,
        Unknown
    };
    Q_ENUM(Type)

    static DspConfig &instance(bool watcherEnabled = false)
	{
        static DspConfig _instance(watcherEnabled);
		return _instance;
	}

	DspConfig(DspConfig const &)       = delete;
	void operator= (DspConfig const &) = delete;

    DspConfig(bool watcherEnabled = false)
	{
		_conf = new ConfigContainer();

        if(watcherEnabled)
        {
            Log::debug("DspConfig::ctor: File system watcher enabled");

            _watcher = new QFileSystemWatcher(this);
            if(!_watcher->addPath(AppConfig::instance().getDspConfPath()))
            {
                Log::warning(QString("DspConfig::ctor: Failed to register path %0 with QFileSystemWatcher").arg(AppConfig::instance().getDspConfPath()));
            }
            connect(_watcher, &QFileSystemWatcher::fileChanged, this, &DspConfig::fileChanged);
        }
	}

    void set(const Key &key,
	         const QVariant &value)
	{
        _conf->setValue(QVariant::fromValue(key).toString(), value);
	}

    void commit()
    {
        emit updated(this);
    }

	template<class T>
    T get(const Key &key, bool* exists = nullptr)
	{
        auto skey = QVariant::fromValue(key).toString();
        auto variant = _conf->getVariant(skey, true, exists);

        if constexpr (std::is_same_v<T, QVariant>) {
            return variant;
        }
        if constexpr (std::is_same_v<T, std::string>) {
            return variant.toString().toStdString();
        }
        if constexpr (std::is_same_v<T, QString>) {
            return variant.toString();
        }
        if constexpr (std::is_same_v<T, int>) {
            return variant.toInt();
        }
        if constexpr (std::is_same_v<T, float>) {
            return variant.toFloat();
        }
        if constexpr (std::is_same_v<T, bool>) {
            return variant.toBool();
        }

        Log::error("DspConfig::get<T>: Unknown type T");
        return 0;
	}

    Type type(const Key &key)
    {
        bool exists;
        auto skey = QVariant::fromValue(key).toString();
        auto variant = _conf->getVariant(skey, true, &exists);

        if(!exists)
        {
            return Type::Unknown;
        }

        auto type = variant.type();
        if(type == QVariant::Type::Int)
        {
            return Type::Int32;
        }
        if(type == QVariant::Type::Double)
        {
            return Type::Float;
        }
        if(type == (int)QMetaType::Float)
        {
            return Type::Float;
        }
        if(type == QVariant::Type::String)
        {
            return Type::String;
        }
        if(type == QVariant::Type::Bool)
        {
            return Type::Boolean;
        }

        Log::error("DspConfig::type: Unknown type");
        return Type::Unknown;
    }


	void save()
	{
		auto file = AppConfig::instance().getDspConfPath();
		ConfigIO::writeFile(file, _conf->getConfigMap());
	}

	void load()
	{
		auto map = ConfigIO::readFile(AppConfig::instance().getDspConfPath());

		if (map.count() < 1)
		{
			Log::debug("DspConfig::load: Empty config file, using defaults");
			loadDefault();
			return;
		}

		_conf->setConfigMap(map);
		emit configBuffered();
        emit updated(this);
	}

	void load(const QString &string)
	{
		auto map = ConfigIO::readString(string);

		if (map.count() < 1)
		{
			Log::debug("DspConfig::load: Empty config file, using defaults");
			loadDefault();
			return;
		}

		_conf->setConfigMap(map);
		emit configBuffered();
        emit updated(this);

        save();
	}

	void loadDefault()
	{
        QFile file(":/assets/default.conf");

		if (file.open(QIODevice::ReadOnly))
		{
			auto map = ConfigIO::readString(file.readAll());
			_conf->setConfigMap(map);
			emit configBuffered();
            emit updated(this);

            save();
		}
	}

signals:
	void configBuffered();
    void updated(DspConfig* self);
    void updatedExternally(DspConfig* self);

private slots:
    void fileChanged(const QString &path)
    {
        if(_watcher->files().contains(path))
        {
            Log::debug("DspConfig::fileChanged: Config changed");
            load();
            emit updatedExternally(this);
        }
    }

private:
	ConfigContainer *_conf;
    QFileSystemWatcher *_watcher;
};

#endif // DSPCONFIGWRAPPER_H
