#pragma once
#include "ComponentBase.h"

class PhysicsSnippetTest : public ComponentBase
{
public:
   PhysicsSnippetTest();
   virtual ~PhysicsSnippetTest();

   // ComponentBase Override
   virtual void Start() override;
   virtual void Update(float dTime) override;
   virtual void OnRender() override;


};
