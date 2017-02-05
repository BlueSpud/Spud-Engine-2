//
//  SRectPacker.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 10/27/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SRectPacker.hpp"

/******************************************************************************
 *  Implementation for rect node                                              *
 ******************************************************************************/

SRectPackerNode::~SRectPackerNode() {
    
    // If we had children in this node, delete them
    if (children[0] != nullptr) {
        
        delete children[0];
        delete children[1];
        
    }
    
}

SRectPackerNode* SRectPackerNode::insertNode(glm::vec2 rect_size) {
    
    // If we dont have something placed inside here, check if we can place it
    if (!placed) {
        
        if (size.x >= rect_size.x && size.y >= rect_size.y) {
            
            // We could place it in this node, create child nodes
            children[0] = new SRectPackerNode();
            children[1] = new SRectPackerNode();
            
            children[0]->position = glm::vec2(position.x + rect_size.x, position.y);
            children[0]->size = glm::vec2(size.x - rect_size.x, rect_size.y);
            
            children[1]->position = glm::vec2(position.x, position.y + rect_size.y);
            children[1]->size = glm::vec2(rect_size.x, size.y - rect_size.y);
            
            // Save that this node has something in it and return self as the node that we placed in
            placed = true;
            return this;
            
        }
        
        return nullptr;
        
    }
    
    SRectPackerNode* accepting_child_node = children[1]->insertNode(rect_size);
    if (!accepting_child_node)
        accepting_child_node = children[0]->insertNode(rect_size);

    return accepting_child_node;
    
}

/******************************************************************************
 *  Implementation for rect packer                                            *
 ******************************************************************************/

SRectPacker::SRectPacker(glm::vec2 max_size) {
    
    // Create the root node
    root_node = new SRectPackerNode();
    
    // The root node is at (0,0) and has the max size that we want
    root_node->size = max_size;
    
}

SRectPacker::~SRectPacker() {
    
    // Delete the root node, this should propagate down through the tree
    delete root_node;
    
}

glm::vec2 SRectPacker::insertRect(glm::vec2 rect_size) {
    
    // Try to insert it into the tree somewhere
    SRectPackerNode* accepting_node = root_node->insertNode(rect_size);
    
    // The node will be valid if we were able to accept the rect somewhere into the tree
    if (accepting_node)
        return accepting_node->position;
    
    // If we could not put it into the tree, return a negative position so we can check if it is out of bounds
    return glm::vec2(-1);
    
}
