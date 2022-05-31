#pragma once

/// FBXParser를 통해서 나온 Data를 다시 DX11에 맞게끔 만들어서 돌려주는 클래스

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include <queue>

struct SkeletonData;
struct AnimationClipData;
struct FBXModelBinaryFile;
class ModelMesh;
class NaviMeshSet;
class CenterCreator;

class DX11MeshFactory
{
public:
	DX11MeshFactory();
	~DX11MeshFactory();

	// FBX로부터 생성된 bin 파일에서 modelMesh와 material을 뽑아낸다
	// 현재 Device를 스마트 포인터로 감쌀 수가 없다 그래서 원시 포인터를 매번 전달한다
	void GetModelMeshBinary(
		ID3D11Device* pDevice,
		std::shared_ptr<FBXModelBinaryFile> inputFile,
		std::shared_ptr<ModelMesh> outModel,
		std::vector<std::shared_ptr<MaterialDesc>>& outMaterial_V);

	void GetAnimationData(
		std::shared_ptr<AnimFileForRead> inputFile,
		std::shared_ptr<AnimationClipData> outAnimationData);

	void GetModelMeshBinaryAsNeviMesh(
		ID3D11Device* pDevice,
		std::shared_ptr<FBXModelBinaryFile> inputFile,
		std::shared_ptr<NaviMeshSet>& outNeviMesh);


	std::shared_ptr<CenterCreator> m_CenterCreator;
};

