#pragma once
//싱글톤으로 만들 클래스가 상속해야할 부모 클래스

template<class T>
class Singletone
{
public:
	static T* GetInstance()
	{
		if (m_Instance == nullptr)
		{
			m_Instance = new T();
		}
		return m_Instance;
	}

	static void ReleaseInstance()
	{
		if (m_Instance == nullptr)
		{
			return;
		}

		delete m_Instance;
		m_Instance = nullptr;
	}

protected:
	//인스턴스는 하나만 존재해야하므로 static을 사용한다.
	static T* m_Instance;
};

//static 변수는 객체 생성전에도 존재하기 때문에 전역적으로 초기화해주어야 한다.
template<class T>
T* Singletone<T>::m_Instance = nullptr;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//												사용 예시													//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//싱글톤화할 클래스
//class SingletoneClass : public Singletone<SingletoneClass>
//{	
// 
//}
// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
