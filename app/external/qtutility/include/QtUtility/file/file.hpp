#pragma once

#include <QFileInfo>

namespace QtUtility {
namespace file {

bool create(const QFileInfo &info, const bool &override = false);
QFileInfo getUniqueFile(const QFileInfo &info);
QFileInfo createUnique(const QFileInfo &info);
void copyAndReplaceFolderContents(const QDir &from, const QDir &to);

} // namespace file
} // namespace QtUtility