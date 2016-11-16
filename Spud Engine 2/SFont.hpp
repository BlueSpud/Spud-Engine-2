//
//  SFont.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/15/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SFont_hpp
#define SFont_hpp

#include "SResourceManager.hpp"
#include "STexture.hpp"

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
    
    public:
    
        static SResource* allocate();
        
    
    protected:
    
        virtual bool load(const SPath& path);
        virtual void unload();
    
    private:
    
        STexture* font_atlas;
        std::map<char, SFontCharacter> characters;
    
        float font_size;
        int line_height;
    
};

#endif /* SFont_hpp */
