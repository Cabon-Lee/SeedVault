#pragma once
/// <summary>
/// Component Interface
/// 다른 형태의 컴포넌트가 생겼을때 대응하기 위함
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


