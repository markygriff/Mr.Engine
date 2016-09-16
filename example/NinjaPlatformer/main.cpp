
/*****************************************
 
 Demo of how to use Mr.Engine, custom 2D game engine
 1. Create a main 'App' class which inherits from
    Mr.Engine's IMainGame class
 2. Run the app. SIMPLE :)
 
 ****************************************/
#include "App.hpp"

#include <IMainGame.hpp>
#include <ScreenList.hpp>

int main(int argc, char** argv) {
    
    App app;
    app.run();
    
    return 0;
}
