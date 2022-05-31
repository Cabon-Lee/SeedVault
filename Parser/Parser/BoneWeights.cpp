#include <vector>
#include "BoneWeights.h"

BoneWeights::BoneWeights()
	:sumWeight(0.0f)
{

}

BoneWeights::~BoneWeights()
{
	m_BoneWeight_V.clear();
}

void BoneWeights::AddBoneWeight(int boneIndex, float boneWeight)
{
	if (boneWeight <= 0.0f)
		return;

	sumWeight += boneWeight;

	bool isAdded = false;
	std::vector<std::pair<int, float>>::iterator it;
	for (it = m_BoneWeight_V.begin(); it != m_BoneWeight_V.end(); it++)
	{
		if (boneWeight > it->second)
		{
			m_BoneWeight_V.insert(it, std::make_pair(boneIndex, boneWeight));
			isAdded = true;
			break;
		}
	}

	if (isAdded == false)
		m_BoneWeight_V.push_back(std::make_pair(boneIndex, boneWeight));
}

void BoneWeights::AddBoneWeight(std::pair<int, float> boneWeightPair)
{
	AddBoneWeight(boneWeightPair.first, boneWeightPair.second);
}

void BoneWeights::AddBoneWeights(const BoneWeights& boneWeights)
{
	for (size_t i = 0; i < boneWeights.m_BoneWeight_V.size(); ++i)
	{
		AddBoneWeight(boneWeights.m_BoneWeight_V[i]);
	}
}

void BoneWeights::Validate()
{
	sumWeight = 0.0f;
	int i = 0;

	std::vector<std::pair<int, float>>::iterator it = m_BoneWeight_V.begin();
	while (it != m_BoneWeight_V.end())
	{
		sumWeight += it->second;
		++it;
	}
}

void BoneWeights::Normalize()
{
	Validate();

	float fScale = 1.0f / sumWeight;
	std::vector<std::pair<int, float>>::iterator it = m_BoneWeight_V.begin();

	while (it != m_BoneWeight_V.end())
	{
		it->second *= fScale;
		++it;
	}
}
