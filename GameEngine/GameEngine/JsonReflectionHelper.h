#pragma once
#include <type_traits>
#include <json/json.h>

#include <boost/describe.hpp>
#include <boost/mp11.hpp>

namespace save
{
	//포인터 생성자
	template<typename T>
	void newAuto1(T* t)
	{
		t = new T();
	}

	//2중포인터 생성자
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
		class Md = boost::describe::describe_members<Data, boost::describe::mod_any_access | boost::describe::mod_inherited>>	//상속 변수도 접근할 수 있다.
		void WriteValue(Json::Value* value, std::string key, Data const& data)
	{

		boost::mp11::mp_for_each<Bd>([&](auto D)
			{
				using B = typename decltype(D)::type;
			});

		/// <summary>
		/// 벡터 사이즈(변수끝에 '_S'가 붙은)를 미리 보관한다.
		/// </summary>
		std::map<std::string, unsigned int> _vectorSize;
		boost::mp11::mp_for_each<Md>([&](auto D)
			{
				///map에 백터의 사이즈 값만 추려서 넣는다.
				auto k = data.*D.pointer;

				if constexpr (std::is_unsigned<decltype(k)>())
				{
					std::string _variable = D.name;
					_variable.substr(2, _variable.size());

					///변수끝에 _S가 붙어있는지 확인
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
				/// 난해한 템블릿 메타프로그래밍 대신 템플릿 분기를 태우는 용도로 사용했다. 
				/// 템플릿이 인스턴스화 되면서 생성되는 코드에 컴파일이 불가능한 부분 발생
				/// 해당 조건 false라면 아예 컴파일되지 않는다.
				/// </summary>
				/// 1차 배열, std::vector (_S : size 데이터 넘겨줘야함)지원합니다.
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
					/// 구조체, 클래스 포인터인 경우
					/// </summary>
					if constexpr (std::is_class<decltype(j)>())
					{
						//없어져도 좋기 때문에 지역변수로
						Json::Value _classValue;
						/// <summary>
						/// Bd의 템플릿 인수를 추론할 수 없다고 나오는 경우
						/// BOOST_DESCRIBE를 해당 클래스의 namespace안에서 선언해주지 않았거나 BOOST에 등록된 적 없는 클래스이다.
						/// </summary>

						save::WriteValue(&_classValue, typeid(j).name(), *(data.*D.pointer));
						(*value)[key.c_str()][D.name] = _classValue;
					}
					else
					{
						///변수끝에 _V가 붙어있는지 확인
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
							//배열
							Json::Value _array;
							for (int i = 0; i < (sizeof(data.*D.pointer) / sizeof(*k)); i++)
							{
								_array.append(*(k + i));
							}

							(*value)[key.c_str()][D.name] = _array;
						}
					}
				}
					//std::string도 일로 들어온다 쇼킹
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
		//json 확인 코드
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

					//오류를 막기 위함입니다. 앵간하면 안들어오게 하쇼
					if (data.*D.pointer == nullptr)
					{
						save::newAuto1(data.*D.pointer);
						Trace("save::ReadValue %s = nullptr" , typeid(k).name());
					}

					auto j = *k;

					if constexpr (std::is_class<decltype(j)>())
					{
						/// <summary>
						///auto&가 아니어서 j값은 그저 복사본이다
						/// </summary>
						Json::Value _classValue = (*value)[key.c_str()][D.name];
						save::ReadValue(&_classValue, typeid(j).name(), *(data.*D.pointer));
					}
					else
					{
						///변수끝에 _V가 붙어있는지 확인
						//if constexpr((sizeof(data.*D.pointer) / sizeof(*k)<=1))
						if (_variable.find("_V") != std::string::npos)
						{
							//////Vector
							//if (data.*D.pointer != nullptr)
							//{
							//	//	// 여기선가 어떻게 그 타입에 맞게 (또는 모르고서) 메모리 할당을 해 줘야 한다.
							//	//	// 벡터는 크기를 정적으로 모르므로 json에서 가져와서 뭔가 해 줘야 해..
							//	//	//data.*D.pointer =static_cast<decltype(k)>(calloc((*value)[key.c_str()][D.name].size(), sizeof(decltype(j))));
							//	//
							//	//	Json::Value _vectorValue = (*value)[key.c_str()][D.name];
							//	//	save::ReadVectorValue(&_vectorValue, data.*D.pointer);
							//	//	//std::vector<decltype(j)>* _tempVector = new std::vector<decltype(j)>;
							//	for (int i = 0; i < (*value)[key.c_str()][D.name].size(); i++)
							//	{
							//		/// <summary>
							//		/// 위험해보이는 코드... 괜찮을까?
							//		/// 괜찮지 않다고 했다. 
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
				//std::string도 일로 들어온다 쇼킹
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
				//객체면 new로 할당해서 넣어야하고
				T* temp = new T();
				save::ReadValue(value, typeid(T).name(), *temp);
				vector.push_back(*temp);
			}
			else
			{
				// 원시 값이면 그냥 쑤셔넣기
				vector.push_back((*value)[i].as<T>());
			}

		}
	}

}
