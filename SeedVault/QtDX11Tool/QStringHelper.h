#pragma once

//const char* QStringToConstCharPtr(const QString& str);
void QStringToConstCharPtr(const QString& str, char* dest);

const std::string QStringToString(const QString& str);

const QString ConstCharPtrToQString(const char* str);

const QString StringToQString(const std::string& str);
