#pragma once
/// <summary>
/// Object Interface 
/// </summary>
/// 

__interface IObject
{
public:
	virtual void Update(float dTime)abstract;
	virtual void Release()abstract;
   virtual void Reset() abstract;

};

