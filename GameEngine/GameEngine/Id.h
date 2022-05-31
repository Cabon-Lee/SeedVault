#pragma once

/// <summary>
/// Debug¿ë Id System
/// </summary>
class Id
{
private:
	Id();
public:
	Id(unsigned int type, unsigned int detailType);
	Id(unsigned int type);
	virtual ~Id();

public:
	const unsigned int m_Id;

private:
	const unsigned int SetIdByType(unsigned int type, unsigned int detailType);
};

