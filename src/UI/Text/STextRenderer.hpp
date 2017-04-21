//
//  STextRenderer.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/15/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef STextRenderer_hpp
#define STextRenderer_hpp

#include "Engine/SSubsystem.hpp"
#include "SFont.hpp"
#include "Resource/Resources/Rendering/Shader/SShader.hpp"

#define CURSOR_WIDTH 1.0f

/******************************************************************************
 *  Definition of character draw call for centered text                       *
 ******************************************************************************/

struct SCharacterDrawCall {
    
    char character;
    glm::vec2 position;
    
};

/******************************************************************************
 *  Definition for text renderer                                              *
 ******************************************************************************/

class STextRenderer : public SSubsystem {
    
    public:
    
        static void startup();
        static void shutdown();
    
        static void renderText(std::string text, std::shared_ptr<SFont> font, float font_size, glm::vec2 screen_pos);
        static void renderTextCentered(std::string text, std::shared_ptr<SFont> font, float font_size, glm::vec2 screen_pos, float width);
        static void renderTextWithCursor(std::string text, int cursor_pos, std::shared_ptr<SFont> font, float font_size, glm::vec2 screen_pos);
    
    private:
    
        static void renderCursor(std::shared_ptr<SFont> font, float font_size_multiplier, const glm::vec2& position);
        static void renderCenteredTextLine(std::vector<SCharacterDrawCall>& character_draw_calls, std::shared_ptr<SFont> font,
                                           float font_size_multiplier, float& line_length, float width);
    
        static std::shared_ptr<SFont> default_font;
        static std::shared_ptr<SShader> text_shader;
    
};

#endif /* STextRenderer_hpp */
