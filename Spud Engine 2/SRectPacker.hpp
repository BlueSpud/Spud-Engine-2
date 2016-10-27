//
//  SRectPacker.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 10/27/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SRectPacker_hpp
#define SRectPacker_hpp

#include <glm/glm.hpp>

/******************************************************************************
 *  Definition for rect node                                                  *
 ******************************************************************************/

struct SRectPackerNode {
    
    ~SRectPackerNode();
    
    glm::vec2 position;
    glm::vec2 size;
    
    SRectPackerNode* children[2];
    
    SRectPackerNode* insertNode(glm::vec2 rect_size);
    
    bool placed = false;
    
};

/******************************************************************************
 *  Definition for rect packer                                                *
 ******************************************************************************/

class SRectPacker {
    
    public:
    
        SRectPacker(glm::vec2 max_size);
        ~SRectPacker();
    
        glm::vec2 insertRect(glm::vec2 rect_size);
    
    
    private:
    
        SRectPackerNode* root_node;
    
    
};

#endif /* SRectPacker_hpp */
