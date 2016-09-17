//
//  SGLUpload.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/15/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SGLUpload.hpp"

/******************************************************************************
 *  Functions for upload que                                                  *
 ******************************************************************************/

std::queue<SGLUpload*> SGLUploadSystem::upload_queue;
int SGLUploadSystem::num_uploads;

void SGLUploadSystem::startup() {

    SLog::verboseLog(SVerbosityLevel::Debug, "SGLUploadSystem startup");
    
}

void SGLUploadSystem::shutdown() {
    
    // Delete uploads
    while (!upload_queue.empty()) {
        
        // Delete the upload
        delete upload_queue.front();
        upload_queue.pop();
        
    }
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SGLUploadSystem shutdown");

}

void SGLUploadSystem::addUpload(SGLUpload* upload) { upload_queue.push(upload); }

void SGLUploadSystem::setUploadLimitPerFrame(int _num_uploads) { num_uploads = _num_uploads; }

void SGLUploadSystem::processUploads() {
    
    // Upload as many times as we possibly can
    int i = 0;
    while ((i < num_uploads || num_uploads == UPLOADS_INFINITE) && !upload_queue.empty()) {
        
        // Perform upload
        SGLUpload* upload = upload_queue.front();
        
        if (!upload->canceled) {
        
            upload->upload();
        
            // Tell whatever wanted an upload we are done
            if (upload->uploaded != nullptr)
                *upload->uploaded = true;
            
        }
        
        // Delete the upload
        delete upload;
        
        upload_queue.pop();
        i++;
        
    }
    
}
