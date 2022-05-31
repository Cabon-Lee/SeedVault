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
	std::cout << "�̸�: " << name << std::endl;
	std::cout << "����: " << age << std::endl;
	std::cout << "�ּ�: " << address << std::endl;
}



Person* CreatePerson(char* name, int age, char* address)
{
	std::cout << "Person ��ü ����" << std::endl;
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

	std::cout << "Person ��ü ����" << std::endl;
}