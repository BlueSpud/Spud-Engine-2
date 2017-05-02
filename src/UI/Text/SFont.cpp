//
//  SFont.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/15/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SFont.hpp"

/******************************************************************************
 *  Fucntions for font                                                        *
 ******************************************************************************/

float SFont::getLineHeight(float _font_size) { return ((float)line_height / font_size * _font_size) / SGL::getScreenScale(); }

bool SFont::load(const SPath& path) {
    
    // Load in the font
    SFile* font_file = SFileSystem::loadFile(SPath(path.getPathAsString() + "/font.fnt"));
    
    if (font_file) {
        
        // We got the font file now we load up the texture
        font_atlas = SResourceManager::getResource<STexture>(SPath(path.getPathAsString() + "/atlas.dds"));
        if (font_atlas) {
            
            std::string line;
            
            // Read the first line so that we can find the font size IN PIXELS
            font_file->getNextTokenWithDeliminator('\n', line);
            int size_position = (int)line.find("size=");
            
            // Get the point size
            int font_size_pixels;
            sscanf(line.substr(size_position, line.length() - size_position).c_str(), "size=%i", &font_size_pixels);
            font_size = (float)font_size_pixels / SGL::getScreenScale();
            
            // Find where the padding is
            int padding_position = (int)line.find("padding=");
            
            // We assume the padding was square for simplicity and we accoutn for a little extra
            int padding;
            sscanf(line.substr(padding_position, line.length() - padding_position).c_str(), "padding=%i", &padding);
            padding = padding * 1.8;
            
            // Read the second line which we do care about
            font_file->getNextTokenWithDeliminator('\n', line);
            sscanf(line.c_str(), "common lineHeight=%i", &line_height);
            line_height = line_height - padding;
            
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
                character.position = glm::vec2(character.position.x, character.position.y);
                
                character.size = glm::vec2(width, -height) / (glm::vec2)font_atlas->size;
                character.offset = glm::vec2(x_offset, y_offset);
                
                character.x_advance = x_advance - padding;
                
                // Set it into the map based on the character id
                characters[char_id] = character;
                
            }
            
            // We had all of the data that we needed so we can return true
            return true;
            
        }
        
    }
    
    return false;
    
}

void SFont::unload() {}
