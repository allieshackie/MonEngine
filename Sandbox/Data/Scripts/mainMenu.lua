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

    mainMenu = guiSystem:CreatePopup("Main Menu", {0,0}, {400, 400})
    mainMenu:SetAnchor(2,2) -- Center, Center
    local layout = mainMenu:AddLayout(false) -- false means vertical
    layout:AddButton("Play", {10, 10}, {200, 60}, function() StartGame() end)
    layout:AddButton("Options", {10, 10}, {200, 60}, function() OpenOptions() end)
    layout:AddButton("Exit", {10, 10}, {200, 60}, function() ExitGame() end)
end