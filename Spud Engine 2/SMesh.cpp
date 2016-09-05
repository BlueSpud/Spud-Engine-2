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
    
        SLog::verboseLog(SVerbosityLevel::Debug, "\n");
        while (file->getNextTokenWithDeliminator('\n', t))
            SLog::verboseLog(SVerbosityLevel::Debug, "%s", t.c_str());
        SLog::verboseLog(SVerbosityLevel::Debug, "\n");
        
        return true;
        
    }

    return false;
    
}

void SMesh::unload() {}

void SMesh::upload() {}
