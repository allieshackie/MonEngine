#pragma once
#include <glm/vec2.hpp>

#include "LLGL/Buffer.h"

class TextPipeline;

struct TextItem
{
	TextItem(std::string text, glm::vec2 pos, glm::vec2 size, int id)
		: mText(std::move(text)), mPos(pos), mSize(size), mId(id)
	{
	}

	bool operator==(const TextItem& text) const
	{
		if (mText == text.mText && mPos == text.mPos)
		{
			return true;
		}
		return false;
	}

	std::string mText;
	glm::vec2 mPos = {0, 0};
	glm::vec2 mSize = {0, 0};

	int mId;
};

class UISystem
{
public:
	UISystem(TextPipeline& textPipeline);
	~UISystem() = default;

	void AddText(std::string text, glm::vec2 pos, glm::vec2 size);
	void RemoveText(const TextItem& item);

	const std::vector<TextItem>& GetTextItems();

private:
	std::vector<TextItem> mTextItems;
	TextPipeline& mTextPipeline;

	inline static int texCount = 0;
};
