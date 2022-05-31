//precompiled header

///QT ����
#include <QtWidgets>
#include <QString>
#include <QTimer>
#include <QMessageBox>
//////////////////////////////////////////////////////////////////////////


///DX11Engine ����
//////////////////////////////////////////////////////////////////////////
#include "SimpleMath.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace DirectX::SimpleMath;
///Physics ����
//////////////////////////////////////////////////////////////////////////
#include "PxPhysics.h" // PhysX DLL
#include "PxPhysicsAPI.h"
using namespace physx;

///GameEngine ����
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
///SIGNAL, SLOT�� �ҹ��ڷ� �����ϰ�
// intialize();
///�Ϲ� �Լ��� �빮�ڷ� �����Ѵ�.
//Intialize();

