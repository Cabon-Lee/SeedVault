#include "pch.h"
#include "Person.h"

Person::Person(std::string name, int age, std::string address)
{
	this->age = age;
	this->name = name;
	this->address = address;
}

void MYDLL_DECLSPEC Person::ShowInfo() const
{
	std::cout << "捞抚: " << name << std::endl;
	std::cout << "唱捞: " << age << std::endl;
	std::cout << "林家: " << address << std::endl;
}



Person* CreatePerson(char* name, int age, char* address)
{
	std::cout << "Person 按眉 积己" << std::endl;
	return new Person(name, age, address);
}

void ShowInfo(Person* obj)
{
	obj->ShowInfo();
}

void DeletePerson(Person* obj)
{
	delete obj;
	obj = nullptr;

	std::cout << "Person 按眉 秦力" << std::endl;
}