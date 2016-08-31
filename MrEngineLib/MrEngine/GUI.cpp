//
//  GUI.cpp
//  MisterEngineLib
//
//  Created by Mark Griffith on 2016-08-11.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#include "GUI.hpp"

#include <iostream>

namespace MrEngine {
    
    //static member variables
    MrEngine::Window*  GUI::m_window = NULL;
    int GUI::m_shaderHandle = 0;
    int GUI::m_vertHandle = 0;
    int GUI::m_fragHandle = 0;
    int GUI::m_attribLocationTex = 0, GUI::m_attribLocationProjMtx = 0;
    int GUI::m_attribLocationPosition = 0, GUI::m_attribLocationUV = 0, GUI::m_attribLocationColor = 0;
    unsigned int GUI::m_vboHandle = 0, GUI::m_vaoHandle = 0, GUI::m_elementsHandle = 0;
    bool GUI::m_created = false;
    
    GUI::GUI()
    {
        //empty
    }
    
    GUI::~GUI()
    {
        destroy();
    }
    
    void GUI::init(MrEngine::Window* window, bool isDisplayed)
    {
        //check if the io and system were already initialized
        if (m_created == false)
        {
            ImGuiIO& io = ImGui::GetIO();
            
            // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
            io.KeyMap[ImGuiKey_Tab] = SDLK_TAB;
            io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
            io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
            io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
            io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
            io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
            io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
            io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
            io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
            io.KeyMap[ImGuiKey_Delete] = SDLK_DELETE;
            io.KeyMap[ImGuiKey_Backspace] = SDLK_BACKSPACE;
            io.KeyMap[ImGuiKey_Enter] = SDLK_RETURN;
            io.KeyMap[ImGuiKey_Escape] = SDLK_ESCAPE;
            io.KeyMap[ImGuiKey_A] = SDLK_a;
            io.KeyMap[ImGuiKey_C] = SDLK_c;
            io.KeyMap[ImGuiKey_V] = SDLK_v;
            io.KeyMap[ImGuiKey_X] = SDLK_x;
            io.KeyMap[ImGuiKey_Y] = SDLK_y;
            io.KeyMap[ImGuiKey_Z] = SDLK_z;
            
            // Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.
            
            io.RenderDrawListsFn = &renderDrawLists; ///< render function
            io.SetClipboardTextFn = setClipboardText;
            io.GetClipboardTextFn = getClipboardText;
            
            //initialize time step
            //this will change when newframe is called
            io.DeltaTime = 1.0f / 60.0f;
            
            #ifdef _WIN32
            SDL_SysWMinfo wmInfo;
            SDL_VERSION(&wmInfo.version);
            SDL_GetWindowWMInfo(window->getSDLWindow(), &wmInfo);
            io.ImeWindowHandle = wmInfo.info.win.window;
            #endif
            
            m_window = window;
            
            //initialize window display size
            //this will change when newframe is called to accomodate for window resizing
            io.DisplaySize = ImVec2(window->getScreenWidth(), window->getScreenHeight());
        
            m_created = true;
        }
        displayed = isDisplayed;
    }
    
    void GUI::destroy()
    {
        ///destroy device objects
        if (m_vaoHandle) glDeleteVertexArrays(1, &m_vaoHandle);
        if (m_vboHandle) glDeleteBuffers(1, &m_vboHandle);
        if (m_elementsHandle) glDeleteBuffers(1, &m_elementsHandle);
        m_vaoHandle = m_vboHandle = m_elementsHandle = 0;
        
        glDetachShader(m_shaderHandle, m_vertHandle);
        glDeleteShader(m_vertHandle);
        m_vertHandle = 0;
        
        glDetachShader(m_shaderHandle, m_fragHandle);
        glDeleteShader(m_fragHandle);
        m_fragHandle = 0;
        
        glDeleteProgram(m_shaderHandle);
        m_shaderHandle = 0;
        
        //delete font textures
        if (m_fontTexture)
        {
            glDeleteTextures(1, &m_fontTexture);
            ImGui::GetIO().Fonts->TexID = 0;
            m_fontTexture = 0;
        }
        
        displayed = false;
        
        //shutdown ImGui
        ImGui::Shutdown(); ///< what do?
    }
    
    void GUI::hide()
    {
        displayed = false;
    }
    
    void GUI::display()
    {
        displayed = true;
    }
    
