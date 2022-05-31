#pragma once
#include <typeinfo>
#include <string>

namespace Json
{
	class StreamWriterBuilder;
	class StreamWriter;
	class CharReaderBuilder;
	class CharReader;
	class Value;
}

namespace boost::json
{
	class value;
	class stream_parser;
}

class JsonManager
{
public:
	JsonManager();
	~JsonManager();

private:
	Json::StreamWriterBuilder* m_WriterBuilder;
	const std::unique_ptr<Json::StreamWriter>	m_Writer;
	Json::CharReaderBuilder* m_ReaderBuilder;
	const std::unique_ptr<Json::CharReader>		m_Reader;

	//const std::unique_ptr<boost::json::stream_parser> m_Parser;
					

public:
	bool Write(const Json::Value* const value, const std::string& filePath, const std::string& fileName);
	//bool Write(const boost::json::value* const value, const std::string& filePath, const std::string& fileName);

};

/*
template<class Data,
	class Bd = boost::describe::describe_bases<Data, boost::describe::mod_any_access>,
	class Md = boost::describe::describe_members<Data, boost::describe::mod_any_access>>
	void WriteValue(Json::Value* value, Data const& data)
{
	boost::mp11::mp_for_each<Bd>([&](auto D)
		{
			using B = typename decltype(D)::type;
		});
	boost::mp11::mp_for_each<Md>([&](auto D)
		{
			(*value)[typeid(data).name()][D.name] = data.*D.pointer;

		});
}

*/