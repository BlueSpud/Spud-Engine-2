//
//  SFont.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/15/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SFont_hpp
#define SFont_hpp

#include "Resource/SResourceManager.hpp"
#include "Resource/Resources/Rendering/STexture.hpp"

// Forward declarations
class STextRenderer;

/******************************************************************************
 *  Definition for character                                                  *
 ******************************************************************************/

struct SFontCharacter {
    
    glm::vec2 offset;
    
    glm::vec2 position;
    glm::vec2 size;
    
    glm::vec2 stide;
    
    int x_advance;
    
};

/******************************************************************************
 *  Definition for font                                                       *
 ******************************************************************************/

class SFont : public SResource {
    
    friend class STextRenderer;

    SRESOURCE(SFont, "font")

    public:
    
        float getLineHeight(float _font_size);
    
    protected:
    
        virtual bool load(const SPath& path);
        virtual void unload();
    
    private:
    
        std::shared_ptr<STexture> font_atlas;
        std::map<char, SFontCharacter> characters;
    
        float font_size;
        int line_height;
    
};

#endif /* SFont_hpp */
