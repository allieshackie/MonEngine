local menu = MonUI.CreateMenuPopup("Main Menu", 100, 100, 200, 150)

-- Add buttons to the menu
menu:AddButton("Button 1", 10.0, 45.0, 100.0, 20.0, 
function()
    print("Button 1 clicked!")
end)

menu:AddButton("Button 2", 10.0, 70.0, 100.0, 20.0, 
function()
    print("Button 2 clicked!")
end)

local menuBar = MonUI.CreateMenuBar()

menuBar:AddMenuBarItem("Play", 
    function()
    end
)
