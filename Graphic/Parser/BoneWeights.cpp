#include <vector>
#include "BoneWeights.h"

BoneWeights::BoneWeights()
	:sumWeight(0.0f)
{

}

BoneWeights::~BoneWeights()
{
	m_BoneWeights.clear();
}

void BoneWeights::AddBoneWeight(int boneIndex, float boneWeight)
{
	if (boneWeight <= 0.0f)
		return;

	sumWeight += boneWeight;

	bool isAdded = false;
	std::vector<std::pair<int, float>>::iterator it;
	for (it = m_BoneWeights.begin(); it != m_BoneWeights.end(); it++)
	{
		if (boneWeight > it->second)
		{
			m_BoneWeights.insert(it, std::make_pair(boneIndex, boneWeight));
			isAdded = true;
			break;
		}
	}

	if (isAdded == false)
		m_BoneWeights.push_back(std::make_pair(boneIndex, boneWeight));
}

void BoneWeights::AddBoneWeight(std::pair<int, float> boneWeightPair)
{
	AddBoneWeight(boneWeightPair.first, boneWeightPair.second);
}

void BoneWeights::AddBoneWeights(const BoneWeights& boneWeights)
{
	for (size_t i = 0; i < boneWeights.m_BoneWeights.size(); ++i)
	{
		AddBoneWeight(boneWeights.m_BoneWeights[i]);
	}
}

void BoneWeights::Validate()
{
	sumWeight = 0.0f;
	int i = 0;

	std::vector<std::pair<int, float>>::iterator it = m_BoneWeights.begin();
	while (it != m_BoneWeights.end())
	{
		sumWeight += it->second;
		++it;
	}
}

void BoneWeights::Normalize()
{
	Validate();

	float fScale = 1.0f / sumWeight;
	std::vector<std::pair<int, float>>::iterator it = m_BoneWeights.begin();

	while (it != m_BoneWeights.end())
	{
		it->second *= fScale;
		++it;
	}
}
