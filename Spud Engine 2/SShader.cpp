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
 *  Functions for model                                                       *
 ******************************************************************************/

SResource* SShader::allocate() { return new SShader(); }

void SShader::bind() {
    
    bound_shader = this;
    glUseProgram(program_id);

}

void SShader::unbind() {
    
    bound_shader = nullptr;
    glUseProgram(0);
}

SShader* SShader::getBoundShader() { return bound_shader; }

int SShader::getUniformLocation(SShader* shader, const char* name) {
    
    // Check if the shader is valid, otherwise return cant fine it
    if (shader && shader->uploaded)
        return glGetUniformLocation(shader->program_id, name);
    else return -1;
    
}

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
    
    return true;
    
}

void SShader::unload() {

    // Delete the program if it was successfully made
    if (uploaded)
        glDeleteProgram(program_id);
    else {
        
        // Delete the strings
        delete vert_string;
        delete frag_string;
        
    }
    
}

void SShader::upload() {
    
    // Create space for the shader programs on the GPU
    GLuint vert_id = glCreateShader(GL_VERTEX_SHADER);
    GLuint frag_id = glCreateShader(GL_FRAGMENT_SHADER);
    
    // Upload the source
    glShaderSource(vert_id, 1, &vert_string, NULL);
    glShaderSource(frag_id, 1, &frag_string, NULL);
    
    // Delete the strings
    delete vert_string;
    delete frag_string;
    
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
    program_id = glCreateProgram();
    glAttachShader(program_id, frag_id);
    glAttachShader(program_id, vert_id);
    
    // Attribute bindings
    glBindAttribLocation(program_id, 0, "position");
    glBindAttribLocation(program_id, 1, "normal");
    glBindAttribLocation(program_id, 2, "tex_coord");
    
    glLinkProgram(program_id);
    
    // Clean up the uneeded shader source
    glDeleteShader(vert_id);
    glDeleteShader(frag_id);
    
    uploaded = true;
    
}
