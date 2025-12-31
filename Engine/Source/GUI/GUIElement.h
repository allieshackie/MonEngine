#pragma once

class GUIElement
{
public:
	GUIElement(std::string label) : mLabel(std::move(label))
	{
	}

	virtual ~GUIElement() = default;

	GUIElement(GUIElement& other) = delete;
	GUIElement& operator=(GUIElement& other) = delete;
	GUIElement(GUIElement&& other) = delete;
	GUIElement& operator=(GUIElement&& other) = delete;

	virtual void Render() = 0;
protected:
	std::string mLabel;
};

class GUIButton : public GUIElement
{
public:
	GUIButton(std::string label, glm::vec2 pos, glm::vec2 size, LuaUtil::LuaCallback clickCallback)
		: GUIElement(std::move(label)), mPosition(pos), mSize(size), mClickCallback(std::move(clickCallback))
	{
	}

	void Render() override
	{
		ImGui::SetCursorPos({mPosition.x, mPosition.y});
		if (ImGui::Button(mLabel.c_str(), {mSize.x, mSize.y}))
		{
			mClickCallback();
		}
	}

private:
	glm::vec2 mPosition = {0, 0};
	glm::vec2 mSize = {0, 0};

	LuaUtil::LuaCallback mClickCallback;
};

class GUICombo : public GUIElement
{
public:
	GUICombo(std::string label, std::vector<const char*> items, LuaUtil::LuaCallback onChange)
		: GUIElement(std::move(label)), mItems(std::move(items)), mOnChangeCallback(std::move(onChange))
	{
	}

	void Render() override
	{
		if (ImGui::Combo(mLabel.c_str(), &mCurrentItem, mItems.data(), static_cast<int>(mItems.size())))
		{
			mOnChangeCallback();
		}
	}

	int GetCurrentItem() const
	{
		return mCurrentItem;
	}

private:
	int mCurrentItem = 0;

	std::vector<const char*> mItems;
	LuaUtil::LuaCallback mOnChangeCallback;
};

class GUILayout : public GUIElement
{
public:
	enum Layout { None, Horizontal, Vertical };

	GUILayout(Layout layout) : GUIElement(""), mLayout(layout)
	{
	}

	void AddButton(std::string label, glm::vec2 offset, glm::vec2 size, LuaUtil::LuaCallback clickCallback)
	{
		auto element = std::make_unique<GUIButton>(std::move(label), offset, size, std::move(clickCallback));
		mElements.push_back(std::move(element));
	}

	void AddCombo(std::string label, std::vector<const char*> items, LuaUtil::LuaCallback onChange)
	{
		auto element = std::make_unique<GUICombo>(std::move(label), std::move(items), std::move(onChange));
		mElements.push_back(std::move(element));
	}

	void Render() override
	{
		for (const auto& element : mElements)
		{
			element->Render();
		}
	}

private:
	Layout mLayout = Layout::None;
	std::vector<std::unique_ptr<GUIElement>> mElements;
};
