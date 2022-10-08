#pragma once
#include "opengl/OGLCore.h"
#ifndef app
#define app
class Application
{
public:
	Application() = default;
    ~Application();
public:
    virtual void Initialize();
   virtual void Update(float inDeltaTime);
     virtual void Render(float inAspectRatio);
    virtual void Shutdown();
};
#endif
