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
	GUIButton(std::string label, glm::vec2 pos, glm::vec2 size, std::function<void()> clickCallback)
		: GUIElement(std::move(label)), mPosition(pos), mSize(size), mClickCallback(std::move(clickCallback))
	{
	}

	void Render() override
	{
		ImGui::SetCursorPos({mPosition.x, mPosition.y});
		if (ImGui::Button(mLabel.c_str(), {mSize.x, mSize.y}))
		{
			if (mClickCallback)
			{
				mClickCallback();
			}
		}
	}

private:
	glm::vec2 mPosition = {0, 0};
	glm::vec2 mSize = {0, 0};

	std::function<void()> mClickCallback = nullptr;
};

class GUICombo : public GUIElement
{
public:
	GUICombo(std::string label, std::vector<const char*> items, std::function<void()> onChange)
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
	std::function<void()> mOnChangeCallback = nullptr;
};
