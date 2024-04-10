#pragma once
#ifndef __GAME_COMPONENT_H__
#define __GAME_COMPONENT_H__

//定义的一些组件类型枚举
typedef enum class emComponentType
{
	ComType_None,//还没有确定的缺省类型
	ComType_Transform,//变换组件
	ComType_MeshRender,//模型渲染组件
	ComType_Camera,//摄像机组件
	ComType_Light,//灯光组件
	ComType_Terrain,//地形组件
	ComType_SkyBox,
	//...
}COMTYPE,*LPCOMTYPE;

//前置声明gameobject类
class GameObject;

typedef class Component
{
public:
	virtual ~Component() = default;
	Component(const Component& that) = delete;
	Component& operator = (const Component& that) = delete;

	Component(GameObject* pObject)
	{
		mType = COMTYPE::ComType_None;//现在还不知道自己是什么类型，应该后续在子类的构造函数中指定具体的类型
		mOwner = pObject;
	}

	//获取自己的类型
	COMTYPE Type() { return mType; }

	//获取所属的gameobject
	GameObject* gameobject() { return mOwner; }
protected:
	GameObject* mOwner;//该组件的持有者，也就是这个组件在哪一个游戏对象上
	COMTYPE mType;//组件的类型
}COMPONENT,*LPCOMPONENT;

#endif