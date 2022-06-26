#pragma once

/// <summary>
/// Triangle 클래스 
/// 
/// NavMesh의 부분인 한 Cell에 해당하는 영역
/// 
/// Triangle(Cell)이 모여서 NavMesh를 이룬다.
/// 
/// 삼각형은 번호(index)가 있고
/// 구성요소로 Vertex, Edge, CenterPos(꼭지점이 아니라 삼각형 면의 포지션)
/// 등의 데이터로 이루어진다.
/// 
/// 작성자 : 최 요 환
/// </summary>
class Triangle
{
public:
	// 삼각형의 모서리(2개의 정점으로 이루어짐)
	struct Edge
	{
		Vector3* v0;			// 모서리를 이루는 삼각형의 정점 0 
		Vector3* v1;			// 모서리를 이루는 삼각형의 정점 1

		float distanceToCenter;	// 모서리의 가운데부터 삼각형의 중점까지의 거리
	};



public:
	Triangle();
	~Triangle();

	unsigned int m_Index;		// NavMesh 에서 삼각형의 번호
	
	Vector3 m_CenterPosition;	// 삼각형의 중점 위치
	Vector3* m_Vertices[3];		// Vertex Position

	Edge m_Edge[3];				// 삼각형 모서리 정보

protected:

private:

};

