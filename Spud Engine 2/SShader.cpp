//
//  SShader.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/10/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SShader.hpp"

SShader* SShader::bound_shader;

/***********************************************************************************
 *  Registration for supported shader extensions (GLSL used to load frag and vert) *
 ***********************************************************************************/

REGISTER_RESOURCE_CLASS(glsl, SShader);

/******************************************************************************
 *  Functions for shader                                                      *
 ******************************************************************************/

SResource* SShader::allocate() { return new SShader(); }

bool SShader::bind() {
    
    if (bound_shader != this && uploaded) {
        
        bound_shader = this;
        glUseProgram(program_id);
        
        return true;
        
    }
    
    return false;

}

void SShader::unbind() {
    
    bound_shader = nullptr;
    glUseProgram(0);
}

SShader* SShader::getBoundShader() { return bound_shader; }

int SShader::getUniformLocation(SShader* shader, const std::string& name) {
    
    // Check if the shader is valid, otherwise return cant fine it
    if (shader && shader->uploaded)
        return glGetUniformLocation(shader->program_id, name.c_str());
    else return -1;
    
}

void SShader::bindUniform(void* value, const std::string& name, int type, int count) {
    
    // Keep track of the current shader and attempt to bind ourself
    SShader* bound_shader_old = bound_shader;
    bool needed_bind = bind();
    
    // Call the appropriate uniform function
    switch (type) {
            
            case UNIFORM_INT:
                glUniform1iv(glGetUniformLocation(program_id, name.c_str()), count, (GLint*)value);
                break;
            
            case UNIFORM_FLOAT:
                glUniform1fv(glGetUniformLocation(program_id, name.c_str()), count, (GLfloat*)value);
                break;
            
            case UNIFORM_VEC2:
                glUniform2fv(glGetUniformLocation(program_id, name.c_str()), count, (GLfloat*)value);
                break;
            
            case UNIFORM_VEC3:
                glUniform3fv(glGetUniformLocation(program_id, name.c_str()), count, (GLfloat*)value);
                break;
            
            case UNIFORM_VEC4:
                glUniform4fv(glGetUniformLocation(program_id, name.c_str()), count, (GLfloat*)value);
                break;
            
            
            case UNIFORM_MAT3:
                glUniformMatrix3fv(glGetUniformLocation(program_id, name.c_str()), count, GL_FALSE, (GLfloat*)value);
                break;
            
            case UNIFORM_MAT4:
                glUniformMatrix4fv(glGetUniformLocation(program_id, name.c_str()), count, GL_FALSE, (GLfloat*)value);
                break;
            
    }
    
    // If we were not the current shader, we need to bind the old one
    if (needed_bind)
        bound_shader_old->bind();
    
}

void SShader::bindUniform(SUniform* uniform) { bindUniform(uniform->value, uniform->name, uniform->type, uniform->count); }

bool SShader::load(const SPath& path) {
    
    // Get two files, one for the frag and one for the vert
    std::string file_name = path.getFilename();
    
    SPath path_vert = SPath(path.getPathAsString());
    path_vert.removeLastPathComponent();
    path_vert.appendPathComponent(file_name + ".vert");
    
    SPath path_frag = SPath(path.getPathAsString());
    path_frag.removeLastPathComponent();
    path_frag.appendPathComponent(file_name + ".frag");
    
    SFile* vert_file = SFileSystem::loadFile(path_vert);
    SFile* frag_file = SFileSystem::loadFile(path_frag);
    
    // Make sure that we were able to find the shader files
    if (!vert_file || !frag_file)
        return false;
    
    // Accumulate the differnt files into strings
    std::string temp_string;
    std::string file_string;
    
    while (vert_file->getNextTokenWithDeliminator('\n', temp_string))
        file_string = file_string + temp_string + "\n";
    
    const char* vert_string_r = file_string.c_str();
    vert_string = new char[strlen(vert_string_r)];
    strcpy(vert_string, vert_string_r);
    
    file_string = "";
    
    while (frag_file->getNextTokenWithDeliminator('\n', temp_string))
        file_string = file_string + temp_string + "\n";
    
    const char* frag_string_r = file_string.c_str();
    frag_string = new char[strlen(frag_string_r)];
    strcpy(frag_string, frag_string_r);
    
    // Make an upload
    SShaderUpload* upload = new SShaderUpload();
    
    upload->vert_string = vert_string;
    upload->frag_string = frag_string;
    
    upload->program_id = &program_id;
    upload->uploaded = &uploaded;
    
    SGLUploadSystem::addUpload(upload);
    
    return true;
    
}

void SShader::unload() {

    // Delete the program if it was successfully made
    if (uploaded)
        glDeleteProgram(program_id);
    
}

/******************************************************************************
 *  Functions for shader upload                                               *
 ******************************************************************************/

void SShaderUpload::upload() {
    
    // Regardless of if we fail, we uploaded
    *uploaded = true;
    
    // Create space for the shader programs on the GPU
    GLuint vert_id = glCreateShader(GL_VERTEX_SHADER);
    GLuint frag_id = glCreateShader(GL_FRAGMENT_SHADER);
    
    // Upload the source
    glShaderSource(vert_id, 1, &vert_string, NULL);
    glShaderSource(frag_id, 1, &frag_string, NULL);
    
    unload();
    
    // Compile the shaders
    glCompileShader(vert_id);
    
    GLint compiled = 0;
    glGetShaderiv(vert_id, GL_COMPILE_STATUS, &compiled);
    
    if (compiled == GL_FALSE) {
        
        // Failed!
        GLint log_length = 0;
        glGetShaderiv(vert_id, GL_INFO_LOG_LENGTH, &log_length);
        
        char* log = new char[log_length];
        glGetShaderInfoLog(vert_id, log_length, &log_length, &log[0]);
        
        SLog::verboseLog(SVerbosityLevel::Critical, "Compile for vertex shader failed! Error: %s", log);
        
        // Clean up
        glDeleteShader(vert_id);
        glDeleteShader(frag_id);
        
        delete[] log;
        return;
        
    }
    
    glCompileShader(frag_id);
    
    compiled = 0;
    glGetShaderiv(frag_id, GL_COMPILE_STATUS, &compiled);
    
    if (compiled == GL_FALSE) {
        
        // Failed!
        GLint log_length = 0;
        glGetShaderiv(frag_id, GL_INFO_LOG_LENGTH, &log_length);
        
        char* log = new char[log_length];
        glGetShaderInfoLog(frag_id, log_length, &log_length, &log[0]);
        
        SLog::verboseLog(SVerbosityLevel::Critical, "Compile for fragment shader failed! Error: %s", log);
        
        // Clean up
        glDeleteShader(vert_id);
        glDeleteShader(frag_id);
        
        delete[] log;
        return;
        
    }
    
    
    // Both shaders compiled correctly, link them and finish
    *program_id = glCreateProgram();
    glAttachShader(*program_id, frag_id);
    glAttachShader(*program_id, vert_id);
    
    // Attribute bindings
    glBindAttribLocation(*program_id, 0, "position");
    glBindAttribLocation(*program_id, 1, "normal");
    glBindAttribLocation(*program_id, 2, "tex_coord");
    
    glLinkProgram(*program_id);
    
    // Clean up the uneeded shader source
    glDeleteShader(vert_id);
    glDeleteShader(frag_id);
    
}

void SShaderUpload::unload() {
    
    // Delete the strings in RAM
    delete vert_string;
    delete frag_string;
    
}
