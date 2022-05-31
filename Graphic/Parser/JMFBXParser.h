#pragma once
#include <vector>
#include <map>
#include <fbxsdk.h>
#include "ParsingDataClass.h"

class JMFBXParser
{
public:
	JMFBXParser();
	~JMFBXParser();

	void Initalize();
	void Destroy();
	void LoadScene(fbxsdk::FbxString fileName);

private:
	void SceneSetting();
	void LoadNode(fbxsdk::FbxNode* node);
	void LoadMesh(fbxsdk::FbxNode* node);
	void ProcessControlPoints(fbxsdk::FbxMesh* mesh);

private:
	fbxsdk::FbxManager* m_pManager;
	fbxsdk::FbxImporter* m_pImporter;
	fbxsdk::FbxScene* m_pScene;

	fbxsdk::FbxString m_fbxFileName;
	ParserData::Mesh* m_pOneMesh;

public:
	map<std::string, ParserData::Mesh*> m_Mesh_M;
};

