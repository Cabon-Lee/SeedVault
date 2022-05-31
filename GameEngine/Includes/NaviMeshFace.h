#pragma once

/// 2022.03.23 �̼���
/// �׺� �޽��� ���� �ﰢ���� �����ϱ� ���ؼ� ���� Ŭ����
/// �� ����� ���ҽ� �Ŵ��������� �����Ѵ�
/// ���� ������Ʈ�� �߰��� ������ Ȥ�ö� �� Ŭ������ �������� ���� �Լ��� �߰��� ���ɼ� ����
/// �ٸ�, ���ҽ� �Ŵ������� �Լ��� ���� ���ɼ��� �����Ѵ�

#include <vector>
#include <memory>
#include "SimpleMath.h"

struct NaviMeshFace
{
	NaviMeshFace()
		: m_Index(0)
		, m_FaceNormal(DirectX::SimpleMath::Vector3())
	{
		// ���̽��� �� ���� ����
		m_Vertex_Arr[0] = DirectX::SimpleMath::Vector3();
		m_Vertex_Arr[1] = DirectX::SimpleMath::Vector3();
		m_Vertex_Arr[2] = DirectX::SimpleMath::Vector3();

		/// Added by YoKing
		// ���� Face 
		m_AdjoinFace_V.reserve(3);

		// Face�� �߽��� �ʱ�ȭ
		m_CenterPosition = DirectX::SimpleMath::Vector3();

		// Edge�� ���� �ʱ�ȭ
		m_EdgeMid_Arr[0] = DirectX::SimpleMath::Vector3();
		m_EdgeMid_Arr[1] = DirectX::SimpleMath::Vector3();
		m_EdgeMid_Arr[2] = DirectX::SimpleMath::Vector3();
	
		// Face�� ���⺤�� �ʱ�ȭ
		m_CycleEdgeVector_Arr[0] = DirectX::SimpleMath::Vector3();
		m_CycleEdgeVector_Arr[1] = DirectX::SimpleMath::Vector3();
		m_CycleEdgeVector_Arr[2] = DirectX::SimpleMath::Vector3();

	};
	~NaviMeshFace() 
	{
		// �迭 �����ϴ� ���..?
		
	};

	unsigned int m_Index;										// ���̽�(�ﰢ��) �ε���
	DirectX::SimpleMath::Vector3 m_Vertex_Arr[3];				// ���̽��� �̷�� ���� A, B, C
	DirectX::SimpleMath::Vector3 m_FaceNormal;					// �븻
	
	// Added by Yoking
	DirectX::SimpleMath::Vector3 m_CenterPosition;				// ���̽��� ��ġ(�ﰢ���� �߾�)
	DirectX::SimpleMath::Vector3 m_EdgeMid_Arr[3];				// Edge�� ����( [0] -> (A,B) �� ���, [1] -> (B,C)�� ���, [2] -> (C,A) �� ���
	DirectX::SimpleMath::Vector3 m_CycleEdgeVector_Arr[3];		// Face�� �� Edge�鿡 ���� ���⺤��
																// [0] A -> B
																// [1] B -> C
																// [2] C -> A

	std::vector<std::shared_ptr<NaviMeshFace>> m_AdjoinFace_V;	// ���� Face ����
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

