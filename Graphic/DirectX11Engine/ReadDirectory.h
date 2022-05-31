#pragma once

/// 210615
/// 디렉토리 읽기, 파일 이름, 경로, 확장자
/// 나열한 것들을 관리하기 위한 클래스
/// 디렉토리에 넣기만하면 편하게 불러오기 위해서 만들었다.

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

	// 경로와 벡터를 전달하면 이름을 담는다.
	void FindFileFromDirectory(std::string path, std::vector<std::string>& fileNames);

	// 경로와 벡터를 전달하면 이름과 경로를 붙여준다.
	void FindFileFromDirectory(std::string path, std::vector<std::string>& fileNames, eFileType fileType);

	// 파일 타입이 텍스쳐인지 ASE인지 알아봐주고 
	// 벡터 내에서 걸러준다.
	void ValidateFileType(std::vector<std::string>& fileNames, eFileType fileType);
	
	// 파일 형식이 DSS인지 아닌지 파악한다.
	bool IsTextureDDS(std::string& fileName);

	// 파일 이름에서 확장자를 붙이거나 떼준다.
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

