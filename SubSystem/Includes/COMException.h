#pragma once
#include <comdef.h>
#include <string>

#define COM_ERROR_IF_FAILED( hr, msg ) if( FAILED( hr ) ) throw COMException( hr, msg, __FILE__, __FUNCTION__, __LINE__ )

#define RESOURCE_MISSING( hr, msg ) if( E_FAIL == hr ) throw RESOURCE_MISSING_Exception( msg )
#define RESOURCE_NULLNAME( hr, msg ) if( E_INVALIDARG == hr ) throw NULLNameException( msg )

class COMException
{
public:
	COMException(HRESULT hr, const std::string& msg, const std::string& file, const std::string& function, int line);
	
	const wchar_t* what() const;

private:
	std::wstring whatmsg;
};

class RESOURCE_MISSING_Exception
{
public:
	RESOURCE_MISSING_Exception(const std::string& msg);
};

// 이름에 공백이 있다는 것을 알리기 위함
class NULLNameException
{
public:
	NULLNameException(const std::string& msg);
};