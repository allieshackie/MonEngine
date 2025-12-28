function Initialize()
    print("LoadScene")
    local sceneManager = gGameData.GetSceneManager()
    sceneManager:LoadScene("game.json")
end