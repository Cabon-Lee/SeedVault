#pragma once

/// <summary>
/// ��ƿ��Ƽ �Լ���
/// �Ÿ� ��, ���� ��� ��
/// </summary>

#define RAD_TO_DEGREE(rad) { rad * (180.0f / 3.1415926535f) }

namespace UtilityFunction
{

	/// <summary>
	/// Ÿ���� Ž�� ���� �ȿ� �ִ��� �˻�
	/// </summary>
	/// <param name="origin">���� ��ġ</param>
	/// <param name="target">��� ��ġ</param>
	/// <param name="detectionRange">�Ÿ�</param>
	/// <returns>���</returns>
	static bool IsTargetInDetectionRange(SimpleMath::Vector3 origin, const SimpleMath::Vector3 target, const float detectionRange)
	{
		// �Ÿ��� ������
		float distance = Vector3::Distance(origin, target);

		if (distance < detectionRange)
		{
			return true;
		}

		else
		{
			return false;
		}
	}

	/// <summary>
	/// Ÿ���� Ž�� ���� �ȿ� �ִ��� �˻�
	/// </summary>
	/// <param name="origin">���� ������Ʈ</param>
	/// <param name="target">��� ������Ʈ</param>
	/// <param name="detectionRange">�Ÿ�</param>
	/// <returns>���</returns>
	static bool IsTargetInDetectionRange(GameObject& origin, const GameObject& target, const float detectionRange)
	{
		// �Ÿ��� ������
		float distance = Vector3::Distance(origin.m_Transform->m_Position, target.m_Transform->m_Position);

		return IsTargetInDetectionRange(origin.m_Transform->m_Position, target.m_Transform->m_Position, detectionRange);
	}

	/// <summary>
	/// target ������Ʈ���� ������ ���ؼ� �����Ѵ�
	/// </summary>
	/// <param name="origin">���� ������Ʈ</param>
	/// <param name="target">Ÿ�� ������Ʈ</param>
	/// <returns>Ÿ������ ���ϴ� ���Ϸ���</returns>
	static float CalcAngleToTarget(const GameObject& origin, const GameObject& target)
	{
		Vector3 myPos = origin.m_Transform->m_Position;
		Vector3 targetPos = target.m_Transform->m_Position;

		float dx = targetPos.x - myPos.x;
		float dz = targetPos.z - myPos.z;

		float rad = std::atan2f(dx, dz);
		float angle = RAD_TO_DEGREE(rad);

		return angle;
	}

	/// <summary>
	/// ���� 0 ~ 360 �� ���̷� �����ϱ� ���� �Լ�
	/// ���� ������ �ֱ������� ���߿� ����.
	/// </summary>
	/// <param name="angle">����ȭ�� ��</param>
	static bool NormalizeAngle(float& angle)
	{
		if (angle > 360)
		{
			angle -= 360;
		}
		else if (angle < 0)
		{
			angle += 360;
		}

		return angle;
	}
}