    void GUI::update(MrEngine::InputManager& inputManager, float deltaTime)
    {
        ImGuiIO& io = ImGui::GetIO();
        
        //load font texture
        if (!m_fontTexture)
        {
            createDeviceObjects();
        }
        
        //window draw dimensions
        int display_w, display_h;
        SDL_GL_GetDrawableSize(m_window->getSDLWindow(), &display_w, &display_h);
        io.DisplaySize = ImVec2(m_window->getScreenWidth(), m_window->getScreenHeight());
        io.DisplayFramebufferScale = ImVec2( m_window->getScreenWidth() > 0 ? ((float)display_w / m_window->getScreenWidth()) : 0, m_window->getScreenHeight() > 0 ? ((float)display_h / m_window->getScreenHeight()) : 0);
        
        //update time step
        io.DeltaTime = deltaTime;

        //update the mouse position
        ImVec2 mousePos = ImVec2(inputManager.getMouseCoords().x, inputManager.getMouseCoords().y);
        io.MousePos = mousePos;
        
        int mx, my;
        Uint32 mouseMask = SDL_GetMouseState(&mx, &my);
        //mouse clicks
        io.MouseDown[0] = m_mousePressed[0] || (mouseMask & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
        io.MouseDown[1] = m_mousePressed[1] || (mouseMask & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
        io.MouseDown[2] = m_mousePressed[2] || (mouseMask & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
        m_mousePressed[0] = m_mousePressed[1] = m_mousePressed[2] = false; //unnecessary
        
        ///std::cout << io.MouseDown[0] << "  " << io.MouseDown[1] << "  " << io.MouseDown[2] << std::endl;
        
        //update to new mouse wheel scroll and reset the mouse wheel value
        io.MouseWheel = m_mouseWheel;
        m_mouseWheel = 0.0f;
        // Hide OS mouse cursor if ImGui is drawing it
        SDL_ShowCursor(io.MouseDrawCursor ? 0 : 1);
        
        //start the frame
        ImGui::NewFrame();
    }
    
    void GUI::processEvent(SDL_Event* event)
    {
        
        ImGuiIO& io = ImGui::GetIO();
        switch (event->type)
        {
            case SDL_MOUSEWHEEL:
            {
                if (event->wheel.y > 0)
                {
                    m_mouseWheel = 1;
                }
                if (event->wheel.y < 0)
                    m_mouseWheel = -1;
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                if (event->button.button == SDL_BUTTON_LEFT) m_mousePressed[0] = true;
                if (event->button.button == SDL_BUTTON_RIGHT) m_mousePressed[1] = true;
                if (event->button.button == SDL_BUTTON_MIDDLE) m_mousePressed[2] = true;
            }
            case SDL_TEXTINPUT:
            {
                io.AddInputCharactersUTF8(event->text.text);
            }
            case SDL_KEYDOWN:
            case SDL_KEYUP:
            {
                int key = event->key.keysym.sym & ~SDLK_SCANCODE_MASK;
                io.KeysDown[key] = (event->type == SDL_KEYDOWN);
                io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
                io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
                io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
                io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
            }
        }
    }
    
    void GUI::createDeviceObjects()
    {
        // Backup GL state
        GLint last_texture, last_array_buffer, last_vertex_array;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
        
        const GLchar *vertex_shader =
        "#version 410\n"
        "uniform mat4 ProjMtx;\n"
        "in vec2 Position;\n"
        "in vec2 UV;\n"
        "in vec4 Color;\n"
        "out vec2 Frag_UV;\n"
        "out vec4 Frag_Color;\n"
        "void main()\n"
        "{\n"
        "	Frag_UV = UV;\n"
        "	Frag_Color = Color;\n"
        "	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
        "}\n";
        
        const GLchar* fragment_shader =
        "#version 410\n"
        "uniform sampler2D Texture;\n"
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "out vec4 Out_Color;\n"
        "void main()\n"
        "{\n"
        "	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
        "}\n";
        
        m_shaderHandle = glCreateProgram();
        m_vertHandle = glCreateShader(GL_VERTEX_SHADER);
        m_fragHandle = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(m_vertHandle, 1, &vertex_shader, 0);
        glShaderSource(m_fragHandle, 1, &fragment_shader, 0);
        glCompileShader(m_vertHandle);
        glCompileShader(m_fragHandle);
        glAttachShader(m_shaderHandle, m_vertHandle);
        glAttachShader(m_shaderHandle, m_fragHandle);
        glLinkProgram(m_shaderHandle);
        
        m_attribLocationTex = glGetUniformLocation(m_shaderHandle, "Texture");
        m_attribLocationProjMtx = glGetUniformLocation(m_shaderHandle, "ProjMtx");
        m_attribLocationPosition = glGetAttribLocation(m_shaderHandle, "Position");
        m_attribLocationUV = glGetAttribLocation(m_shaderHandle, "UV");
        m_attribLocationColor = glGetAttribLocation(m_shaderHandle, "Color");
        
        glGenBuffers(1, &m_vboHandle);
        glGenBuffers(1, &m_elementsHandle);
        
        glGenVertexArrays(1, &m_vaoHandle);
        glBindVertexArray(m_vaoHandle);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboHandle);
        glEnableVertexAttribArray(m_attribLocationPosition);
        glEnableVertexAttribArray(m_attribLocationUV);
        glEnableVertexAttribArray(m_attribLocationColor);
        
#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
        glVertexAttribPointer(m_attribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
        glVertexAttribPointer(m_attribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
        glVertexAttribPointer(m_attribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF
        
        createFontsTexture();
        
        // Restore modified GL state
        glBindTexture(GL_TEXTURE_2D, last_texture);
        glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
        glBindVertexArray(last_vertex_array);
    }
    
    void GUI::loadFont(const char* fontFile, float size)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF(fontFile, size);
        m_numFonts++;
//        ImFont* font = io.Fonts->Fonts[m_numFonts];
//        font = io.Fonts->Fonts[0];
//        io.Fonts->Fonts[0] = font;
    }

    void GUI::createFontsTexture()
    {
        ImGuiIO& io = ImGui::GetIO();
        
        // Build texture atlas
        unsigned char* pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.
        
        // Upload texture to graphics system
        GLint last_texture;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
        glGenTextures(1, &m_fontTexture );
        glBindTexture(GL_TEXTURE_2D, m_fontTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        
        // Store our identifier
        io.Fonts->TexID = (void *)(intptr_t)m_fontTexture;
        
        // Restore state
        glBindTexture(GL_TEXTURE_2D, last_texture);
    }
    
    void GUI::renderDrawLists(ImDrawData* draw_data)
    {
        ImGuiIO& io = ImGui::GetIO();
        
        // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
        int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x); /// not set
        int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
        if (fb_width == 0 || fb_height == 0)
            return;
        draw_data->ScaleClipRects(io.DisplayFramebufferScale);
        
        // Backup GL state
        GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
        GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
        GLint last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, &last_active_texture);
        GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
        GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
        GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
        GLint last_blend_src; glGetIntegerv(GL_BLEND_SRC, &last_blend_src);
        GLint last_blend_dst; glGetIntegerv(GL_BLEND_DST, &last_blend_dst);
        GLint last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
        GLint last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
        GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
        GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
        GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
        GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
        GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);
        
        // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_SCISSOR_TEST);
        glActiveTexture(GL_TEXTURE0);
        
        // Setup viewport, orthographic projection matrix
        glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
        const float ortho_projection[4][4] =
        {
            { 2.0f/io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
            { 0.0f,                  2.0f/-io.DisplaySize.y, 0.0f, 0.0f },
            { 0.0f,                  0.0f,                  -1.0f, 0.0f },
            {-1.0f,                  1.0f,                   0.0f, 1.0f },
        };
        glUseProgram(m_shaderHandle); //don't need
        glUniform1i(m_attribLocationTex, 0); //don't need
        glUniformMatrix4fv(m_attribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]); //don't need
        
        
        glBindVertexArray(m_vaoHandle);
        
        for (int n = 0; n < draw_data->CmdListsCount; n++)
        {
            const ImDrawList* cmd_list = draw_data->CmdLists[n];
            const ImDrawIdx* idx_buffer_offset = 0;
            
            glBindBuffer(GL_ARRAY_BUFFER, m_vboHandle);
            glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.size() * sizeof(ImDrawVert), (GLvoid*)&cmd_list->VtxBuffer.front(), GL_STREAM_DRAW);
            
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementsHandle);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx), (GLvoid*)&cmd_list->IdxBuffer.front(), GL_STREAM_DRAW);
            
            for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++)
            {
                if (pcmd->UserCallback)
                {
                    pcmd->UserCallback(cmd_list, pcmd);
                }
                else
                {
                    glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
                    glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
                    glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
                }
                idx_buffer_offset += pcmd->ElemCount;
            }
        }
        
        // Restore modified GL state
        glUseProgram(last_program);
        glActiveTexture(last_active_texture);
        glBindTexture(GL_TEXTURE_2D, last_texture);
        glBindVertexArray(last_vertex_array);
        glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
        glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
        glBlendFunc(last_blend_src, last_blend_dst);
        if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
        if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
        if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
        if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
        glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
    }
    
}

