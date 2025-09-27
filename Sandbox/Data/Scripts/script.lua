-- Testing!

local mainMenuBar = MonUI.CreateMenuBar()
local levelNames = gEngineContext:GetLevelNames()
local currentLevel = 1
local combo = nil

local function UpdateCurrentLevel()
    if combo then
        -- Add 1 since Lua starts from index 1 instead of 0 like C++
        currentLevel = combo:GetCurrentItem() + 1
    end
end

local function OpenLevel()
    print(levelNames[currentLevel])
    --gEngineContext:LoadLevel(currentLevel)
end

local function OpenPlayMenu()
    local levelsMenu = MonUI.CreateMenuPopup("Load Level", 100, 100, 300, 300)
    levelsMenu:SetNoMoveFlag(false)

    combo = levelsMenu:AddCombo("Levels", levelNames, UpdateCurrentLevel)
    -- Add buttons to the menu
    levelsMenu:AddButton("Open", 10.0, 100.0, 100.0, 20.0, OpenLevel)
end

function Initialize()
    mainMenuBar:AddMenuBarItem("Play", OpenPlayMenu)
end