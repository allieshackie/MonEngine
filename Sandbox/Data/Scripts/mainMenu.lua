local mainMenu = nil

local function StartGame()
    _G.print("Start Game")
    mainMenu:Close()
    local sceneManager = gGameData.GetSceneManager()
    sceneManager:LoadScene("game.json")
end

local function OpenOptions()
    _G.print("Open options")
end

local function ExitGame()
    _G.print("Exit Game")
end

function Initialize()
    local guiSystem = gGameData.GetGUISystem()

    mainMenu = guiSystem:CreatePopup("Main Menu", {20,20}, {200, 200})
    mainMenu:AddButton("Play", {10, 10}, {200, 60}, function() StartGame() end)
end