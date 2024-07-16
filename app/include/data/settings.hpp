#pragma once

#include <QSettings>
#include <QString>
#include <QVariant>

namespace data
{
    class Settings
    {
    public:
        static Settings &instance();

        void setValue(const QString &key, const QVariant &value);
        QVariant value(const QString &key) const;

    private:
        Settings();
        ~Settings();
        Settings(const Settings &) = delete;
        Settings &operator=(const Settings &) = delete;

        QSettings m_settings;
    };
}