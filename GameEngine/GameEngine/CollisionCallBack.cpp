#include "pch.h"
#include "CollisionCallBack.h"



void CollisionCallBack::onConstraintBreak(physx::PxConstraintInfo* constraints, unsigned int count)
{
   PX_UNUSED(constraints); PX_UNUSED(count); 
}

void CollisionCallBack::onWake(physx::PxActor** actors, unsigned int count)
{
   PX_UNUSED(actors); PX_UNUSED(count);
}

void CollisionCallBack::onSleep(physx::PxActor** actors, unsigned int count)
{
   PX_UNUSED(actors); PX_UNUSED(count);
}

void CollisionCallBack::onTrigger(physx::PxTriggerPair* pairs, unsigned int count)
{
   PX_UNUSED(pairs); PX_UNUSED(count);
}
void CollisionCallBack::onAdvance(const physx::PxRigidBody* const*, const physx::PxTransform*, const unsigned int)
{

}

void CollisionCallBack::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, unsigned int pairNum)
{
   std::vector<physx::PxContactPairPoint> contactPoints;

   for (unsigned int i = 0; i < pairNum; i++)
   {
      unsigned int contactCount = pairs[i].contactCount;

      if (contactCount)
      {
         contactPoints.resize(contactCount);


         for (unsigned int j = 0; j < contactCount; j++)
         {
            //gContactPositions.push_back(contactPoints[j].position);
            //Push back reported contact impulses
            //gContactImpulses.push_back(contactPoints[j].impulse);

            //Compute the effective linear/angular impulses for each body.
            //Note that the local mass scaling permits separate scales for invMass and invInertia.
            for (unsigned int k = 0; k < 2; ++k)
            {
               const physx::PxRigidDynamic* dynamic = pairHeader.actors[k]->is<physx::PxRigidDynamic>();

            }
         }
      }
   }
}
