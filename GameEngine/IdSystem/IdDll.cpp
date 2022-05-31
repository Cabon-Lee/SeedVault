#include "pch.h"
#include "IdDll.h"

#include <string>

unsigned int g_Count[10] = { 0, };

_ID_DLL const unsigned int ID::GetIdByType(unsigned int type, int detailType)
{
	//Scene 1/ Object 2/ Component 3/
	std::string _id = std::to_string(type);
	std::string _base = "00000000";			//최대 99,999,999까지 셀 수 있다. 넘어가면? 어쩌지?
	std::string _cnt = std::to_string(g_Count[type]++);

	for (int i = 0; i < _cnt.length(); i++)
	{
		_base[(int)(_base.length() - _cnt.length() + i)] = _cnt[i];
	}


	_id = _id
		+ std::to_string((detailType < 0) ? 0 : detailType)
		+ _base;

	//_id = _id
	//	+ _detail
	//	+ _cnt;

	return std::stoul(_id.c_str(), nullptr, 10);
}
