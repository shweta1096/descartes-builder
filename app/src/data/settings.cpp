#include "data/settings.hpp"

namespace
{
    const std::map<QString, QVariant> DEFAULT_VALUES = {
        {"engine", "kedro"},
    };
}

namespace data
{
    Settings &Settings::instance()
    {
        static Settings instance;
        return instance;
    }

    Settings::Settings()
        : m_settings("CNRS@CREATE", "DesCartes Builder")
    {
        // init the default values if they don't exist
        for (auto pair : DEFAULT_VALUES)
            if (!m_settings.contains(pair.first))
                m_settings.setValue(pair.first, pair.second);
    }

    Settings::~Settings()
    {
    }

    void Settings::setValue(const QString &key, const QVariant &value)
    {
        m_settings.setValue(key, value);
    }

    QVariant Settings::value(const QString &key) const
    {
        if (DEFAULT_VALUES.count(key) > 0)
            return m_settings.value(key, DEFAULT_VALUES.at(key));
        return m_settings.value(key);
    }
}