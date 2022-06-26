#pragma once

/// 리소스매니저는 원래 렌더러에 넣으려고 했으나, 짜다보니 영 이상한 구조가 되었다.
/// 게임 엔진이 어떤 것을 그릴지 클라이언트를 통해서 가져올 수 있어야 하는데, 그때마다 렌더러에 접근하는 것은 별로.
/// 또한 모든 데이터가 로드된 다음 렌더러의 리소스 매니저가 어떤 것들을 로드했는지 게임 엔진에 알려주는 것도 별로

#include <unordered_map>
#include <vector>
#include <string>
#include "AnimationSystem.h"

__interface IResourceManager;


// 파일 이름을 넣으면 메쉬메타 데이터를 채워줄 수 있도록
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
	 
	// 사용되는 모든 외부 리소스의 이름들은 두개로 복사될 필요가 없다
	// MeshFilter, MaterialComponent에서 이 static 변수들을 가지고 이름에 접근하게 하려고 한다.
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

