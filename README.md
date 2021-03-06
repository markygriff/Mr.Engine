## Mark Griffith.

# Overview
This is a very basic 2D game engine I have created using C/C++, OpenGL and SDL2.
It has served mainly as a playground for me to explore engine development as I am fascinated with it!
Mr.Engine includes classes for:
  - Custom SDL Window creation (Window)
  - Loading png sprites and rendering them using vertex/fragment shaders (GLSLProgram)
  - Creating sprite batches for efficient sprite rendering (SpriteBatch)
  - Rendering fonts (SpriteFont)
  - Handling user input (InputManager)
  - Controlling the game world camera (Camera2D)
  - Calculating and limiting the fps (Timing)
  - Controlling the main game using a game interface (IMainGame, IGameScreen, ScreenList)
  - Simple debug rendering for outlining simple shapes (DebugRenderer)
  - Easy to set up and use User Interfaces using ImGui --> https://github.com/ocornut/imgui (GUI)
    NOTE: I would rather create my own GUI, but I needed a quick one for testing.
  - Basic Spritesheet handling, however it is really only useful for instances where the
    the spritesheet is completely uniform (SpriteSheet)

# TODO
(functionality I plan on working on to further my knowledge of engine design)
  - Crossplatform development (this won't be a huge concern until I have adequetly learnt enough
    to begin developing an engine for release)
  - Implement my own physics engine (currently I am using the Box2D physics engine)
  - Implement my own GUI
  - Upgrade window creation to account for all systems as well as resizing
  - Better spritesheet handling system to handle uneven spritesheets
  - Continue learning about game dev:)

# Build
BUILDING (OSX only) :
  - Open MisterEngineLib.xcodeproj
  - Build this project in both Debug and Release mode
  - Test by running NinjaPlatformer.xcodeproj in release mode
  
# Usage  
 How to use Mr.Engine, custom 2D game engine
 1. Create a main 'App' class which inherits from
    Mr.Engine's IMainGame class.
 2. Run the app. SIMPLE :)
 
 A sample game (example) is provided which uses Mr.Engine to create a mini game and a level editor
  - Note: the level editor is unfinished, but has the ability to save a level and load it. 
  - todo: save and load game data using binary rather than text format.


