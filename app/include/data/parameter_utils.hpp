#pragma once

#include <sstream>
#include <vector>
#include <QString>
#include <QVariant>
#include <QVariantList>

// String <-> std::vector<int>
inline QString vectorToString(const std::vector<int> &vec)
{
    QStringList strList;
    for (int v : vec) {
        strList << QString::number(v);
    }
    return "[" + strList.join(", ") + "]";
}

inline std::vector<int> stringToVector(const QString &str)
{
    QString formatted = str;
    formatted.remove('[').remove(']').remove(' ');
    std::vector<int> result;
    for (const auto &s : formatted.split(",", Qt::SkipEmptyParts)) {
        result.push_back(s.toInt());
    }
    return result;
}
