#pragma once
#include <string>

/// <summary>
/// 각자 Json 저장, 로드 코드를 짜야하는 불편함이 있는 MK1 방식 Save & Load
/// 빌더와 라이터가 하나이려면 결국 싱글턴을 사용해 이를 사용하도록 해야하나?
/// </summary>
/// 
//class Json::StreamWriterBuilder;
//class Json::StreamWriter;

namespace Json
{
	class Value;
}

class JsonSerializer
{
public:
	 JsonSerializer():m_Value(new Json::Value()) {};
	 virtual ~JsonSerializer() {};

public:
	 virtual void Save() abstract;
	 virtual void Load() abstract;

	 const Json::Value* const GetValue() { return m_Value; }
	 void SetValue(Json::Value* v) { m_Value = v; }

	 //typeid class "Class명" 출력에서 class를 제외하는 용도
	 const std::string EraseClass(const std::string& className)
	 {
		 std::string _exceptClass = className;
		 return 	_exceptClass.erase(0, 6);
	 }

protected:
	//Json::StreamWriterBuilder* m_Builder;
	//const std::unique_ptr<Json::StreamWriter> m_Writer;

	//boost::json::value* m_Value;
	Json::Value* m_Value;
	
};

