//
//  SUniform.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/14/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SUniform_hpp
#define SUniform_hpp

#include <iostream>
#include <map>

#include "Engine/Utility/SHash.hpp"

/******************************************************************************
 *  Definition for types of uniforms                                          *
 ******************************************************************************/

#define UNIFORM_INT 0
#define UNIFORM_FLOAT 1

#define UNIFORM_VEC2 10
#define UNIFORM_VEC3 11
#define UNIFORM_VEC4 12

#define UNIFORM_MAT3 20
#define UNIFORM_MAT4 21

/******************************************************************************
 *  Definition for uniform struct                                             *
 ******************************************************************************/

struct SUniform {
    
    int type, count;
    void* value;
    
    std::string name;
    
};

/******************************************************************************
 *  Definition for uniform manager                                            *
 ******************************************************************************/

class SUniformManger {
    
    public:
    
        bool registerUniform(std::string name, void* value, int type, int count);
        SUniform* getUniformWithName(std::string name);
    
        static SUniformManger* instance();
    
    private:
    
        std::map<size_t, SUniform> uniforms;
    
};

// Define a macro that can be used to register a variable
#define REGISTER_UNIFORM(V, N, T, C) bool isRegistered_##T_##E =  SUniformManger::instance()->registerUniform(N, V, T, C)

#endif /* SUniform_hpp */
