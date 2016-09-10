//
//  SModel.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/2/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SModel.hpp"

/******************************************************************************
 *  Registration for supported model extensions                               *
 ******************************************************************************/

REGISTER_RESOURCE_CLASS(txt, SModel);
REGISTER_RESOURCE_CLASS(obj, SModel);

/******************************************************************************
 *  Functions for model                                                       *
 ******************************************************************************/

SResource* SModel::allocate() { return new SModel(); }

bool SModel::load(const SPath& path) {

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

void SModel::unload() {}

void SModel::upload() {}
