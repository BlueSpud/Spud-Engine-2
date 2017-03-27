//
//  STexture.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/10/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "STexture.hpp"

/******************************************************************************
 *  Registration for supported texture extensions                             *
 ******************************************************************************/

REGISTER_RESOURCE_CLASS(dds, STexture)

/******************************************************************************
 *  Implementation for texture                                                *
 ******************************************************************************/

void STexture::bind() {
    
    // Bind the texture to texture 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);

}

void STexture::bind(int texture_number) {
    
    // Bind the texture to texture specified
    glActiveTexture(GL_TEXTURE0 + texture_number);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    
}

bool STexture::load(const SPath& path) {
	
	file = SFileSystem::loadFile(path, true);
	
	if (file) {
		
		// Read the magic
		std::uint32_t magic;
		file->read((char*)&magic, sizeof(std::uint32_t));
		
		// Check that magic was right
		if (magic != DDS_MAGIC) {
			
			SLog::verboseLog(SVerbosityLevel::Critical, "DDS magic was incorrect");
			return false;
			
		}
		
		// Read the header
		SDDSHeader header;
		file->read((char*)&header, sizeof(header));
		
		// Make sure the header size was 124
		if (header.size != DDS_HEADER_SIZE) {
			
			SLog::verboseLog(SVerbosityLevel::Critical, "DDS header size was not 124");
			return false;
			
		}
		
		// Make sure we are using one of the supported formats
		if (!(header.flags & DDPF_FOURCC)) {
			
			SLog::verboseLog(SVerbosityLevel::Critical, "DDS format was not supported");
			return false;
			
		}
		
		int block_bytes;
		GLenum internal_format;
		
		// Figure out which format we are using, DX1, DX3 or DX5
		switch (header.format.four_CC) {
				
			case D3DFMT_DXT1:
				
				// DirectX 1
				block_bytes = 8;
				internal_format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
				
				break;
				
			case D3DFMT_DXT3:
				
				// DirectX 3
				block_bytes = 16;
				internal_format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				
				break;
				
			case D3DFMT_DXT5:
				
				// DirectX 5
				block_bytes = 16;
				internal_format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				
				break;
				
			default:
				
				SLog::verboseLog(SVerbosityLevel::Critical, "DDS format was not supported");
				return false;
				
				break;
				
		}
		
		// Get the number of mipmaps
		int mip_maps = header.flags & DDSD_MIPMAPCOUNT;
		
		// If mipmap is absent from the flags, this was a single texture
		if (!mip_maps)
			mip_maps = 1;
		else mip_maps = header.mipmaps;
		
		// Store mip map size seperately
		unsigned int size_x = header.width;
		unsigned int size_y = header.height;
		
		// Determine the size we need to read
		size_t size_b = glm::max((unsigned int)DDS_DIV_SIZE, size_x) / DDS_DIV_SIZE * glm::max((unsigned int)DDS_DIV_SIZE, (unsigned int)size_y) / DDS_DIV_SIZE * block_bytes;
		
		// Read the mipmaps
		for (int i = 0; i < mip_maps; i++) {
			
			// Allocate space for the data
			char* data = (char*)malloc(size_b);
			
			// Read and upload
			file->read(data, size_b);
			
			SMipMap mipmap;
			mipmap.data = data;
			mipmap.level = i;
			mipmap.size = size_b;
			mipmap.dimensions = glm::ivec2(size_x, size_y);
			
			mipmaps.push_back(mipmap);
			
			// Resize what we are going to read for the mipmaps
			size_x = (size_x + 1) >> 1;
			size_y = (size_y + 1) >> 1;
			
			size_b = glm::max((unsigned int)DDS_DIV_SIZE, size_x) / DDS_DIV_SIZE * glm::max((unsigned int)DDS_DIV_SIZE, (unsigned int)size_y) / DDS_DIV_SIZE * block_bytes;
			
		}
		
		// The size of the image is the size of the top mipmap
		size = mipmaps[0].dimensions;
		
		// Create the upload
		upload = new STextureUpload(mipmaps);
		upload->texture_id = &texture_id;
		upload->format = internal_format;
		
		upload->uploaded = &uploaded;
		
		SGLUploadSystem::addUpload(upload);
		
		return true;
		
	}
	
	return false;
    
}

void STexture::unload() {

    // Delete the texture on the GPU or the CPU data if we need to
	if (uploaded) {
	
		STextureUnload* unload = new STextureUnload();
		unload->texture_id = texture_id;
		
		SGLUploadSystem::addUpload(unload);
		
	} else for (int i = 0; i < mipmaps.size(); i++)
		free(mipmaps[i].data);

}

void STexture::hotload(const SPath& path) {
    
    // Delete the last texture
    if (uploaded) {
        
        // Send a deletion command
        STextureUnload* unload = new STextureUnload();
        unload->texture_id = texture_id;
        SGLUploadSystem::addUpload(unload);
        
    } else {
        
        // Cancel the upload we had already sent
        upload->canceled = true;
        
        // Free the stuff we have
        upload->unload();
        
    }
	
	// Clear mipmap
	mipmaps.clear();
	
	SFileSystem::closeFile(file);
	
    // Load the new texture
    load(path);
    
}

/******************************************************************************
 *  Implementation for texture upload                                         *
 ******************************************************************************/

void STextureUpload::upload() {

    // Generate a texture
    glGenTextures(1, texture_id);
    glBindTexture(GL_TEXTURE_2D, *texture_id);

	// Upload the data
	for (int i = 0; i < mipmaps.size(); i++)
		glCompressedTexImage2D(GL_TEXTURE_2D, i, format, mipmaps[i].dimensions.x, mipmaps[i].dimensions.y, 0, (int)mipmaps[i].size, mipmaps[i].data);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	if (mipmaps.size() == 1)
		glGenerateMipmap(GL_TEXTURE_2D);
	else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, (int)mipmaps.size() - 1);
    
    unload();
	
	*uploaded = true;
	
    // Bind the default texture just in case
    glBindTexture(GL_TEXTURE_2D, 0);
    
}

void STextureUpload::unload() {
    
	for (int i = 0; i < mipmaps.size(); i++)
		free(mipmaps[i].data);
	
}

/******************************************************************************
 *  Definition for texture safe detroy                                        *
 ******************************************************************************/

void STextureUnload::upload() { glDeleteTextures(1, &texture_id); }

void STextureUnload::unload() { /* nothing */ }

