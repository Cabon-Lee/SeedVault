#pragma once

/// <summary>
/// Triangle Ŭ���� 
/// 
/// NavMesh�� �κ��� �� Cell�� �ش��ϴ� ����
/// 
/// Triangle(Cell)�� �𿩼� NavMesh�� �̷��.
/// 
/// �ﰢ���� ��ȣ(index)�� �ְ�
/// ������ҷ� Vertex, Edge, CenterPos(�������� �ƴ϶� �ﰢ�� ���� ������)
/// ���� �����ͷ� �̷������.
/// 
/// �ۼ��� : YoKing
/// </summary>
class Triangle
{
public:
	// �ﰢ���� �𼭸�(2���� �������� �̷����)
	struct Edge
	{
		Vector3* v0;			// �𼭸��� �̷�� �ﰢ���� ���� 0 
		Vector3* v1;			// �𼭸��� �̷�� �ﰢ���� ���� 1

		float distanceToCenter;	// �𼭸��� ������� �ﰢ���� ���������� �Ÿ�
	};



public:
	Triangle();
	~Triangle();

	unsigned int m_Index;		// NavMesh ���� �ﰢ���� ��ȣ
	
	Vector3 m_CenterPosition;	// �ﰢ���� ���� ��ġ
	Vector3* m_Vertices[3];		// Vertex Position

	Edge m_Edge[3];				// �ﰢ�� �𼭸� ����

protected:

private:

};

