//precompiled header

///QT 관련
#include <QtWidgets>
#include <QString>
#include <QTimer>
#include <QMessageBox>
//////////////////////////////////////////////////////////////////////////


///DX11Engine 관련
//////////////////////////////////////////////////////////////////////////
#include "SimpleMath.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace DirectX::SimpleMath;
///Physics 관련
//////////////////////////////////////////////////////////////////////////
#include "PxPhysics.h" // PhysX DLL
#include "PxPhysicsAPI.h"
using namespace physx;

///GameEngine 관련
//////////////////////////////////////////////////////////////////////////
#include <boost/describe.hpp>
#include <boost/mp11.hpp>
#include <json/json.h>

#include "IdDll.h"

#include "CATrace.h"
#include "EngineDLL.h"
#include <memory>


#include "BoostDescribeAssist.h"
#include "JsonReflectionHelper.h"

#include "Collision.h"

#include "ComponentBase.h"
#include "GameObject.h"

#include "Settings.h"

#include "QStringHelper.h"

//////////////////////////////////////////////////////////////////////////
///SIGNAL, SLOT은 소문자로 시작하고
// intialize();
///일반 함수는 대문자로 시작한다.
//Intialize();

