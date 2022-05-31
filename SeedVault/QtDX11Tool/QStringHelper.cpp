#include "pch.h"
#include "QStringHelper.h"


//당분간 봉인
//const char* QStringToConstCharPtr(const QString& str)
//{
//	QByteArray arr = str.toUtf8();
//
//	/// <summary>
//	/// 여기서 생성한 char들은 어쩐담;
//	/// </summary>
//	char* buffer = new char(200);
//	strcpy(buffer, arr.constData());
//	return buffer;
//}

void QStringToConstCharPtr(const QString& str, char* dest)
{
	QByteArray arr = str.toUtf8();

	unsigned int i = 0;
	bool _isRun = true;
	while (_isRun == true)
	{
		auto _nowChar = arr.constData()[i];
		if (_nowChar == '\0')
		{
		
			_isRun = false;
			dest[i] = '\0';
		}
			

		dest[i] = _nowChar;
		i++;
	}
}

const std::string QStringToString(const QString& str)
{
	QByteArray arr = str.toUtf8();
	return std::string(arr.constData());
}

const QString ConstCharPtrToQString(const char* str)
{
	return QString(str);
}

const QString StringToQString(const std::string& str)
{
	return QString(str.c_str());
}
