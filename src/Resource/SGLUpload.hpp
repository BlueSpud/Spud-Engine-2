//
//  SGLUpload.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/15/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SGLUpload_hpp
#define SGLUpload_hpp

#include <queue>

#include "Rendering/SGL.hpp"

/******************************************************************************
 *  Defiunition for default kind of upload                                    *
 ******************************************************************************/

struct SGLUpload {
    
    virtual ~SGLUpload() {};
    
    bool* uploaded = nullptr;
    virtual void upload() = 0;
    virtual void unload() = 0;
    
    bool canceled = false;
    
};


/******************************************************************************
 *  Defiunition for upload system                                             *
 ******************************************************************************/

#define UPLOADS_INFINITE -1

class SGLUploadSystem : public SSubsystem {
    
    public:
    
        static void startup();
        static void shutdown();
    
        static void addUpload(SGLUpload* upload);
    
        static void setUploadLimitPerFrame(int _num_uploads);
        static void processUploads();
    
    private:
    
        static std::queue<SGLUpload*> upload_queue;
        static int num_uploads;
    
    
};

#endif /* SGLUpload_hpp */
