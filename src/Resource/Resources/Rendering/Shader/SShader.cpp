//
//  SShader.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/10/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SShader.hpp"

SShader* SShader::bound_shader = nullptr;

/******************************************************************************
 *  Implementation for shader                                                 *
 ******************************************************************************/
bool SShader::bind() {
    
    if ((bound_shader != this && uploaded) || (force_rebind && program_id != 0)) {
        
        bound_shader = this;
        glUseProgram(program_id);
        
        // Set force rebind to false
        force_rebind = false;
        
        return true;
        
    }
    
    return false;

}

void SShader::unbind() {
    
    bound_shader = nullptr;
    glUseProgram(0);
}

SShader* SShader::getBoundShader() { return bound_shader; }

int SShader::getUniformLocation(const std::string& uniform) {
	
	// Hash the name
	size_t hash = SHash::hashString(uniform);
	
	if (!locations.count(hash))
		locations[hash] = glGetUniformLocation(program_id, uniform.c_str());
		
	return locations[hash];
	
}

void SShader::bindUniform(void* value, const std::string& name, int type, int count) {
	
	// Check if we have been uploaded, otherwise save it
	if (!uploaded) {
		
		preuploadUniform(value, name, type, count);
		return;
		
	}
	
    // Keep track of the current shader and attempt to bind ourself
	SShader* bound_shader_old = bound_shader;
    bool needed_bind = bind();
    
    // Call the appropriate uniform function
    switch (type) {
            
            case UNIFORM_INT:
                glUniform1iv(getUniformLocation(name), count, (GLint*)value);
                break;
            
            case UNIFORM_FLOAT:
                glUniform1fv(getUniformLocation(name), count, (GLfloat*)value);
                break;
            
            case UNIFORM_VEC2:
                glUniform2fv(getUniformLocation(name), count, (GLfloat*)value);
                break;
            
            case UNIFORM_VEC3:
                glUniform3fv(getUniformLocation(name), count, (GLfloat*)value);
                break;
            
            case UNIFORM_VEC4:
                glUniform4fv(getUniformLocation(name), count, (GLfloat*)value);
                break;
            
	
            case UNIFORM_MAT3:
                glUniformMatrix3fv(getUniformLocation(name), count, GL_FALSE, (GLfloat*)value);
                break;
            
            case UNIFORM_MAT4:
                glUniformMatrix4fv(getUniformLocation(name), count, GL_FALSE, (GLfloat*)value);
                break;
            
    }
	
    // If we were not the current shader, we need to bind the old one
    if (needed_bind && bound_shader_old)
        bound_shader_old->bind();
	else bound_shader = this;
    
}

void SShader::bindUniform(SUniform* uniform) { bindUniform(uniform->value, uniform->name, uniform->type, uniform->count); }

void SShader::bindTextureLocation(const std::string& name, int location) { bindUniform(&location, name, UNIFORM_INT, 1); }

bool SShader::load(const SPath& path) {
	
	// Get two files, one for the frag and one for the vert
	std::string file_name = path.getFilename();
	
	SPath path_vert = SPath(path.getPathAsString());
	path_vert.removeLastPathComponent();
	path_vert.appendPathComponent(file_name + ".vert");
	
	SPath path_frag = SPath(path.getPathAsString());
	path_frag.removeLastPathComponent();
	path_frag.appendPathComponent(file_name + ".frag");
	
	return load(path_vert, path_frag);
	
}

bool SShader::load(const SPath& vert, const SPath& frag) {
    
    // Make sure the program is clear, just in case this is a hot load and we are forcing a rebind
    program_id = 0;
    
    // Save the other path
    if (paths.size() == 1) {
        
        paths.push_back(vert);
        paths.push_back(frag);
        
    }
    
    vert_file = SFileSystem::loadFile(vert);
    frag_file = SFileSystem::loadFile(frag);
    
    // Make sure that we were able to find the shader files
    if (!vert_file || !frag_file)
        return false;
    
    // Accumulate the differnt files into strings
    std::string temp_string;
    std::string file_string;
    
    while (vert_file->getNextTokenWithDeliminator('\n', temp_string))
        file_string = file_string + temp_string + "\n";
    
    const char* vert_string_r = file_string.c_str();
    vert_string = new char[strlen(vert_string_r) + 1];
    strcpy(vert_string, vert_string_r);
    
    file_string = "";
    
    while (frag_file->getNextTokenWithDeliminator('\n', temp_string))
        file_string = file_string + temp_string + "\n";
    
    const char* frag_string_r = file_string.c_str();
    frag_string = new char[strlen(frag_string_r) + 1];
    strcpy(frag_string, frag_string_r);

    // Set that we are done with the files
    vert_file->endUse();
    frag_file->endUse();
    
    // Make an upload
    upload = new SShaderUpload();
    
    upload->vert_string = vert_string;
    upload->frag_string = frag_string;
    
    upload->program_id = &program_id;
    upload->uploaded = &uploaded;
	
	// Tell the upload who we are so that we can upload any uniforms we need to when its uploaded
	upload->shader = this;
    
    SGLUploadSystem::addUpload(upload);
	
    return true;
    
}

