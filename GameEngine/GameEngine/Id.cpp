#include "pch.h"
#include "string"
#include "EngineDLL.h"
#include "Id.h"

Id::Id()
	:m_Id(0)
{

}

Id::Id(unsigned int type)
	: m_Id(SetIdByType(type, 0))
{


}

Id::Id(unsigned int type, unsigned int detailType)
	: m_Id(SetIdByType(type, detailType))
{
}

Id::~Id()
{

}

const unsigned int Id::SetIdByType(unsigned int type, unsigned int detailType)
{
	//Scene 1, Object 2 Component 3
	std::string _id;
	unsigned int _cnt=0;
//#ifdef _LIB_DLL
//	_cnt = g_Count;
//#endif
//	_cnt = DLLEngine::GetGlobalCnt();
//	
	_id = _id
		+ std::to_string(type)
		+ std::to_string(detailType)
		+ std::to_string(_cnt);

	
	return std::stoul(_id.c_str(), nullptr, 10);
}
