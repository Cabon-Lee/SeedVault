#pragma once


class CollisionCallBack : public physx::PxSimulationEventCallback
{
public:
   
   void onConstraintBreak(physx::PxConstraintInfo* constraints, unsigned int count) override;
   void onWake(physx::PxActor** actors, unsigned int count) override;
   void onSleep(physx::PxActor** actors, unsigned int count) override;
   void onTrigger(physx::PxTriggerPair* pairs, unsigned int count) override;
   void onAdvance(const physx::PxRigidBody* const*, const physx::PxTransform*, const unsigned int) override;

   void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pair, unsigned int pairNum) override;


};

