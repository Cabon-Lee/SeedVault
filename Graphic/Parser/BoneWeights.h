#pragma once
//#include <vector>
class BoneWeights
{
public:
	BoneWeights();
	~BoneWeights();

public:
	void AddBoneWeight(int boneIndex, float boneWeight);
	void AddBoneWeight(std::pair<int, float> boneWeightPair);
	void AddBoneWeights(const BoneWeights& boneWeights);

	void Validate();
	void Normalize();

public:
	float sumWeight;
	std::vector<std::pair<int, float>> m_BoneWeights;
};

