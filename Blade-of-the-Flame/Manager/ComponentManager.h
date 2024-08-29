#pragma once
#include <list>
#include <memory>

class GameObject;

/**
* @brief	ComponentManager�� Component�� ������ �������� �����ϹǷ� GameObject�� Component ������ ���� �ؾ� ��
* @tparam T	EngineComponent, GraphicsComponent, ...
*/
template <typename T>
class ComponentManager
{
private:
	std::list<std::unique_ptr<T>> components_;

	ComponentManager() = default;
	~ComponentManager() = default;

	ComponentManager(const ComponentManager&) = delete;
	ComponentManager& operator =(const ComponentManager&) = delete;
	ComponentManager(ComponentManager&&) = delete;
	ComponentManager& operator =(ComponentManager&&) = delete;

public:
	static ComponentManager& GetInstance()
	{
		static ComponentManager instance;
		return instance;
	}

	// @brief	GameObject::AddComponent()�� Component ���� + GameObject�� ������ �ѹ��� �����ϹǷ� ��õ��
	// @tparam U	Transform, Sprite, ...
	template <typename U>
	U* CreateComponent(GameObject* owner);

	void UpdateComponent();
	void DeleteComponent(T* const comp);

	void Clear();
};

#include "ComponentManager.inl"