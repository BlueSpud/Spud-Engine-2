//
//  STexture.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/10/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef STexture_hpp
#define STexture_hpp

#include <vector>

#include "Resource/SResourceManager.hpp"
#include "Resource/SGLUpload.hpp"

#define DDS_MAGIC 0x20534444
#define DDS_HEADER_SIZE 124

#define DDPF_FOURCC 0x00000004
#define D3DFMT_DXT1 '1TXD'
#define D3DFMT_DXT3 '3TXD'
#define D3DFMT_DXT5 '5TXD'

#define DDS_DIV_SIZE 4

#define DDSD_MIPMAPCOUNT 0x20000

/******************************************************************************
 *  Definition for mip map used for texture upload						      *
 ******************************************************************************/

struct SMipMap {
	
	char* data;
	int level;
	
	size_t size;
	glm::ivec2 dimensions;
	
};

/******************************************************************************
 *  Definition for dds components (most of these are unused currently)        *
 ******************************************************************************/

struct SDDSPixelFormat {
	
	std::uint32_t size;
	std::uint32_t flags;
	std::uint32_t four_CC;
	std::uint32_t bit_count;
	std::uint32_t red_bit_mask;
	std::uint32_t green_bit_mask;
	std::uint32_t blue_bit_mask;
	std::uint32_t alpha_bit_mask;
	
};


struct SDDSHeader {
	
	std::uint32_t size;
	std::uint32_t flags;
	std::uint32_t height;
	std::uint32_t width;
	std::uint32_t pitch_or_linear_scale;
	std::uint32_t depth;
	std::uint32_t mipmaps;
	std::uint32_t reserved[11];
	SDDSPixelFormat	format;
	std::uint32_t caps;
	std::uint32_t caps_1;
	std::uint32_t caps_2;
	std::uint32_t caps_3;
	std::uint32_t reserved2;
	
};

/******************************************************************************
 *  Definition for upload                                                     *
 ******************************************************************************/

struct STextureUpload : public SGLUpload {
	
	STextureUpload(const std::vector<SMipMap>& _mipmaps) : mipmaps(_mipmaps) {}
	
    virtual void upload();
    virtual void unload();
	
	const std::vector<SMipMap>& mipmaps;
	
	GLenum format;
    GLuint* texture_id;
    
};

/******************************************************************************
 *  Definition for safe detroy                                                *
 ******************************************************************************/

struct STextureUnload : public SGLUpload {
  
    virtual void upload();
    virtual void unload();
    
    GLuint texture_id;
    
};

/******************************************************************************
 *  Definition for texture                                                    *
 ******************************************************************************/

class STexture : public SResource {
    
    public:
    
        glm::ivec2 size;
    
        virtual void bind();
        virtual void bind(int texture_number);
    
    protected:
    
        virtual bool load(const SPath& path);
        virtual void unload();
        virtual void hotload(const SPath& path);
    
    private:
	
		SFile* file;
    
		std::vector<SMipMap> mipmaps;
    
        unsigned int width;
        unsigned int height;
    
        GLuint texture_id;
    
        STextureUpload* upload;
    
};

#endif /* STexture_hpp */
