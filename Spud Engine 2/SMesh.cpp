//
//  SMesh.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/2/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SMesh.hpp"

/******************************************************************************
 *  Registration for supported mesh extensions                                *
 ******************************************************************************/

REGISTER_RESOURCE_CLASS(txt, SMesh);
REGISTER_RESOURCE_CLASS(obj, SMesh);

/******************************************************************************
 *  Functions for mesh                                                        *
 ******************************************************************************/

SResource* SMesh::allocate() { return new SMesh(); }

bool SMesh::load(const SPath& path) {

    SFile* file = SFileSystem::loadFile(path);
    
    if (file) {
    
        std::string t;
    
        std::cout << "\n\n";
        while (file->getNextTokenWithDeliminator('\n', t))
            std::cout << t << std::endl;
        std::cout << "\n\n";
        
        return true;
        
    }

    return false;
    
}

void SMesh::unload() {}

void SMesh::upload() {}
