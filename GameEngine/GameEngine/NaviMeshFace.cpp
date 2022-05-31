#include "pch.h"
#include "NaviMeshFace.h"

unsigned int NaviMeshSet::GetIndexSize()
{
	return m_NeviMeshFace_V.size();
}

std::shared_ptr<NaviMeshFace> NaviMeshSet::GetNeviMeshFace(unsigned int idx)
{
	return m_NeviMeshFace_V.at(idx);
}
