#include "Text.h"

#include <typeindex>
#include "../GameObject/GameObject.h"
#include "../Manager/ResourceManager.h"
#include "../Resource/FontResource.h"

Text::Text(GameObject* owner) : GraphicsComponent(owner), string_(), fontName_() {}

Text::~Text()
{
	if (fontId_ < 0)
		ResourceManager::GetInstance().Unload(fontName_);
}

void Text::Update()
{
	if (fontId_ < 0)
		std::cerr << "Text::Update() " << owner_->GetName() << "'s font ID not found" << std::endl;
	else
	{
		AEGfxSetBlendMode(AE_GFX_BM_BLEND);

		AEGfxPrint(fontId_, string_.c_str(), position_.x, position_.y, size_, color_.red, color_.green, color_.blue, 1.0f);
	}
}

void Text::LoadFromJson(const json&)
{
}

json Text::SaveToJson()
{
	return json();
}

void Text::OnEvent(BaseEvent* event)
{
	if (std::type_index(typeid(*event)) == std::type_index(typeid(ScoreEvent)))
	{
		Score* score = owner_->GetComponent<Score>();
		if (!score)
		{
			std::cerr << "Text::OnEvent() Score not found" << std::endl;
			return;
		}

		string_ = std::to_string(score->GetScore());
	}
}

void Text::SetFont(const std::string& name)
{
	fontName_ = name;
	fontId_ = ResourceManager::GetInstance().Get<FontResource>(name)->GetData();
}

ComponentSerializer* Text::CreateComponent(GameObject* owner)
{
	if (!owner->AddComponent<Text>())
		std::cout << "Text::CreateComponent() Component already exists" << std::endl;

	return owner->GetComponent<Text>();
}
