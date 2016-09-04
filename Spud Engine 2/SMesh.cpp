//
//  SMesh.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/2/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SMesh.hpp"

SResource* SMesh::allocate() { return new SMesh(); }

void SMesh::registerAllocators() {
    
    // Register types of files that can be handled by this class
    SResourceManager::registerAllocatorForExtension("txt", SMesh::allocate);
    SResourceManager::registerAllocatorForExtension("obj", SMesh::allocate);
    
}

void SMesh::load(const SPath& path) {

    SFile* file = SFileSystem::loadFile(path);
    std::string t;
    
    std::cout << "\n\n";
    while (file->getNextTokenWithDeliminator('\n', t))
        std::cout << t << std::endl;
    std::cout << "\n\n";

}

void SMesh::unload() {}

void SMesh::upload() {}
