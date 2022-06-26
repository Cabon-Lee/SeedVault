#pragma once

/// 입력된 경로의 모든 Directory를 확인하고, 모든 경로의 파일을 읽어들이는 클래스
/// 단순히 기능적인 역할만 하기 때문에 내부는 static으로 만들었다

#include <string>
#include <vector>
#include <queue>

class DirectoryReader
{
public:
	static void InputEntryDir(std::string& path);

	static std::vector<std::string>& GetTextureNames();
	static std::vector<std::string>& GetMeshNames();
	static std::vector<std::string>& GetShaderNames();
	static std::vector<std::string>& GetBinaryNames();
	static std::vector<std::string>& GetAnimNames();
	static std::vector<std::string>& GetMatNames();
	static std::vector<std::string>& GetIBLNames();
	static std::vector<std::string>& GetBankNames();
	static std::vector<std::string>& GetQuestNames();
	static std::vector<std::string>& GetDialogueNames();

	static std::string GetFileName(std::string& name, bool ex);


private:
	// 모든 경로를 찾아오는 함수로 재귀로 동작한다
	static void GetAllDir(std::string& path, std::vector<std::string>& dirs);
	static void GetContents(std::string& path, std::vector<std::string>& dirs);
	static void StorePath(std::string& path);
	static void ReverseString(std::string& path);

	static std::vector<std::string> m_TextureNames;
	static std::vector<std::string> m_MeshNames;
	static std::vector<std::string> m_BinaryNames;
	static std::vector<std::string> m_ShaderNames;
	static std::vector<std::string> m_AnimNames;
	static std::vector<std::string> m_MatNames;
	static std::vector<std::string> m_IBLNames;
	static std::vector<std::string> m_BankNames;
	static std::vector<std::string> m_QuestNames;
	static std::vector<std::string> m_DialogueNames;

};