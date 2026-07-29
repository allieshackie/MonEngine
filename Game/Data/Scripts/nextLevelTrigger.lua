function Initialize()
    _G.print("Initializing Next Level Trigger")
end

function OnTriggerEnter()
    _G.print("OnTriggerEnter called")
    local sceneManager = gGameData.GetSceneManager()
    sceneManager:LoadScene("level2.json")
end

function OnTriggerExit()
    _G.print("OnTriggerExit called")
end