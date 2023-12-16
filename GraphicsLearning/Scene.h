#pragma once
#ifndef __MY_SCENE_H__
#define __MY_SCENE_H__
#include "SingletonTemplate.h"

class Scene :public Singleton<Scene>
{
	PRIVATE_CONSTRUCTOR_AND_COPY(Scene)
public:
	bool Init();
	void Update(float dt);
	void Render(float dt);
	~Scene();
protected:
private:
};

#define MainScene Scene::Instance()
#endif