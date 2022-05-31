#pragma once
//�̱������� ���� Ŭ������ ����ؾ��� �θ� Ŭ����

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
	//�ν��Ͻ��� �ϳ��� �����ؾ��ϹǷ� static�� ����Ѵ�.
	static T* m_Instance;
};

//static ������ ��ü ���������� �����ϱ� ������ ���������� �ʱ�ȭ���־�� �Ѵ�.
template<class T>
T* Singletone<T>::m_Instance = nullptr;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//												��� ����													//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//�̱���ȭ�� Ŭ����
//class SingletoneClass : public Singletone<SingletoneClass>
//{	
// 
//}
// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
