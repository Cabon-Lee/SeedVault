#pragma once

/// �Էµ� ����� ��� Directory�� Ȯ���ϰ�, ��� ����� ������ �о���̴� Ŭ����
/// �ܼ��� ������� ���Ҹ� �ϱ� ������ ���δ� static���� �������

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
	// ��� ��θ� ã�ƿ��� �Լ��� ��ͷ� �����Ѵ�
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