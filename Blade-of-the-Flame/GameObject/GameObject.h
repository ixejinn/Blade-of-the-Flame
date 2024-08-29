#pragma once
#include <map>
#include <typeindex>	// std::type_index
#include <string>
#include "AEVec2.h"

class Component;

/**
* @brief	Transform ������Ʈ ���� �ʼ�
*			GameObjectManager�� ������ ������ ����
*/
class GameObject
{
private:
	std::map<std::type_index, Component*> components_;

	std::string name_;

	// GameObjectManager ���� ���� �Ұ�
	GameObject(const std::string& name) : components_(), name_(name) {}

public:
	bool active_ = true;

	~GameObject();

	const std::string& GetName() const { return name_; }
	const std::map<std::type_index, Component*>& GetAllComponents() const { return components_; }

	void SetName(const std::string& name) { name_ = name; }

	// Component ���� + GameObject�� ���� �ѹ��� ����
	// @tparam T	Transform, Sprite, ...
	template <typename T>
	bool AddComponent();

	void DeleteComponent(std::type_index type);

	// @tparam T	Transform, Sprite, ...
	template <typename T>
	T* GetComponent();

	friend class GameObjectManager;
};

#include "GameObject.inl"