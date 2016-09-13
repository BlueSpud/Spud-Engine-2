//
//  SShader.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/10/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SShader_hpp
#define SShader_hpp

#include "SResourceManager.hpp"
#include "SGL.hpp"

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
 *  Definition for shader                                                     *
 ******************************************************************************/

class SShader : public SResource {
    
    public:
    
        static SResource* allocate();
    
        bool bind();
        void static unbind();
        static SShader* getBoundShader();
    
        static int getUniformLocation(SShader* shader, const std::string& name);
    
        void bindUniform(void* value, const std::string& name, int type, int count);
    
    protected:
    
        virtual bool load(const SPath& path);
        virtual void unload();
    
        virtual void upload();
    
    private:
    
        char* vert_string;
        char* frag_string;
    
        GLuint program_id;

        static SShader* bound_shader;
    
    
};

/******************************************************************************
 *  Definition for uniform struct                                             *
 ******************************************************************************/

struct SShaderUniform {
    
    int type;
    int count;
    void* value;
    
    std::string name;
    
    SShaderUniform(void* _value, std::string _name, int _type, int _count);
    void bind(SShader* shader);
    
};

#endif /* SShader_hpp */
