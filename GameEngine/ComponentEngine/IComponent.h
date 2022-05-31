#pragma once
/// <summary>
/// Component Interface
/// �ٸ� ������ ������Ʈ�� �������� �����ϱ� ����
/// </summary>
class IComponent
{
public:
	IComponent() {};
	virtual ~IComponent() {};
public:
	virtual void Start() abstract;
	virtual void Update(float dTime) abstract;

	virtual void Remove() abstract;
};


