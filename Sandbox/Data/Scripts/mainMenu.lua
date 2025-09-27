local function StartGame()
    gEngineContext:LoadLevel("game.json")
    print("Start Game")
end

local function OpenOptions()
    print("Open options")
end

local function ExitGame()
    print("Exit Game")
end

function Initialize()
    local levelsMenu = MonUI.CreateMenuPopup("Main Menu", 100, 100, 250, 300)

    levelsMenu:AddButton("Play", 50.0, 50.0, 125.0, 40.0, StartGame)
    levelsMenu:AddButton("Options", 50.0, 100.0, 125.0, 40.0, OpenOptions)
    levelsMenu:AddButton("Exit", 50.0, 150.0, 125.0, 40.0, ExitGame)
end