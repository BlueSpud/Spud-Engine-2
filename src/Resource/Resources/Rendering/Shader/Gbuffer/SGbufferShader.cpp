//
//  SGbufferShader.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 2/1/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#include "SGbufferShader.hpp"

/***********************************************************************************
 *  Registration for supported shader extensions (GLSL used to load frag and vert) *
 ***********************************************************************************/

REGISTER_RESOURCE_CLASS(glsl, SGbufferShader)

/******************************************************************************
 *  Implementation for Gbuffer shader                                         *
 ******************************************************************************/

bool SGbufferShader::load(const SPath& path) {
	
	// Get two files, one for the frag and one for the vert
	std::string file_name = path.getFilename();
	
	SPath path_vert = SPath(path.getPathAsString());
	path_vert.removeLastPathComponent();
	path_vert.appendPathComponent(file_name + ".vert");
	paths.push_back(path_vert);
	
	SPath path_frag = SPath(path.getPathAsString());
	path_frag.removeLastPathComponent();
	path_frag.appendPathComponent(file_name + ".frag");
	paths.push_back(path_frag);
	
	for (int i = 0; i < SGbufferShaderCount; i++) {
		
		// Compile the vertex shader
		compileShader(path_vert, SPath("Shader/master/" + std::string(SGbufferShaderExtensions[i]) + ".master"), SGbufferShaderExtensions[i]);
		
		// Now compile a GLSL shader for the extension
		SPath compiled_vertex_path = path_vert;
		compiled_vertex_path.removeLastPathComponent();
		compiled_vertex_path.appendPathComponent(file_name + "_" + SGbufferShaderExtensions[i] + ".vert");
		
		SShader* shader = new SShader();
		
		std::cout << "Check" << path.getPathAsString() << std::endl;
		
		if (!shader->load(compiled_vertex_path, path_frag)) {
			
			SLog::verboseLog(SVerbosityLevel::Critical, "Failed to compile Gbuffer shader attatchment %s", SGbufferShaderExtensions[i]);
			return false;
			
		}

		shaders[(SGbufferShaderShaders)i] = shader;
		
	}
	
	return true;
	
}

void SGbufferShader::hotload(const SPath& path) {
	
	// Create a hotload because we are deleting shaders and something might need them for this frame
	SGbufferShaderHotload* hotload = new SGbufferShaderHotload(path);
	hotload->shader = this;
	
	SGLUploadSystem::addUpload(hotload);
	
}

void SGbufferShader::compileShader(SPath shader_vert, const SPath& master_vert, const std::string& extension) {
	
	// Get the master and the file we are going to compile
	SFile* master_file = SFileSystem::loadFile(master_vert);
	SFile* shader_file = SFileSystem::loadFile(shader_vert);
	
	if (master_file && shader_file) {
	
		// Load up the master file as a string
		std::string line;
		std::string master;
	
		while(master_file->getNextTokenWithDeliminator('\n', line))
			master = master + line + "\n";
	
		// Load the shader file into a string
		std::string shader;
		while (shader_file->getNextTokenWithDeliminator('\n', line))
			shader = shader + line + "\n";
		
		// Now grab the uniform and in / out declarations from the master
		size_t first_end = master.find("#SEnd");
	
		// Declarations are appended to before the shader
		shader.insert(0, master.substr(0, first_end) + "\n");
		
		// Get the next occufance of the #SEnd in the master shader
		size_t second_end = master.find("#SEnd", first_end + 1);
		
		// Figure out where the body of the main function starts
		size_t main_decl = shader.find("void main()");
		size_t main_body = shader.find("{", main_decl);
		
		// Insert the pre-vertex shader code, offset by 5 because we need to accoutn for the #SEnd chars
		shader.insert(main_body + 1, "\n" + master.substr(first_end + 5, second_end - (first_end + 5)));
		
		// We go through all of the brackets and try to find the end of the main function
		int brackets = 1;
		size_t current_char = main_body;
		
		while (brackets && current_char < shader.length()) {
			
			current_char++;
			
			// If it is an open bracket, we need to find an additional close block
			if (!shader.substr(current_char, 1).compare("{"))
				brackets++;
			else if (!shader.substr(current_char, 1).compare("}"))
				brackets--;
			
		}
		
		// We should know where the last bracket is so we copy and past the final code in
		shader.insert(current_char - 1, master.substr(second_end + 5, master.length() - (second_end + 5)));
		
		// The shader string now contains the shader with the inserted code, so we just need to write it out to a file, same directory as the original shader
		std::string new_file_name = shader_vert.getFilename() + "_" + extension + ".vert";
		shader_vert.removeLastPathComponent();
		shader_vert.appendPathComponent(new_file_name);
		
		SFileSystem::writeStringToFile(shader_vert, shader);
		
		// Save the path for hotloading
		paths.push_back(master_vert);
		
	}
	
}

bool SGbufferShader::bind(SGbufferShaderShaders shader) { return shaders[shader]->bind(); }

void SGbufferShader::bindUniform(SGbufferShaderShaders shader, void* value, const std::string& name, int type, int count) { shaders[shader]->bindUniform(value, name, type, count); }

void SGbufferShader::bindUniform(SGbufferShaderShaders shader, SUniform* uniform) { shaders[shader]->bindUniform(uniform); }

void SGbufferShader::bindTextureLocation(SGbufferShaderShaders shader, const std::string& name, int location) { shaders[shader]->bindTextureLocation(name, location); }

void SGbufferShader::unload() {
	
	// Unload all of the shaders
	for (std::map<SGbufferShaderShaders, SShader*>::iterator i = shaders.begin(); i != shaders.end(); i++) {
		
		i->second->unload();
		delete i->second;
		
	}
	
}

/******************************************************************************
 *  Implementation for Gbuffer hotload                                        *
 ******************************************************************************/

void SGbufferShaderHotload::upload() {
	
	// Call unload on all of the shaders
	for (std::map<SGbufferShaderShaders, SShader*>::iterator i = shader->shaders.begin(); i != shader->shaders.end(); i++) {
		
		// Unload the shader
		i->second->unload();
		delete i->second;
		
		// Erase the shader
		shader->shaders.erase(i);
		i = shader->shaders.begin();
		
	}
	
	// Start at 1 to skip the default .glsl path
	for (int i = 1; i < shader->paths.size(); i++)
		SFileSystem::closeFile(SFileSystem::loadFile(shader->paths[i]));
	
	// Clear the hotload paths
	shader->paths.clear();
	
	// Reload the shader
	shader->load(path);
	
}

void SGbufferShaderHotload::unload() { /* intentionally empty */ }
