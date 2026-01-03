#pragma once

class GUIElement
{
public:
	GUIElement(std::string label, glm::vec2 offset, glm::vec2 size) : mLabel(std::move(label)), mOffset(offset), mSize(size)
	{
	}

	virtual ~GUIElement() = default;

	GUIElement(GUIElement& other) = delete;
	GUIElement& operator=(GUIElement& other) = delete;
	GUIElement(GUIElement&& other) = delete;
	GUIElement& operator=(GUIElement&& other) = delete;

	virtual void Render() = 0;

	const glm::vec2& GetPosition() { return mPosition; }
	void SetPosition(const glm::vec2& newPos) 
	{ 
		mPosition = newPos; 
	}
	const glm::vec2& GetSize() { return mSize; }
	const glm::vec2& GetOffset() { return mOffset; }

	enum Layout { None, Horizontal, Vertical };

	static void RecalcElementPositions(std::vector<std::shared_ptr<GUIElement>>& elements, Layout layout, const glm::vec2& containerPos)
	{
		if (elements.empty())
		{
			return;
		}

		glm::vec2 shift = { 0,0 };
		int i = 0;
		for (auto& element : elements) 
		{
			element->SetPosition(containerPos + shift - (element->GetSize() * 0.5f));
			
			if (layout == Layout::Vertical)
			{
				shift.y += element->GetSize().y + element->GetOffset().y;
			}
			else if (layout == Layout::Horizontal)
			{
				shift.x += element->GetSize().x + element->GetOffset().x;
			}
			else
			{
				shift += element->GetOffset();
			}

			i++;
		}
	}
protected:
	std::string mLabel;
	glm::vec2 mPosition = { 0, 0 };
	glm::vec2 mSize = { 0, 0 };
	glm::vec2 mOffset = { 0,0 };
};

class GUIButton : public GUIElement
{
public:
	GUIButton(std::string label, glm::vec2 offset, glm::vec2 size, LuaUtil::LuaCallback clickCallback)
		: GUIElement(std::move(label), offset, size), mClickCallback(std::move(clickCallback))
	{
	}

	void Render() override
	{
		ImGui::SetCursorPos({ mPosition.x, mPosition.y});
		if (ImGui::Button(mLabel.c_str(), {mSize.x, mSize.y}))
		{
			mClickCallback();
		}
	}

private:
	LuaUtil::LuaCallback mClickCallback;
};

class GUICombo : public GUIElement
{
public:
	GUICombo(std::string label, glm::vec2 offset, glm::vec2 size, std::vector<const char*> items, LuaUtil::LuaCallback onChange)
		: GUIElement(std::move(label), offset, size), mItems(std::move(items)), mOnChangeCallback(std::move(onChange))
	{
	}

	void Render() override
	{
		ImGui::SetCursorPos({ mPosition.x, mPosition.y });
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
	GUILayout(Layout layout, glm::vec2 offset) : GUIElement("", offset, {0,0}), mLayout(layout)
	{
		mPosition = mOffset;
	}

	void AddButton(std::string label, glm::vec2 offset, glm::vec2 size, LuaUtil::LuaCallback clickCallback)
	{
		auto element = std::make_shared<GUIButton>(std::move(label), offset, size, std::move(clickCallback));
		mElements.push_back(std::move(element));

		glm::vec2 calcSize = { 0,0 };
		for (auto& element : mElements) 
		{
			calcSize += element->GetSize();
			calcSize += element->GetOffset();
		}

		mSize = calcSize;

		glm::vec2 pos = mPosition;
		if (mLayout == Layout::Vertical) 
		{
			pos.y -= (mSize.y * 0.5f) - ((mSize.y / mElements.size()) * 0.5f);
		}
		else if (mLayout == Layout::Horizontal) 
		{
			pos.x -= (mSize.x * 0.5f) - ((mSize.x / mElements.size()) * 0.5f);
		}

		RecalcElementPositions(mElements, mLayout, pos);
	}

	void AddCombo(std::string label, glm::vec2 offset, glm::vec2 size, std::vector<const char*> items, LuaUtil::LuaCallback onChange)
	{
		auto element = std::make_shared<GUICombo>(std::move(label), offset, size, std::move(items), std::move(onChange));
		mElements.push_back(std::move(element));

		glm::vec2 calcSize = { 0,0 };
		for (auto& element : mElements)
		{
			calcSize += element->GetSize();
			calcSize += element->GetOffset();
		}

		mSize = calcSize;

		glm::vec2 pos = mPosition;
		if (mLayout == Layout::Vertical)
		{
			pos.y -= (mSize.y * 0.5f) - ((mSize.y / mElements.size()) * 0.5f);
		}
		else if (mLayout == Layout::Horizontal)
		{
			pos.x -= (mSize.x * 0.5f) - ((mSize.x / mElements.size()) * 0.5f);
		}

		RecalcElementPositions(mElements, mLayout, pos);
	}

	void Render() override
	{
		for (const auto& element : mElements)
		{
			element->Render();
		}
	}

	Layout GetLayout() { return mLayout; }

	static void Bind(lua_State* state)
	{
		luaL_newmetatable(state, LuaName);

		LuaUtil::RegisterMethod<GUILayout, &GUILayout::AddButton>(state, "AddButton");
		LuaUtil::RegisterMethod<GUILayout, &GUILayout::AddCombo>(state, "AddCombo");

		lua_pushvalue(state, -1);
		lua_setfield(state, -2, "__index");

		lua_pop(state, 1);
	}

	static constexpr char LuaName[] = "GUILayout";

private:
	Layout mLayout = Layout::None;
	std::vector<std::shared_ptr<GUIElement>> mElements;
};
