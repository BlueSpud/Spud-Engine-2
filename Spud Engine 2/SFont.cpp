//
//  SFont.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/15/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SFont.hpp"

/******************************************************************************
 *  Registration for supported font extensions                                *
 ******************************************************************************/

REGISTER_RESOURCE_CLASS(font, SFont);

/******************************************************************************
 *  Fucntions for font                                                        *
 ******************************************************************************/

SResource* SFont::allocate() { return new SFont(); }

bool SFont::load(const SPath& path) {
    
    // Load in the font
    SFile* font_file = SFileSystem::loadFile(SPath(path.getPathAsString() + "/font.fnt"));
    
    if (font_file) {
        
        // We got the font file now we load up the texture
        font_atlas = (STexture*)SResourceManager::getResource(SPath(path.getPathAsString() + "/atlas.png"));
        if (font_atlas) {
            
            std::string line;
            
            // First read the first line, contains somme information we dont care about
            font_file->getNextTokenWithDeliminator('\n', line);
            
            // Read the second line which we do care about
            font_file->getNextTokenWithDeliminator('\n', line);
            sscanf(line.c_str(), "common lineHeight=%i", &line_height);
            
            while (font_file->getNextTokenWithDeliminator('\n', line)) {
                
                int char_id;
                int x, y;
                int width, height;
                int x_offset, y_offset;
                int x_advance;
                
                sscanf(line.c_str(), "char id=%i      x=%i    y=%i    width=%i    height=%i    xoffset=%i   yoffset=%i    xadvance=%i", &char_id, &x, &y, &width, &height, &x_offset, &y_offset, &x_advance);
                
                // Now we make an actual character object
                SFontCharacter character;
                
                // Convert the pixel data into UV data
                // Position is 1.0 - y - height because given coordinates are from top
                character.position = glm::vec2(x, y + height) / (glm::vec2)font_atlas->size;
                character.position = glm::vec2(character.position.x, 1.0 - character.position.y);
                
                character.size = glm::vec2(width, height) / (glm::vec2)font_atlas->size;
                character.offset = glm::vec2(x_offset, y_offset);
                
                character.x_advance = x_advance;
                
                // Set it into the map based on the character id
                characters[char_id] = character;
                
            }
            
            // We had all of the data that we needed so we can return true
            return true;
            
        }
        
    }
    
    return false;
    
}

void SFont::unload() {
    
    
    
}
