#pragma once

/// �� ���, ������Ʈ ��ġ �� �ʿ��� �׸��� Ŭ����
/// ������ �����̳� �������� �ʿ�����Ƿ� ���ӿ�����Ʈ�� Ʈ������ Ŭ������ �ʿ����

#include "SimpleMath.h"

class Grid
{
public:
	Grid();
	~Grid();

private:
	DirectX::SimpleMath::Vector3 m_Pos[2];

};