void SShader::unload() { unload(true); }

void SShader::unload(bool preuploads) {

    // Delete the program in 2 ways, depending on if its already been uploaded
    if (uploaded) {

        // Send a deletion command
        SShaderUnload *unload = new SShaderUnload();
        unload->program_id = program_id;
        SGLUploadSystem::addUpload(unload);

    } else {

        // Cancel the upload we had already sent
        upload->canceled = true;

        // Free the stuff we have
        upload->unload();

    }

    // Clear stored locations
    locations.clear();

    // Free the preuploads memory
    if (preuploads) {

        for (int i = 0; i < pre_upload_uniforms.size(); i++)
            free(pre_upload_uniforms[i].value);

        pre_upload_uniforms.clear();

    }

}

void SShader::hotload(const SPath& path) {

    // Unload the shader, making sure not to clear the preuploads
    unload(false);

    // Close the files and then load the shader again
    SFileSystem::closeFile(vert_file);
    SFileSystem::closeFile(frag_file);
    
    load(path);
    force_rebind = true;
    
}

void SShader::preuploadUniform(void* value, const std::string& name, int type, int count) {
	
	pre_upload_uniforms.emplace_back(SUniform());
	SUniform& uniform = pre_upload_uniforms.back();
	uniform.type = type;
	uniform.count = count;
	uniform.name = name;
	
	// Get the size of what we need, we might not be guranteed to have the value when we do upload
	size_t size = 0;
	switch (type) {
			
		case UNIFORM_INT:
			size = sizeof(int);
			break;
			
		case UNIFORM_FLOAT:
			size = sizeof(float);
			break;
			
		case UNIFORM_VEC2:
			size = sizeof(glm::vec2);
			break;
			
		case UNIFORM_VEC3:
			size = sizeof(glm::vec3);
			break;
			
		case UNIFORM_VEC4:
			size = sizeof(glm::vec4);
			break;
			
			
		case UNIFORM_MAT3:
			size = sizeof(glm::mat3);
			break;
			
		case UNIFORM_MAT4:
			size = sizeof(glm::mat4);
			break;
			
	}
	
	// Copy the value so we are guranteed to have it
	uniform.value = malloc(size);
	memcpy(uniform.value, value, size);
	
}

/******************************************************************************
 *  Implementation for shader upload                                          *
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
    glBindAttribLocation(*program_id, 3, "tangent");
	glBindAttribLocation(*program_id, 4, "bone_indices");
	glBindAttribLocation(*program_id, 5, "vertex_weights");
    
    glLinkProgram(*program_id);
    
    // Clean up the unneeded shader source
    glDeleteShader(vert_id);
    glDeleteShader(frag_id);
	
	// Go through the uniform que
	for (int i = 0; i < shader->pre_upload_uniforms.size(); i++) {
		
		// Upload it
		shader->bindUniform(&shader->pre_upload_uniforms[i]);
			
		// Memory was allocated for the preuploads, free it
		//free(shader->pre_upload_uniforms[i].value);
		
	}
	
	//shader->pre_upload_uniforms.clear();
    
}

void SShaderUpload::unload() {
    
    // Delete the strings in RAM
    delete vert_string;
    delete frag_string;
    
}

/******************************************************************************
 *  Implementation for shader unload                                          *
 ******************************************************************************/

void SShaderUnload::upload() { glDeleteProgram(program_id);}

void SShaderUnload::unload() { /* nothing */ }
