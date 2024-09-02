#pragma once
#include "../LogicComponent.h"
#include "../../Event/EventEntity.h"

class BaseItem : public LogicComponent, public EventEntity
{
protected:
	bool use_ = false;

	BaseItem(GameObject* owner);

public:
	void RemoveFromManager() override;

	bool GetUse() const { return use_; }
	//virtual void SetUse(bool) = 0;
};