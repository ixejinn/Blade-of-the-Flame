#pragma once
#include <list>
#include <string>
#include "GameObjectManager.h"
#include "../Component/LogicComponents/Particle.h"

class SkillManager
{
	static SkillManager* ptr;
	std::list<std::pair<std::string, std::vector<GameObject*>>> skills;
	SkillManager();
	~SkillManager();
public:
	void SetSkill(std::string which);
	BaseAttack* curAttack_ = nullptr;
	static SkillManager* getPtr();
	static void Delete();
};