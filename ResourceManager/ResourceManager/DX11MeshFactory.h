#pragma once

/// FBXParser�� ���ؼ� ���� Data�� �ٽ� DX11�� �°Բ� ���� �����ִ� Ŭ����

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

	// FBX�κ��� ������ bin ���Ͽ��� modelMesh�� material�� �̾Ƴ���
	// ���� Device�� ����Ʈ �����ͷ� ���� ���� ���� �׷��� ���� �����͸� �Ź� �����Ѵ�
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

