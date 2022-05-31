#pragma once
#include <type_traits>
#include <json/json.h>

#include <boost/describe.hpp>
#include <boost/mp11.hpp>

namespace save
{
	//������ ������
	template<typename T>
	void newAuto1(T* t)
	{
		t = new T();
	}

	//2�������� ������
	template<typename T>
	void newAuto2(T** t)
	{
		*t = new T();
	}

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

	template<class Data,
		class Bd = boost::describe::describe_bases<Data, boost::describe::mod_any_access>,
		class Md = boost::describe::describe_members<Data, boost::describe::mod_any_access | boost::describe::mod_inherited>>	//��� ������ ������ �� �ִ�.
		void WriteValue(Json::Value* value, std::string key, Data const& data)
	{

		boost::mp11::mp_for_each<Bd>([&](auto D)
			{
				using B = typename decltype(D)::type;
			});

		/// <summary>
		/// ���� ������(�������� '_S'�� ����)�� �̸� �����Ѵ�.
		/// </summary>
		std::map<std::string, unsigned int> _vectorSize;
		boost::mp11::mp_for_each<Md>([&](auto D)
			{
				///map�� ������ ������ ���� �߷��� �ִ´�.
				auto k = data.*D.pointer;

				if constexpr (std::is_unsigned<decltype(k)>())
				{
					std::string _variable = D.name;
					_variable.substr(2, _variable.size());

					///�������� _S�� �پ��ִ��� Ȯ��
					if (_variable.find("_S") != std::string::npos)
					{
						_vectorSize.insert(std::make_pair(_variable, data.*D.pointer));
					}
				}
			});

		boost::mp11::mp_for_each<Md>([&](auto D)
			{
				auto k = data.*D.pointer;

				/// <summary>
				/// constexpr (C++11)
				/// ������ �ۺ� ��Ÿ���α׷��� ��� ���ø� �б⸦ �¿�� �뵵�� ����ߴ�. 
				/// ���ø��� �ν��Ͻ�ȭ �Ǹ鼭 �����Ǵ� �ڵ忡 �������� �Ұ����� �κ� �߻�
				/// �ش� ���� false��� �ƿ� �����ϵ��� �ʴ´�.
				/// </summary>
				/// 1�� �迭, std::vector (_S : size ������ �Ѱ������)�����մϴ�.
				if constexpr (std::is_pointer<decltype(k)>())
				{
					std::string _variable = D.name;
					_variable.substr(2, _variable.size());

					if (data.*D.pointer == nullptr)
					{
						save::newAuto1(data.*D.pointer);
						Trace("save::WriteValue %s = nullptr", typeid(k).name());
					}

					auto j = *k;
					/// <summary>
					/// ����ü, Ŭ���� �������� ���
					/// </summary>
					if constexpr (std::is_class<decltype(j)>())
					{
						//�������� ���� ������ ����������
						Json::Value _classValue;
						/// <summary>
						/// Bd�� ���ø� �μ��� �߷��� �� ���ٰ� ������ ���
						/// BOOST_DESCRIBE�� �ش� Ŭ������ namespace�ȿ��� ���������� �ʾҰų� BOOST�� ��ϵ� �� ���� Ŭ�����̴�.
						/// </summary>

						save::WriteValue(&_classValue, typeid(j).name(), *(data.*D.pointer));
						(*value)[key.c_str()][D.name] = _classValue;
					}
					else
					{
						///�������� _V�� �پ��ִ��� Ȯ��
						if (_variable.find("_V") != std::string::npos)
						{
							//Vector
							Json::Value _vector;

							_variable = D.name;
							_variable.replace(_variable.size() - 2, _variable.size() - 1, "_S");

							for (int i = 0; i < _vectorSize.at(_variable); i++)
							{
								_vector.append(*(k + i));
							}

							(*value)[key.c_str()][D.name] = _vector;

						}
						else
						{
							//�迭
							Json::Value _array;
							for (int i = 0; i < (sizeof(data.*D.pointer) / sizeof(*k)); i++)
							{
								_array.append(*(k + i));
							}

							(*value)[key.c_str()][D.name] = _array;
						}
					}
				}
					//std::string�� �Ϸ� ���´� ��ŷ
				//else if constexpr(std::is_class<decltype(k)>())
				//{
				//	/*Json::Value _classValue;
				//	save::WriteValue(&_classValue, typeid(k).name(), data.*D.pointer);
				//	(*value)[key.c_str()][D.name] = _classValue;*/
				//}
				else
				{
					(*value)[key.c_str()][D.name] = data.*D.pointer;
				}
			});
	}

	template<class T>
	void WriteVectorValue(Json::Value* value, std::vector<T, std::allocator<T>>& vector)
	{
		Json::Value _vectorValue;

		for (int i = 0; i < vector.size(); ++i)
		{
			if constexpr (std::is_same<T, std::string>())
			{
				_vectorValue.append(vector.at(i));
			}
			else if constexpr (std::is_pointer<T>())
			{
				Json::Value _classValue;
				save::WriteValue(&_classValue, typeid(T).name(), *(vector.at(i)));
				_vectorValue.append(_classValue);
			}
			else if constexpr (std::is_class<T>())
			{
				Json::Value _classValue;
				save::WriteValue(&_classValue, typeid(T).name(), vector.at(i));
				_vectorValue.append(_classValue);
			}
			else
			{
				_vectorValue.append(vector.at(i));
			}
		}

		(*value) = _vectorValue;
	}

