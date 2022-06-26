#pragma once

/// <summary>
/// NavMesh 클래스 
/// 
/// NavMesh Agent들이 이동할 지형
/// 삼각형의 집함으로 구성된다.
/// 
/// Scene(Map)별로 1개씩 존재해야 함.
/// 
/// 작성자 : 최 요 환
/// </summary>

class NavMesh
{
public:
	NavMesh();
	~NavMesh();

	void Initialize();
	void Render();

protected:

private:
	// 삼각형 정보들 필요..
};

