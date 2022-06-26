#pragma once

/// 2022.03.23 이수종
/// 네비 메쉬에 사용될 삼각형을 정의하기 위해서 만든 클래스
/// 이 헤더는 리소스 매니저에서도 참조한다
/// 엔진 프로젝트에 추가한 이유는 혹시라도 이 클래스에 엔진에서 사용될 함수가 추가될 가능성 때문
/// 다만, 리소스 매니저에서 함수가 사용될 가능성은 배재한다

#include <vector>
#include <memory>
#include "SimpleMath.h"

struct NaviMeshFace
{
	NaviMeshFace()
		: m_Index(0)
		, m_FaceNormal(DirectX::SimpleMath::Vector3())
	{
		// 페이스의 각 정점 정보
		m_Vertex_Arr[0] = DirectX::SimpleMath::Vector3();
		m_Vertex_Arr[1] = DirectX::SimpleMath::Vector3();
		m_Vertex_Arr[2] = DirectX::SimpleMath::Vector3();

		/// Added by 최 요 환
		// 인접 Face 
		m_AdjoinFace_V.reserve(3);

		// Face의 중심점 초기화
		m_CenterPosition = DirectX::SimpleMath::Vector3();

		// Edge의 중점 초기화
		m_EdgeMid_Arr[0] = DirectX::SimpleMath::Vector3();
		m_EdgeMid_Arr[1] = DirectX::SimpleMath::Vector3();
		m_EdgeMid_Arr[2] = DirectX::SimpleMath::Vector3();
	
		// Face의 유향벡터 초기화
		m_CycleEdgeVector_Arr[0] = DirectX::SimpleMath::Vector3();
		m_CycleEdgeVector_Arr[1] = DirectX::SimpleMath::Vector3();
		m_CycleEdgeVector_Arr[2] = DirectX::SimpleMath::Vector3();

	};
	~NaviMeshFace() 
	{
		// 배열 해제하는 방법..?
		
	};

	unsigned int m_Index;										// 페이스(삼각형) 인덱스
	DirectX::SimpleMath::Vector3 m_Vertex_Arr[3];				// 페이스를 이루는 정점 A, B, C
	DirectX::SimpleMath::Vector3 m_FaceNormal;					// 노말
	
	// Added by 최 요 환
	DirectX::SimpleMath::Vector3 m_CenterPosition;				// 페이스의 위치(삼각형의 중앙)
	DirectX::SimpleMath::Vector3 m_EdgeMid_Arr[3];				// Edge의 중점( [0] -> (A,B) 의 가운데, [1] -> (B,C)의 가운데, [2] -> (C,A) 의 가운데
	DirectX::SimpleMath::Vector3 m_CycleEdgeVector_Arr[3];		// Face의 각 Edge들에 대한 유향벡터
																// [0] A -> B
																// [1] B -> C
																// [2] C -> A

	std::vector<std::shared_ptr<NaviMeshFace>> m_AdjoinFace_V;	// 인접 Face 정보
};

class NaviMeshSet
{
public:
	NaviMeshSet() {};
	~NaviMeshSet() 
	{
		if (m_NeviMeshFace_V.empty() == true) return;
		for (size_t i = m_NeviMeshFace_V.size() - 1; i > 0; --i)
		{
			m_NeviMeshFace_V[i].reset();
		}
		m_NeviMeshFace_V.clear();
	};

	void SetNeviMeshFaces(std::vector<std::shared_ptr<NaviMeshFace>>& faces)
	{
		m_NeviMeshFace_V = faces;
	}

	unsigned int GetIndexSize();

	std::shared_ptr<NaviMeshFace> GetNeviMeshFace(unsigned int idx);

	
private:
	std::vector<std::shared_ptr<NaviMeshFace>> m_NeviMeshFace_V;

};

