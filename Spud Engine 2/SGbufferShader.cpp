//
//  SGbufferShader.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 2/1/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#include "SGbufferShader.hpp"

/******************************************************************************
 *  Implementation for Gbuffer shader                                         *
 ******************************************************************************/

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
		
	}
	
}
