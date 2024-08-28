#pragma once
#include "State.h"

class GameObject;

class SampleSave : public State
{
private:
	GameObject* obj;

public:
	void Init() override;
	void Update() override;
	void Exit() override;
};