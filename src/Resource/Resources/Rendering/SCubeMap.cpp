//
//  SCubeMap.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/14/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SCubeMap.hpp"

static std::string side_names[] = {"right.jpg", "left.jpg", "bottom.jpg", "top.jpg", "front.jpg", "back.jpg"};

static glm::vec3 rotations[] = {
	
		glm::vec3(0.0,  -M_PI_2, 0.0),
		glm::vec3(0.0, M_PI_2, 0.0),
        glm::vec3(-M_PI_2,  M_PI, 0.0),
        glm::vec3(M_PI_2, M_PI, 0.0),
	    glm::vec3(0.0,  M_PI,    0.0),
	    glm::vec3(0.0,  0.0,   0.0)

};

/******************************************************************************
 *  Registration for supported texture extensions                             *
 ******************************************************************************/

REGISTER_RESOURCE_CLASS(cube, SCubeMap)

/******************************************************************************
 *  Implementation for cube map                                               *
 ******************************************************************************/

SCubeMap::SCubeMap() { /* should never be called manually!!!! */ }

SCubeMap::SCubeMap(const glm::vec3& position, SRenderingPipeline* pipeline, SSceneGraph& scene_graph, unsigned int size) {
	
	// Instead of loading the cubemap from a file, we render a scene to a cubemap
	SCamera camera;
	camera.transform.translation = position;
	
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
	
	// Parameters for the cube map
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	// Allocate storage for the sides
	for (int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_INT, nullptr);

	// Create a framebuffer
	GLuint framebuffer;
	GLuint depth;
	GLuint color;

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	
	// Gen depth
	glGenTextures(1, &depth);
	glBindTexture(GL_TEXTURE_2D, depth);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size, size, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);
	
	// Gen color
	glGenTextures(1, &color);
	glBindTexture(GL_TEXTURE_2D, color);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_INT, 0);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);
	
	// Render
	for (int i = 0; i < 6; i++) {
		
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		
		// Store the framebuffer in the cubemap
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, texture_id, 0);
		
		// Set camera rotation
		camera.transform.rotation = rotations[i];
		
		pipeline->render(scene_graph, camera, 0.0);
		pipeline->finalizeRender(framebuffer);
		
	}
	
	// Generate a mipmap
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	
	// Cleanup
	//glDeleteTextures(1, &depth);
	//glDeleteTextures(1, &color);
	//glDeleteFramebuffers(1, &framebuffer);
	
}

void SCubeMap::bind() {

    // Bind the cube map to texture 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
    
}

void SCubeMap::bind(int texture_number) {
    
    // Bind the cube map to the texture specified
    glActiveTexture(GL_TEXTURE0 + texture_number);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
    
}

bool SCubeMap::load(const SPath& path) {

    std::string absolute_path = path.getPathAsAbsolutePath();
    FreeImage_Initialise();
    
    // Loop through each of the names of the files and load them up
    for (int i = 0; i < 6; i++) {
    
        std::string file_path = absolute_path + "/" + side_names[i];
        
        // Save the path if we still have room
        if (paths.size() < 7) {
            
            SPath new_path = SPath(path.getPathAsString() + "/" + side_names[i]);
            paths.push_back(new_path);
            
        }
    
        if (!path.getIsDirectory()) {
        
            // Initialize free image and prepare storage
			//if (!STexture::freeimage_initialized) {
                
                FreeImage_Initialise();
			//freeimage_initialized = true;
                
			//}
			
            FREE_IMAGE_FORMAT format;
        
            // Get the file format and load the image
            format = FreeImage_GetFileType((const char*)file_path.c_str(),0);
            bitmaps[i] = FreeImage_Load(format, (const char*)file_path.c_str());
        
            // Make sure we can actualy read the image
            if (FreeImage_HasPixels(bitmaps[i])) {
            
                // Save the data, done loading
                image_data[i] = FreeImage_GetBits(bitmaps[i]);
                width = FreeImage_GetWidth(bitmaps[i]);
                height =  FreeImage_GetHeight(bitmaps[i]);
            
            } else return false;
            
            
        } else return false;
        
    }
    
    // Create an upload
    SCubeMapUpload* upload = new SCubeMapUpload();
    upload->bitmaps = bitmaps;
    upload->image_data = image_data;
    
    upload->width = width;
    upload->height = height;
    upload->texture_id = &texture_id;
    upload->uploaded = &uploaded;
    
    SGLUploadSystem::addUpload(upload);
    
    return true;
}


void SCubeMap::unload() {

    // Delete the texture on the GPU
    if (uploaded)
        glDeleteTextures(1, &texture_id);

}

/******************************************************************************
 *  Implementation for cube map upload                                        *
 ******************************************************************************/

void SCubeMapUpload::upload() {

    // Generate a texture
    glGenTextures(1, texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *texture_id);
    
    // Parameters for the cube map
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // Figure out if we had alpha
    GLint internal_format = GL_RGB;
    GLint external_format = GL_BGR;
    if (FreeImage_GetChannel(bitmaps[0], FICC_ALPHA) != NULL) {
        
        internal_format = GL_RGBA;
        external_format = GL_BGRA;
        
    }
    
    // Add all the textures into the cube map
    for (int i = 0; i < 6; i++) {
    
        // Set the parameters of the texture
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internal_format, width, height, 0, external_format, GL_UNSIGNED_BYTE, image_data[i]);
        
    }
	
	// Generate a mipmap
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	
    unload();

    *uploaded = true;
    
}

void SCubeMapUpload::unload() {
    
    // Clean up image data still in normal RAM
    for (int i = 0; i < 6; i++)
        FreeImage_Unload(bitmaps[i]);
    
}
