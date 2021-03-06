//
//  SCubeMap.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/14/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SCubeMap_hpp
#define SCubeMap_hpp

#include <FreeImage.h>

#include "Resource/Resources/Rendering/STexture.hpp"
#include "Rendering/Pipeline/SRenderingPipeline.hpp"

/******************************************************************************
 *  Definition for cube map upload                                            *
 ******************************************************************************/

struct SCubeMapUpload : public SGLUpload {
    
    virtual void upload();
    virtual void unload();
  
    BYTE** image_data;
    FIBITMAP** bitmaps;
    
    unsigned int width;
    unsigned int height;
    
    GLuint* texture_id;
    
};

/******************************************************************************
 *  Definition for cube map                                                   *
 ******************************************************************************/

class SCubeMap : public STexture {

	SRESOURCE(SCubeMap, "cube")

    public:
	
		SCubeMap();
		SCubeMap(const glm::vec3& position, SRenderingPipeline* pipeline, SSceneGraph& scene_graph, unsigned int size);
    
        virtual void bind();
        virtual void bind(int texture_number);
		virtual void unload();
    
    protected:
    
        virtual bool load(const SPath& path);
    
    private:

        BYTE* image_data[6];
        FIBITMAP* bitmaps[6];
    
        unsigned int width;
        unsigned int height;
    
        GLuint texture_id;
    
};

#endif /* SCubeMap_hpp */
