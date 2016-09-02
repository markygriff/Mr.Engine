//
//  App.hpp
//  NinjaPlatformer
//
//  Created by Mark Griffith on 2016-08-07.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#ifndef App_hpp
#define App_hpp

#include "GameplayScreen.hpp"
#include "MainMenuScreen.hpp"
#include "EditorScreen.hpp"

#include <IMainGame.hpp>

#include <stdio.h>

class App : public MrEngine::IMainGame
{
public:
    App();
    ~App();
    
    /// Customs
    virtual void onInit() override;
    virtual void addScreens() override;
    virtual void onExit() override;
    
private:
    std::unique_ptr<MainMenuScreen> m_mainMenuScreen = nullptr;
    std::unique_ptr<GameplayScreen> m_gameplayScreen = nullptr;
    std::unique_ptr<EditorScreen> m_editorScreen = nullptr;
};

#endif /* App_hpp */
