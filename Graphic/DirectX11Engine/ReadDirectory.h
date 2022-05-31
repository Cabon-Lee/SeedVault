#pragma once

/// 210615
/// ���丮 �б�, ���� �̸�, ���, Ȯ����
/// ������ �͵��� �����ϱ� ���� Ŭ����
/// ���丮�� �ֱ⸸�ϸ� ���ϰ� �ҷ����� ���ؼ� �������.

#include <string>
#include <vector>

enum class eFileType
{
	TEXTURE = 0,
	ASE,
	FX,
};

enum class eEXTENSION
{
	DDS = 0,
	PNG,
	BMP,
	TIF,
	JPG,
	EXE,
	ASE,
	MISCELLANEOUS,
};

enum class eFX
{
	CSO = 0,
	FXO,
	FX,
};

class ReadDirectory
{
	ReadDirectory();

public:
	static ReadDirectory* GetInstance() 
	{ 
		if(m_Instance == nullptr)
		{ 
			m_Instance = new ReadDirectory();
		}
		
		return m_Instance; 
	}
	~ReadDirectory();

	// ��ο� ���͸� �����ϸ� �̸��� ��´�.
	void FindFileFromDirectory(std::string path, std::vector<std::string>& fileNames);

	// ��ο� ���͸� �����ϸ� �̸��� ��θ� �ٿ��ش�.
	void FindFileFromDirectory(std::string path, std::vector<std::string>& fileNames, eFileType fileType);

	// ���� Ÿ���� �ؽ������� ASE���� �˾ƺ��ְ� 
	// ���� ������ �ɷ��ش�.
	void ValidateFileType(std::vector<std::string>& fileNames, eFileType fileType);
	
	// ���� ������ DSS���� �ƴ��� �ľ��Ѵ�.
	bool IsTextureDDS(std::string& fileName);

	// ���� �̸����� Ȯ���ڸ� ���̰ų� ���ش�.
	void GetFileName(std::string& name, bool remainEx);

private:
	void IsTextureFileName(std::vector<std::string>& fileNames);
	void IsASEFileName(std::vector<std::string>& fileNames);
	void IsFXFileName(std::vector<std::string>& fileNames);
	eEXTENSION CheckExtension(std::string& name);
	eFX CheckExtensionFX(std::string& name);
	void ReverseString(std::string& str);

private:
	static ReadDirectory* m_Instance;

};

