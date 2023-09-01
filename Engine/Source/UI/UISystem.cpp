#include "Graphics/TextPipeline.h"

#include "UISystem.h"

UISystem::UISystem(TextPipeline& textPipeline) : mTextPipeline(textPipeline)
{
}

void UISystem::AddText(std::string text, glm::vec2 pos, glm::vec2 size)
{
	mTextPipeline.CreateTextMesh(text, pos, size);
	mTextItems.emplace_back(std::move(text), pos, size, texCount);
	texCount++;
}

void UISystem::RemoveText(const TextItem& item)
{
	for (const auto& it : mTextItems)
	{
		if (it == item)
		{
			mTextItems.erase(std::remove(mTextItems.begin(), mTextItems.end(), item), mTextItems.end());
		}
	}
}

const std::vector<TextItem>& UISystem::GetTextItems()
{
	return mTextItems;
}
