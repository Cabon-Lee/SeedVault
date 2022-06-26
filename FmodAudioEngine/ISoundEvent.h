#pragma once
#include <string>

using PATH = std::string;

__interface ISoundEvent
{
public:
	virtual bool IsValid() abstract;
	virtual bool Is3D() const abstract;
};