#pragma once
#include "PhysicsGeometry.h"



class RigidDynamic 
{
public:
   RigidDynamic();
   ~RigidDynamic();

private:
   physx::PxRigidDynamic* m_pRigidDynamic;


};

