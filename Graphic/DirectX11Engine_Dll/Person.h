#pragma once

#include <string>
#include <iostream>

///
/// DLL Å¬·¡½º
/// 

#ifdef DIRECTX11ENGINEDLL_EXPORTS
#define MYDLL_DECLSPEC __declspec(dllexport)
#else
#define MYDLL_DECLSPEC __declspec(dllimport)
#endif

class MYDLL_DECLSPEC Person {
public:
	Person(std::string name, int age, std::string address);
	void ShowInfo() const;

private:
	int age;
	std::string name;
	std::string address;
};

extern "C" 
{
	MYDLL_DECLSPEC Person* CreatePerson(char* name, int age, char* address);
	MYDLL_DECLSPEC void ShowInfo(Person* obj);
	MYDLL_DECLSPEC void DeletePerson(Person* obj);
}