	template<class Data,
		class Bd = boost::describe::describe_bases<Data, boost::describe::mod_any_access>,
		class Md = boost::describe::describe_members<Data, boost::describe::mod_any_access | boost::describe::mod_inherited>>
		void ReadValue(Json::Value* value, std::string key, Data& data)
	{
		//json Ȯ�� �ڵ�
		Json::StreamWriterBuilder _bulider;
		std::string out = Json::writeString(_bulider, *value);

		boost::mp11::mp_for_each<Bd>([&](auto D)
			{
				using B = typename decltype(D)::type;
			});

		boost::mp11::mp_for_each<Md>([&](auto D)
			{
				auto k = data.*D.pointer;

				//if constexpr (std::is_array<decltype(k)>())
				if constexpr (std::is_pointer<decltype(k)>())
				{
					std::string _variable = D.name;
					_variable.substr(2, _variable.size());

					//������ ���� �����Դϴ�. �ް��ϸ� �ȵ����� �ϼ�
					if (data.*D.pointer == nullptr)
					{
						save::newAuto1(data.*D.pointer);
						Trace("save::ReadValue %s = nullptr" , typeid(k).name());
					}

					auto j = *k;

					if constexpr (std::is_class<decltype(j)>())
					{
						/// <summary>
						///auto&�� �ƴϾ j���� ���� ���纻�̴�
						/// </summary>
						Json::Value _classValue = (*value)[key.c_str()][D.name];
						save::ReadValue(&_classValue, typeid(j).name(), *(data.*D.pointer));
					}
					else
					{
						///�������� _V�� �پ��ִ��� Ȯ��
						//if constexpr((sizeof(data.*D.pointer) / sizeof(*k)<=1))
						if (_variable.find("_V") != std::string::npos)
						{
							//////Vector
							//if (data.*D.pointer != nullptr)
							//{
							//	//	// ���⼱�� ��� �� Ÿ�Կ� �°� (�Ǵ� �𸣰�) �޸� �Ҵ��� �� ��� �Ѵ�.
							//	//	// ���ʹ� ũ�⸦ �������� �𸣹Ƿ� json���� �����ͼ� ���� �� ��� ��..
							//	//	//data.*D.pointer =static_cast<decltype(k)>(calloc((*value)[key.c_str()][D.name].size(), sizeof(decltype(j))));
							//	//
							//	//	Json::Value _vectorValue = (*value)[key.c_str()][D.name];
							//	//	save::ReadVectorValue(&_vectorValue, data.*D.pointer);
							//	//	//std::vector<decltype(j)>* _tempVector = new std::vector<decltype(j)>;
							//	for (int i = 0; i < (*value)[key.c_str()][D.name].size(); i++)
							//	{
							//		/// <summary>
							//		/// �����غ��̴� �ڵ�... ��������?
							//		/// ������ �ʴٰ� �ߴ�. 
							//		/// </summary>
							//		k[i] = (*value)[key.c_str()][D.name][i].as<decltype(j)>();
							//	}
							//}
						}
						else
						{
							//Array
							for (int i = 0; i < (sizeof(data.*D.pointer) / sizeof(*k)); i++)
							{
								*(k + i) = (*value)[key.c_str()][D.name][i].as<decltype(j)>();
							}
						}
					}
				}
				//std::string�� �Ϸ� ���´� ��ŷ
				//else if constexpr (std::is_class<decltype(k)>())
				//{
				//	/*Json::Value _classValue = (*value)[key.c_str()][D.name];
				//	save::ReadValue(&_classValue, typeid(k).name(), data.*D.pointer);*/
				//}
				else
				{
					data.*D.pointer = (*value)[key.c_str()][D.name].as<decltype(k)>();
				}
			});

		return;
	}

	template<class T>
	void ReadVectorValue(Json::Value* value, std::vector<T, std::allocator<T>>& vector)
	{
		Json::StreamWriterBuilder _bulider;
		std::string out = Json::writeString(_bulider, *value);
		std::string name = typeid(T).name();

		for (int i = 0; i < (*value).size(); ++i)
		{
			if constexpr (std::is_same<T, std::string>())
			{
				vector.push_back((*value)[i].as<T>());
			}
			else if constexpr (std::is_pointer<T>())
			{
				T* temp = new T();
				newAuto2(temp);
				save::ReadValue(value, typeid(T).name(), **temp);
				vector.push_back(*temp);
			}
			else if constexpr (std::is_class<T>())
			{
				//��ü�� new�� �Ҵ��ؼ� �־���ϰ�
				T* temp = new T();
				save::ReadValue(value, typeid(T).name(), *temp);
				vector.push_back(*temp);
			}
			else
			{
				// ���� ���̸� �׳� ���ųֱ�
				vector.push_back((*value)[i].as<T>());
			}

		}
	}

}
