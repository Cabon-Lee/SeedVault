#include "pch.h"
#include "JsonManager.h"

JsonManager::JsonManager()
	:m_WriterBuilder(new Json::StreamWriterBuilder())
	, m_Writer(m_WriterBuilder->newStreamWriter())
	, m_ReaderBuilder(new Json::CharReaderBuilder())
	, m_Reader(m_ReaderBuilder->newCharReader())
	//, m_Parser(std::make_unique<boost::json::stream_parser>())
{

}

JsonManager::~JsonManager()
{

}


bool JsonManager::Write(const Json::Value* const value, const std::string& filePath, const std::string& fileName)
{
	std::string _fullFileName = filePath + "/" + fileName + ".json";
	std::ofstream _save(_fullFileName.c_str());

	m_Writer->write(*value, &_save);

	_save.close();

	return true;
}

//bool JsonManager::Write(const boost::json::value* const value, const std::string& filePath, const std::string& fileName)
//{
//	std::string _fullFileName = filePath + "/" + fileName + ".json";
//	std::ofstream _save(_fullFileName.c_str());
//
//	boost::json::error_code _ec;
//	char _buf[4096];
//
//	m_Parser->write(_buf,_ec);
//
//	_save.close();
//
//	return true;
//}
//
//
