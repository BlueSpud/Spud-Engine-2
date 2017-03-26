//
//  SUniform.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/14/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SUniform.hpp"

/******************************************************************************
 *  Implementation for resource allocation manager                            *
 ******************************************************************************/

SUniformManger* SUniformManger::instance() {
    
    // Return a static instace of the manager
    static SUniformManger* _instance = NULL;
    if (_instance == NULL)
        _instance = new SUniformManger();
    
    return _instance;
    
}

SUniform* SUniformManger::getUniformWithName(std::string name) {
    
    // Hash the name
    size_t hash = SHash::hashString(name);
    
    // Make sure we have the uniform
    if (uniforms.count(hash))
        return &uniforms[hash];
    
    return nullptr;
    
}

bool SUniformManger::registerUniform(std::string name, void* value, int type, int count) {
    
    // Hash the name
    size_t hash = SHash::hashString(name);
    
    // Make a new uniform
    SUniform uniform;
    uniform.name = name;
    uniform.value = value;
    uniform.type = type;
    uniform.count = count;
    
    // Add it to the map
    uniforms[hash] = uniform;
    
    return true;
    
}
