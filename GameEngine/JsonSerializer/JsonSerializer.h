#pragma once
#include <string>

/// <summary>
/// ���� Json ����, �ε� �ڵ带 ¥���ϴ� �������� �ִ� MK1 ��� Save & Load
/// ������ �����Ͱ� �ϳ��̷��� �ᱹ �̱����� ����� �̸� ����ϵ��� �ؾ��ϳ�?
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

	 //typeid class "Class��" ��¿��� class�� �����ϴ� �뵵
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

