// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H

// 여기에 미리 컴파일하려는 헤더 추가
#include "framework.h"

/// Trace (직접)출력창에 메시지 출력
#include "CATrace.h"



#pragma region DirectX
// library
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxgi.lib")

// DX Header
#include <d3d11.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include "SimpleMath.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace DirectX::SimpleMath;
#pragma endregion DirectX
////////////////////////////////////////////////

#pragma region GameEngine
// physx
#include "PxPhysics.h" // PhysX DLL
#include "PxPhysicsAPI.h"
using namespace physx;

//boost, json save
#include "BoostDescribeAssist.h"
#include "JsonReflectionHelper.h"

// Dll Header
#include "EngineDLL.h"

// Object
#include "GameObject.h"
#include "ObjectManager.h"

// Component
#include "Transform.h"
#include "MeshFilter.h"
#include "MaterialComponent.h"
#include "MeshRenderer.h"
#include "SkinningAnimation.h"
#include "Light.h"
#include "AnimationSystem.h"
#include "Animation.h"
#include "PhysicsActor.h"
#include "Collision.h"
#include "NaviMeshController.h"
#include "ParticleSystem.h"

#include "PlaneAnimation.h"

#include "Text.h"
#include "Billboard.h"
#include "Sprite2D.h"
#include "button.h"
#include "CSVLoader.h"

#include "TransformAnimation.h"

#include "Audio.h"
#include "AudioListener.h"
#include "SoundEvent.h"


// Render
#include "ReflectionProbeCamera.h"

#include "EffectUIEnum.h"

// Helper
#include "StringHelper.h"
#include "MathHelper.h"
#pragma endregion GameEngine
////////////////////////////////////////////////


#pragma region Client
#include <time.h>

// STL
#include <vector>
#include <string>
#include <algorithm>

#include <math.h>

#define SAFE_DELETE(p)  if(p) { delete p; p = nullptr; }

using uint = unsigned int;
using ulong = unsigned long;

#include "CLInput.h"
#include "Settings.h"
#include "ResourcesPathDef.h"
#include "IngameUIManager.h"
#include "PauseUIManager.h"

#include "UtilityFunction.h"
#pragma endregion Client
//////////////////////////////

#endif //PCH_H
