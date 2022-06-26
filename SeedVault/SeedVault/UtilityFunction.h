#pragma once

/// <summary>
/// 유틸리티 함수들
/// 거리 비교, 각도 계산 등
/// </summary>

#define RAD_TO_DEGREE(rad) { rad * (180.0f / 3.1415926535f) }

namespace UtilityFunction
{

	/// <summary>
	/// 타겟이 탐지 범위 안에 있느지 검사
	/// </summary>
	/// <param name="origin">기준 위치</param>
	/// <param name="target">대상 위치</param>
	/// <param name="detectionRange">거리</param>
	/// <returns>결과</returns>
	static bool IsTargetInDetectionRange(SimpleMath::Vector3 origin, const SimpleMath::Vector3 target, const float detectionRange)
	{
		// 거리를 구하자
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
	/// 타겟이 탐지 범위 안에 있느지 검사
	/// </summary>
	/// <param name="origin">기준 오브젝트</param>
	/// <param name="target">대상 오브젝트</param>
	/// <param name="detectionRange">거리</param>
	/// <returns>결과</returns>
	static bool IsTargetInDetectionRange(GameObject& origin, const GameObject& target, const float detectionRange)
	{
		// 거리를 구하자
		float distance = Vector3::Distance(origin.m_Transform->m_Position, target.m_Transform->m_Position);

		return IsTargetInDetectionRange(origin.m_Transform->m_Position, target.m_Transform->m_Position, detectionRange);
	}

	/// <summary>
	/// target 오브젝트로의 각도를 구해서 리턴한다
	/// </summary>
	/// <param name="origin">기준 오브젝트</param>
	/// <param name="target">타게 오브젝트</param>
	/// <returns>타겟으로 향하는 오일러각</returns>
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
	/// 각을 0 ~ 360 도 사이로 제한하기 위한 함수
	/// 아직 허점이 있긴하지만 나중에 개선.
	/// </summary>
	/// <param name="angle">정규화할 각</param>
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