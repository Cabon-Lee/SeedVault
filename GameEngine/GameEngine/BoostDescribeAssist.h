#pragma once

#include "SimpleMath.h"

#include <boost/describe.hpp>
#include <boost/mp11.hpp>


/// <summary>
/// namespace 내부에 존재하는 class, struct는 해당 namespace내에서 Boost에 등록한다.
/// </summary>
/// 
namespace DirectX::SimpleMath
{
	BOOST_DESCRIBE_STRUCT(Matrix,	(XMFLOAT4X4), ())
	BOOST_DESCRIBE_STRUCT(Vector2,	(XMFLOAT2), ())
	BOOST_DESCRIBE_STRUCT(Vector3,	(XMFLOAT3), ())
	BOOST_DESCRIBE_STRUCT(Quaternion,	(XMFLOAT4), ())
	BOOST_DESCRIBE_STRUCT(Vector4,		(XMFLOAT4), ())
	BOOST_DESCRIBE_STRUCT(Color,		(XMFLOAT4), ())
}

namespace DirectX
{
	BOOST_DESCRIBE_STRUCT(XMFLOAT2, (), (x, y))
	BOOST_DESCRIBE_STRUCT(XMFLOAT3, (), (x, y, z))
	BOOST_DESCRIBE_STRUCT(XMFLOAT4, (), (x, y, z, w))
	BOOST_DESCRIBE_STRUCT(XMFLOAT3X3, (), (
		_11, _12, _13,
		_21, _22, _23,
		_31, _32, _33
		))
	BOOST_DESCRIBE_STRUCT(XMFLOAT4X4, (), (
		_11, _12, _13, _14,
		_21, _22, _23, _24,
		_31, _32, _33, _34,
		_41, _42, _43, _44
		))
	BOOST_DESCRIBE_STRUCT(XMFLOAT3X4, (), (
		_11, _12, _13, _14,
		_21, _22, _23, _24,
		_31, _32, _33, _34
		))
}
