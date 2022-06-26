#pragma once

/// ���ҽ��Ŵ����� ���� �������� �������� ������, ¥�ٺ��� �� �̻��� ������ �Ǿ���.
/// ���� ������ � ���� �׸��� Ŭ���̾�Ʈ�� ���ؼ� ������ �� �־�� �ϴµ�, �׶����� �������� �����ϴ� ���� ����.
/// ���� ��� �����Ͱ� �ε�� ���� �������� ���ҽ� �Ŵ����� � �͵��� �ε��ߴ��� ���� ������ �˷��ִ� �͵� ����

#include <unordered_map>
#include <vector>
#include <string>
#include "AnimationSystem.h"

__interface IResourceManager;


// ���� �̸��� ������ �޽���Ÿ �����͸� ä���� �� �ֵ���
struct MeshMetadata
{
	std::string meshName;
	std::vector<std::string> animationName_V;

	bool hasAnimation;
	bool isSkinned;
};


class ResourceLoader
{
public:
	ResourceLoader();
	~ResourceLoader();

	void Initialize(std::shared_ptr<IResourceManager> pResource);
	void LoadResource(std::string& name);

	std::vector<std::string>* GetNodeDataNames();
	std::vector<std::string>* GetAnimNames();
	std::vector<std::string>* GetTextureNames();
	std::vector<std::string>* GetShaderNames();
	std::vector<std::string>* GetMatNames();
	std::vector<std::string>* GetIBLNames();
	std::vector<std::string>* GetBankNames();
	std::vector<std::string>* GetQuestNames();
	std::vector<std::string>* GetDialogueNames();



//private:
	std::shared_ptr<IResourceManager> m_pResourceManager;
	 
	// ���Ǵ� ��� �ܺ� ���ҽ��� �̸����� �ΰ��� ����� �ʿ䰡 ����
	// MeshFilter, MaterialComponent���� �� static �������� ������ �̸��� �����ϰ� �Ϸ��� �Ѵ�.
	static std::vector<std::string>* m_pModelName_V;
	static std::vector<std::string>* m_pAnimName_V;
	static std::vector<std::string>* m_pTexture_V;
	static std::vector<std::string>* m_pShader_V;
	static std::vector<std::string>* m_pMatName_V;
	static std::vector<std::string>* m_pIBLName_V;
	static std::vector<std::string>* m_pBankName_V;
	static std::vector<std::string>* m_pQuestName_V;
	static std::vector<std::string>* m_pDialogueName_V;

};

