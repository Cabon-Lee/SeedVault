// Parser.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

/// 파싱만을 목적으로 하는 어플리케이션, 딱히 윈도우는 필요없다
/// 이 어플리케이션을 실행하면 FBX를 각각 binary로 바꿔준다
/// 스탑워치를 넣어서 얼마나 걸렸는지 시간도 표시하고
/// cout으로 어떤 걸 하는 지도 알려주자

#include <iostream>
#include <memory>

#include "JMFBXParser.h"
#include "DirectoryReader.h"
#include "SimpleMath.h"
#include "StopWatch.h"

using namespace std;

#ifdef _DEBUG
std::string RESOURCE_PATH = "../../SeedVault/Data/model/";
std::string WRITE_PATH = "../../SeedVault/Data/bin/";
#else
std::string RESOURCE_PATH = "../Data/model/";
std::string WRITE_PATH = "../Data/bin/";
#endif


std::vector<std::string> g_MeshNames;

int main()
{
    cout << "FBX 파싱을 시작한다!" << endl;
	StopWatch::StartMeasure();

    // 일단 정해진 경로에서 FBX를 가져온다
	DirectoryReader::InputEntryDir(RESOURCE_PATH);
    g_MeshNames = DirectoryReader::GetMeshNames();

    for (std::string& _nowName : g_MeshNames)
    {
		cout << "FBX Path = " << _nowName.c_str() << endl;
    }

    std::shared_ptr<JMFBXParser> _JMParser = std::make_shared<JMFBXParser>();

    _JMParser->Initalize();

	for (std::string& _nowName : g_MeshNames)
	{
	    _JMParser->LoadScene(_nowName, WRITE_PATH);
	}

	StopWatch::StopMeasure();
}

void MakeDirectory(wchar_t* full_path)
{
	wchar_t temp[256], * sp;
	wcscpy_s(temp, full_path); // 경로문자열을 복사
	sp = temp; // 포인터를 문자열 처음으로

	while ((sp = wcschr(sp, '\\')))  // 디렉토리 구분자를 찾았으면
	{
		if (sp > temp && *(sp - 1) != ':') // 루트디렉토리가 아니면
		{ 
			*sp = '\0'; // 잠시 문자열 끝으로 설정
			//mkdir(temp, S_IFDIR);
			CreateDirectory(temp, NULL); // 디렉토리를 만들고 (존재하지 않을 때)
			
			*sp = '\\'; // 문자열을 원래대로 복귀
		}
		sp++; // 포인터를 다음 문자로 이동
	}

}

void MakeFolder(std::string WRITE_PATH)
{
    //if ((_waccess(_T(WRITE_PATH), 0)) == -1) //여기에 LOG폴더가 없으면...
    {
		//CreateDirectory(_T(WRITE_PATH), NULL);
    }
	//Create Directory
}