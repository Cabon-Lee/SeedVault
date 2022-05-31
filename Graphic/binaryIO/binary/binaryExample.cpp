#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "BinaryFileManager.h"
#include "BinaryFileManager.cpp"
#include "binary.h"
#include "Vertex.h"
#include <string>
struct Test
{
	std::vector<Vertex> vertex_V;
};
void main()
{
	///예제 1
	
	////std::ofstream of("binary.bin", std::ios::binary);
	//std::ofstream of("binary.txt");
	//
	//int i = 15;
	//char tt = 'a';
	//char bb = 'b';
	//std::string cc = "ccccca";
	//
	////of.write((char*)&i, sizeof(int));
	//of << cc << "\n";
	//
	//of << std::to_string(i) << "\n";
	//of.close();

	//std::ifstream input_f("binary.txt", std::ios::binary);
	//std::ifstream input_f("binary.txt");
	//
	//int k;
	//input_f.read((char*)&k, sizeof(int));
	//
	//std::cout << "read from file : " << k << std::endl;


	

	/// 예제 2
	/*
	float f1 = 1.f;
	int i1 = 64;
	char c1 = 'd';
	//float i2 = 455.f;
	int chachaSize = 4;
	char* chacha = new char[chachaSize] {0, };

	int index = 0;
	//BinaryFileManager::Serialization(chacha, chachaSize, index, &c1, sizeof(char));
	BinaryFileManager::Serialization(chacha, chachaSize, index, &i1, sizeof(int));
	//BinaryFileManager::Serialization(chacha, chachaSize, index, &i2, sizeof(float));

	BinaryFileManager::WriteBinarayFile("bichan.bin", chacha, chachaSize);

	char* chacha2 = new char[chachaSize] {0, };

	BinaryFileManager::ReadBinaryFile("bichan.bin", chacha2, chachaSize);

	int testIndex = 0;
	float testF1, testF2;
	int testI1, testI2;
	char testC1;
	BinaryFileManager::Deserializtion(&testI1, sizeof(testI1), chacha2, testIndex);
	//BinaryFileManager::Deserializtion(&testI2, sizeof(testI), chacha2, testIndex);
	*/

	/// 예제 3
	/*
	float i1 = 1.f;
	//float i2 = 0.00004f;
	Float3 i2;
	i2.x = 455.f;
	i2.y = 35.f;
	i2.z = 4.f;

	int chachaSize = 8;
	char* chacha = new char[chachaSize] {0, };

	int index = 0;
	BinaryFileManager::Serialization(chacha, chachaSize, index, &i1, sizeof(float));
	BinaryFileManager::Serialization(chacha, chachaSize, index, &i2, sizeof(float));

	BinaryFileManager::WriteBinarayFile("bichan.bin", chacha, chachaSize);

	char* chacha2 = new char[chachaSize] {0, };

	BinaryFileManager::ReadBinaryFile("bichan.bin", chacha2, chachaSize);

	int testIndex = 0;
	float testI, testI2;
	BinaryFileManager::Deserializtion(&testI, sizeof(testI), chacha2, testIndex);
	BinaryFileManager::Deserializtion(&testI2, sizeof(testI), chacha2, testIndex);
	*/

	/// 예제 4
	/*
	Vertex vertices[4]; // 직렬화 해야되는 대상

#pragma region VertexRegion
	{
		vertices[0].pos.x = 0.11f;
		vertices[0].pos.y = 0.12f;
		vertices[0].pos.z = 0.13f;

		vertices[0].normal.x = 11.f;
		vertices[0].normal.y = 12.f;
		vertices[0].normal.z = 13.f;

		vertices[0].texCoord.x = 100001.0f;
		vertices[0].texCoord.y = 100002.0f;
	}

	{
		vertices[1].pos.x = 0.21f;
		vertices[1].pos.y = 0.22f;
		vertices[1].pos.z = 0.23f;

		vertices[1].normal.x = 21.f;
		vertices[1].normal.y = 22.f;
		vertices[1].normal.z = 23.f;

		vertices[1].texCoord.x = 200001.0f;
		vertices[1].texCoord.y = 200002.0f;
	}

	{
		vertices[2].pos.x = 0.31f;
		vertices[2].pos.y = 0.32f;
		vertices[2].pos.z = 0.33f;

		vertices[2].normal.x = 31.f;
		vertices[2].normal.y = 32.f;
		vertices[2].normal.z = 33.f;

		vertices[2].texCoord.x = 300001.0f;
		vertices[2].texCoord.y = 300002.0f;
	}

	{
		vertices[3].pos.x = 0.41f;
		vertices[3].pos.y = 0.42f;
		vertices[3].pos.z = 0.43f;

		vertices[3].normal.x = 41.f;
		vertices[3].normal.y = 42.f;
		vertices[3].normal.z = 43.f;

		vertices[3].texCoord.x = 400001.0f;
		vertices[3].texCoord.y = 400002.0f;
	}
#pragma endregion VertexRegion

	std::vector<Vertex> vertex_V(4);
	vertex_V[0] = vertices[0];
	vertex_V[1] = vertices[1];
	vertex_V[2] = vertices[2];
	vertex_V[3] = vertices[3];

	int floatSize = sizeof(float);									// 4바이트
	int float3Size = sizeof(Float3);								// 12바이트
	int float2Size = sizeof(Float2);								// 8바이트
	int _structSize = sizeof(Vertex);								// 32바이트 12 + 12 + 8
	int _verticesSize = sizeof(vertices);							// 32바이트 12 + 12 + 8
	int _vertexVSize = sizeof(vertex_V);							// 32바이트 12 + 12 + 8
	int _vertexVMulsizeSize = sizeof(vertex_V) * vertex_V.size();	// 32바이트 12 + 12 + 8
	int _struct2Size = sizeof(VertexSkinned);						// 60바이트 12 + 8 + 12 + 12 + 16
	int _structpackSize = sizeof(pack1);							// 8바이트
	int _structnotpackSize = sizeof(notpack);						// 8바이트
	int _stringSize = sizeof(std::string);							// 8바이트

	/// 배열
	//char* serializeContainer = new char[_verticesSize] {0, };
	//
	//int index1 = 0;
	//BinaryFileManager::Serialization(serializeContainer, _verticesSize, index1, &vertices, sizeof(vertices));
	//BinaryFileManager::WriteBinarayFile("vertex.bin", serializeContainer, _verticesSize);
	//
	//char* deserializeContainer = new char[_verticesSize] {0, };
	//
	//BinaryFileManager::ReadBinaryFile("vertex.bin", deserializeContainer, _verticesSize);
	//
	//int testIndex1 = 0;
	//Vertex vertextest[4];
	//BinaryFileManager::Deserializtion(&vertextest, sizeof(vertextest), deserializeContainer, testIndex1);


	///////////////////////////////////////////////////////////////////////////////////////////////////
	/// 벡터

	char* serializeContainer = new char[_vertexVMulsizeSize] {0, };


	int index1 = 0;
	BinaryFileManager::Serialization(serializeContainer, _vertexVMulsizeSize, index1, &vertex_V, sizeof(vertex_V)* vertex_V.size());
	BinaryFileManager::WriteBinarayFile("vertex.bin", serializeContainer, _vertexVMulsizeSize);

	char* deserializeContainer = new char[_vertexVMulsizeSize] {0, };

	BinaryFileManager::ReadBinaryFile("vertex.bin", deserializeContainer, _vertexVMulsizeSize);

	int testIndex1 = 0;
	std::vector<Vertex> vertex_V2(4);
	BinaryFileManager::Deserializtion(&vertex_V2, sizeof(vertex_V2), deserializeContainer, testIndex1);
	*/

	/// 예제5 : packing 비교
	/*
	pack1 pack;
	{
		pack.a = 10;
		pack.b = 'i';
		pack.c = 5.0f;
	}
	notpack npack;
	{
		npack.a = 10;
		npack.b = 'i';
		npack.c = 5.0f;
	}

	int packSize = sizeof(pack);
	int notpackSize = sizeof(npack);

	char* packBuffer = new char[packSize] {0, };
	char* notpackBuffer = new char[notpackSize] {0, };

	int packIndex = 0;
	int notpackIndex = 0;

	BinaryFileManager::Serialization(packBuffer, packSize, packIndex, &pack, packSize);
	BinaryFileManager::Serialization(notpackBuffer, notpackSize, notpackIndex, &npack, notpackSize);

	BinaryFileManager::WriteBinarayFile("pack.bin", packBuffer, packSize);
	BinaryFileManager::WriteBinarayFile("notpack.bin", notpackBuffer, notpackSize);

	char* packContainer = new char[packSize] {0, };
	char* notpackContainer = new char[notpackSize] {0, };

	BinaryFileManager::ReadBinaryFile("pack.bin", packContainer, packSize);
	BinaryFileManager::ReadBinaryFile("notpack.bin", notpackContainer, notpackSize);

	pack1 pack2;
	notpack npack2;

	int packIndex1 = 0;
	int notpackIndex1 = 0;

	BinaryFileManager::Deserializtion(&pack2, sizeof(pack2), packContainer, packIndex1);
	BinaryFileManager::Deserializtion(&npack2, sizeof(npack2), notpackContainer, notpackIndex1);
	*/

	/// 예제6 : 벡터 포인터

	//Vertex* vertices[4];
	//for (int i = 0; i < 4; i++)
	//{
	//	vertices[i] = new Vertex(); // 직렬화 해야되는 대상
	//}

	Vertex vertices[4];

#pragma region VertexRegion
	{
		vertices[0].pos.x = 0.11f;
		vertices[0].pos.y = 0.12f;
		vertices[0].pos.z = 0.13f;

		vertices[0].normal.x = 11.f;
		vertices[0].normal.y = 12.f;
		vertices[0].normal.z = 13.f;

		vertices[0].texCoord.x = 100001.0f;
		vertices[0].texCoord.y = 100002.0f;
	}

	{
		vertices[1].pos.x = 0.21f;
		vertices[1].pos.y = 0.22f;
		vertices[1].pos.z = 0.23f;

		vertices[1].normal.x = 21.f;
		vertices[1].normal.y = 22.f;
		vertices[1].normal.z = 23.f;

		vertices[1].texCoord.x = 200001.0f;
		vertices[1].texCoord.y = 200002.0f;
	}

	{
		vertices[2].pos.x = 0.31f;
		vertices[2].pos.y = 0.32f;
		vertices[2].pos.z = 0.33f;

		vertices[2].normal.x = 31.f;
		vertices[2].normal.y = 32.f;
		vertices[2].normal.z = 33.f;

		vertices[2].texCoord.x = 300001.0f;
		vertices[2].texCoord.y = 300002.0f;
	}

	{
		vertices[3].pos.x = 0.41f;
		vertices[3].pos.y = 0.42f;
		vertices[3].pos.z = 0.43f;

		vertices[3].normal.x = 41.f;
		vertices[3].normal.y = 42.f;
		vertices[3].normal.z = 43.f;

		vertices[3].texCoord.x = 400001.0f;
		vertices[3].texCoord.y = 400002.0f;
	}
#pragma endregion VertexRegion
	Test *t = new Test();

	//std::vector<Vertex> vertex_V;
	t->vertex_V.resize(4);

	t->vertex_V[0] = vertices[0];
	t->vertex_V[1] = vertices[1];
	t->vertex_V[2] = vertices[2];
	t->vertex_V[3] = vertices[3];

	int floatSize = sizeof(float);											// 4바이트
	int float3Size = sizeof(Float3);										// 12바이트
	int float2Size = sizeof(Float2);										// 8바이트
	int _structSize = sizeof(Vertex);										// 32바이트 12 + 12 + 8
	int _verticesSize = sizeof(vertices);									// 32바이트 12 + 12 + 8
	int _vertexVSize = sizeof(t->vertex_V);									// 32바이트 12 + 12 + 8
	int _vertexVMulsizeSize = sizeof(t->vertex_V) * t->vertex_V.size();		// 32바이트 12 + 12 + 8
	int _struct2Size = sizeof(VertexSkinned);								// 60바이트 12 + 8 + 12 + 12 + 16
	int _structpackSize = sizeof(pack1);									// 8바이트
	int _structnotpackSize = sizeof(notpack);								// 8바이트
	int _stringSize = sizeof(std::string);									// 8바이트

	/// 배열	
	//char* serializeContainer = new char[_verticesSize] {0, };
	//
	//int index1 = 0;
	//BinaryFileManager::Serialization(serializeContainer, _verticesSize, index1, &vertices, sizeof(vertices));
	//BinaryFileManager::WriteBinarayFile("vertex.bin", serializeContainer, _verticesSize);
	//
	//char* deserializeContainer = new char[_verticesSize] {0, };
	//
	//BinaryFileManager::ReadBinaryFile("vertex.bin", deserializeContainer, _verticesSize);
	//
	//int testIndex1 = 0;
	//Vertex vertextest[4];
	//BinaryFileManager::Deserializtion(&vertextest, sizeof(vertextest), deserializeContainer, testIndex1);


	///////////////////////////////////////////////////////////////////////////////////////////////////
	/// 벡터

	//char* serializeContainer = new char[_vertexVMulsizeSize] {0, };
	//
	//int index1 = 0;
	//BinaryFileManager::Serialization(serializeContainer, _vertexVMulsizeSize, index1, t->vertex_V.data(), sizeof(t->vertex_V)* t->vertex_V.size());
	//BinaryFileManager::WriteBinarayFile("vertex.bin", serializeContainer, _vertexVMulsizeSize);
	//
	//char* deserializeContainer = new char[_vertexVMulsizeSize] {0, };
	//
	//BinaryFileManager::ReadBinaryFile("vertex.bin", deserializeContainer, _vertexVMulsizeSize);
	//
	//int testIndex1 = 0;
	//std::vector<Vertex> vertex_V2(4);
	//
	////memset(vertex_V2.data(), 0, sizeof(Vertex) * 4);
	//
	//BinaryFileManager::Deserializtion(vertex_V2.data(), sizeof(Vertex)* vertex_V2.size(), deserializeContainer, testIndex1);
	////BinaryFileManager::Deserializtion(&vertex_V2, sizeof(Vertex)* vertex_V2.size(), deserializeContainer, testIndex1);
	//


	/// 예제7 
	//std::ofstream of("binary.bin", std::ios::binary);
	//
	//int* i = new int(15);
	//int* j = new int(16);
	//
	//of.write(reinterpret_cast<char*>(i), sizeof(int));
	//of.write(reinterpret_cast<char*>(j), sizeof(int));
	////for (int i = 0; i < t->vertex_V.size(); ++i)
	//{
	//	of.write(reinterpret_cast<char*>(t->vertex_V.data()), sizeof(Vertex) * t->vertex_V.size());
	//}
	//of.close();
	//
	//std::ifstream input_f("binary.bin", std::ios::binary);
	//
	//int k = 0;
	//int l = 0;
	//
	//input_f.read(reinterpret_cast<char*>(&k), sizeof(k));
	//input_f.read(reinterpret_cast<char*>(&l), sizeof(l));
	//
	//std::vector<Vertex> temp_vertex_V(4);
	//
	////for (int i = 0; i < temp_vertex_V.size(); ++i)
	////{
	////	input_f.read(reinterpret_cast<char*>(&temp_vertex_V[i]), sizeof(Vertex));
	////}
	//
	//input_f.read(reinterpret_cast<char*>(temp_vertex_V.data()), sizeof(Vertex) * temp_vertex_V.size());
	//
	//input_f.close();
	//
	//std::cout << "read from file : " << k << std::endl;
	//std::cout << "read from file : " << l << std::endl;
	//
	//for (int i = 0; i < temp_vertex_V.size(); ++i)
	//{
	//	std::cout << "\t" << "posx : " << temp_vertex_V[i].pos.x << std::endl;
	//	std::cout << "\t" << "posy : " << temp_vertex_V[i].pos.y << std::endl;
	//	std::cout << "\t" << "posz : " << temp_vertex_V[i].pos.z << std::endl;
	//				 
	//	std::cout << "\t" << "normalx : " << temp_vertex_V[i].normal.x << std::endl;
	//	std::cout << "\t" << "normaly : " << temp_vertex_V[i].normal.y << std::endl;
	//	std::cout << "\t" << "normalz : " << temp_vertex_V[i].normal.z << std::endl;
	//				 
	//	std::cout << "\t" <<"texCoordu : " << temp_vertex_V[i].texCoord.x << std::endl;
	//	std::cout << "\t" <<"texCoordv : " << temp_vertex_V[i].texCoord.y << std::endl;
	//	std::cout << "\n";
	//}

	/// 예제8 : 2차원 벡터

	//std::vector<std::vector<int>> twoVec;
	//
	//std::vector<int> one(10, 1);
	//std::vector<int> two(20, 2);
	//std::vector<int> three(30, 3);
	//std::vector<int> four(40, 4);
	//
	//twoVec.push_back(one);
	//twoVec.push_back(two);
	//twoVec.push_back(three);
	//twoVec.push_back(four);
	//
	//twoVec[0][0] = 1;
	//twoVec[0][1] = 1;
	//twoVec[0][2] = 1;
	//
	//twoVec[3][0] = 1;
	//twoVec[3][2] = 1;
	//twoVec[3][39] = 1;
	//
	//int i = 1005;
	//
	//std::ofstream of("binary.bin", std::ios::binary);
	//for (int i = 0; i < twoVec.size(); ++i)
	//{
	//	of.write(reinterpret_cast<char*>(twoVec[i].data()), ((sizeof(int) * twoVec[i].size())));
	//}
	////of.write(reinterpret_cast<char*>(twoVec.data()), ((sizeof(int)* one.size()) + (sizeof(int) * two.size()) + (sizeof(int) * three.size()) + (sizeof(int) * four.size())));
	//of.write(reinterpret_cast<char*>(&i), (sizeof(int)));
	//of.close();
	//
	////////////////////////////////////////////////////////////////////////////
	//std::vector<std::vector<int>> twoVec2;
	//std::vector<int> one2(10);
	//std::vector<int> two2(20);
	//std::vector<int> three2(30);
	//std::vector<int> four2(40);
	//
	//twoVec2.push_back(one2);
	//twoVec2.push_back(two2);
	//twoVec2.push_back(three2);
	//twoVec2.push_back(four2);
	//int temp = 0;
	//
	//std::ifstream input_f("binary.bin", std::ios::binary);
	////input_f.read(reinterpret_cast<char*>(twoVec2.data()), ((sizeof(int)* one2.size()) + (sizeof(int) * two2.size()) + (sizeof(int) * three2.size()) + (sizeof(int) * four2.size())));
	//for (int i = 0; i < twoVec2.size(); ++i)
	//{
	//	input_f.read(reinterpret_cast<char*>(twoVec2[i].data()), ((sizeof(int) * twoVec2[i].size())));
	//}
	//input_f.read(reinterpret_cast<char*>(&temp), (sizeof(int)));
	//input_f.close();


	/// 예제8 : 벡터 스트링

	struct AA
	{
		std::string name = "mama";
		int i = 0;
	};

	std::vector<AA> aaContain(1);
	
	aaContain[0].name = "nono";
	//aaContain[2].name = "squre";
	//aaContain[3].name = "yesyes";
	//
	aaContain[0].i = 1;
	//aaContain[2].i = 2;
	//aaContain[3].i = 3;	
	std::string tag = "tagtag";

	std::vector<int> size(1);
	
	for (int i = 0; i < aaContain.size(); ++i) //(sizeof(int) * twoVec[i].size())
	{
		size[i] = static_cast<int>(aaContain[i].name.length() + 1);
	}

	int tagSize = static_cast<int>(tag.length() + 1);

	std::ofstream of("binary.bin", std::ios::binary);	
	
	for (int i = 0; i < aaContain.size(); ++i) //(sizeof(int) * twoVec[i].size())
	{
		//of.write(aaContain[i].name.c_str(), sizeof(aaContain[i].name.c_str()) - 1);
		//of.write(aaContain[i].name.c_str(), sizeof(aaContain[i].name.c_str()) - 1);
		of.write(aaContain[i].name.c_str(), size[i] - 1);
		of.write("\0", 1);
	
		//of.write(tag.c_str(), tagSize - 1);
		//of.write("\0", 1);
		of.write(reinterpret_cast<char*>(&aaContain[i].i), sizeof(int));
	}
	of.close();

	std::vector<AA> aa2Contain(1);
	aa2Contain[0].name.resize(size[0]);
	std::string tagtagtag = "";
	std::ifstream input_f("binary.bin", std::ios::binary);

	for (int i = 0; i < aaContain.size(); ++i) //(sizeof(int) * twoVec[i].size())
	{
		//input_f.read(reinterpret_cast<char*>(&aa2Contain[i].name), sizeof(aaContain[i].name.c_str()));
		//input_f.read(reinterpret_cast<char*>(aa2Contain.data()[1].i), size[i]);
		//aa2Contain[i].name.clear();
		input_f.read(reinterpret_cast<char*>(&aa2Contain[i].name[0]), size[i]);
		//tagtagtag.resize(tagSize);
		//input_f.read(reinterpret_cast<char*>(&tagtagtag[0]), tagSize);
		//input_f.read(reinterpret_cast<char*>(&aa2Contain[i].name), size[i]);
		input_f.read(reinterpret_cast<char*>(&aa2Contain[i].i), sizeof(int));
	}
	input_f.close();



	/// 예제9 





	return;
}
