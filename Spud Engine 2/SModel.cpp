//
//  SModel.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/2/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SModel.hpp"

/******************************************************************************
 *  Registration for supported model extensions                               *
 ******************************************************************************/

REGISTER_RESOURCE_CLASS(smdl, SModel);

/******************************************************************************
 *  Functions for model                                                       *
 ******************************************************************************/

SResource* SModel::allocate() { return new SModel(); }

void SModel::render() {
    
    // Bind the array and then draw
    glBindVertexArray(array_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_ids[buffer_indicies]);
    
    int sum = 0;
    for (int i = 0; i < draw_calls.size(); i++) {
        
        // Draw the array with an offset based on what we have already drawn
        glDrawElements(GL_TRIANGLES, draw_calls[i], GL_UNSIGNED_INT, (GLubyte*)(sizeof(unsigned int) * sum));
        sum += draw_calls[i];
        
    }
    
    // Dont need to unbind becasue the next thing that draws should bind
    //glBindVertexArray(0);
    
}

void SModel::getModelExtents(glm::vec3& _mins, glm::vec3& _maxes) {
    
    // Save out the extents of the model
    _mins = mins;
    _maxes = maxes;
    
}

bool SModel::load(const SPath& path) {
    
    file = SFileSystem::loadFile(path, true);
    
    if (file != nullptr) {
    
        // Read the header, the min and max of the bounding box
        glm::vec3 extents[2];
        file->read(extents, sizeof(glm::vec3) * 2);
        mins = extents[0];
        maxes = extents[1];
        
        // Grab the face count and vertex count
        unsigned int vertex_count;
        file->read((char*)&vertex_count, sizeof(unsigned int));
        
        // Allocate the arrays for the data
        verts = new glm::vec3[vertex_count];
        normals = new glm::vec3[vertex_count];
        tex_coords = new glm::vec2[vertex_count];
        tangents = new glm::vec3[vertex_count];
        
        // Read the verticies
        for (int i = 0; i < vertex_count; i++) {
                
            // Read position
            file->read(&verts[i], sizeof(glm::vec3));
                
            // Read normal
            file->read(&normals[i], sizeof(glm::vec3));
                
            // Read tangent
            file->read(&tangents[i], sizeof(glm::vec3));
                
            // Read tex coord
            file->read(&tex_coords[i], sizeof(glm::vec2));
                
        }
        
        // Create a vector for the indicies
        indicies = new std::vector<glm::ivec3>();
        
        while (true) {
            
            // Read the next token
            int token;
            file->read((char*)&token, sizeof(int));
            
            // If end of file was reached, we are done
            if (token == END_OF_FILE_TOKEN)
                break;
            
            if (token == NEW_MATERIAL_TOKEN) {
                
                // Read the index count
                unsigned int index_count;
                file->read((char*)&index_count, sizeof(unsigned int));
                
                // Save the number of indicies
                draw_calls.push_back(index_count * 3);
                
                for (int j = 0; j < index_count; j++) {
                    
                    // Read the indicies
                    indicies->push_back(glm::ivec3());
                    file->read((char*)&indicies->back(), sizeof(glm::ivec3));
                }
                
            }
            
        }
        
        // Create an upload
        upload = new SModelUpload();
        upload->verts = verts;
        upload->normals = normals;
        upload->tex_coords = tex_coords;
        upload->tangents = tangents;
        
        upload->indicies = indicies;
        
        upload->face_count = (unsigned int)indicies->size();
        upload->vertex_count = (unsigned int)vertex_count;
        upload->array_id = &array_id;
        upload->buffer_ids = buffer_ids;
        upload->uploaded = &uploaded;
        
        SGLUploadSystem::addUpload(upload);

        return true;
        
    }

    return false;
    
}

void SModel::unload() {

    // Check if we havent already deleted these already, we might not have uploaded it yet
    if (uploaded) {
        
        // We had already uploaded the GPU
        glDeleteBuffers(buffer_count, buffer_ids);
        glDeleteVertexArrays(1, &array_id);
        
    }

}

void SModel::hotload(const SPath& path) {
    
    // Delete the last texture
    if (uploaded) {
        
        // Send a deletion command
        SModelUnload* unload = new SModelUnload();
        unload->array_id = array_id;
        
        // Copy the array
        for (int i = 0; i < buffer_count; i++)
            unload->buffer_ids[i] = buffer_ids[i];
        
        SGLUploadSystem::addUpload(unload);
        
    } else {
        
        // Cancel the upload we had already sent
        upload->canceled = true;
        
        // Free the stuff we have
        upload->unload();
        
    }
    
    
    // Close the old file and then load the new one
    SFileSystem::closeFile(file);
    load(path);
    
}

/******************************************************************************
 *  Functions for model upload                                                *
 ******************************************************************************/

void SModelUpload::upload() {

    // Generate the buffer for indicies
    
    // Generate the array and the buffers
    glGenVertexArrays(1, array_id);
    glGenBuffers(buffer_count, buffer_ids);
    
    // Bind the array and fill the buffers
    glBindVertexArray(*array_id);
    
    glBindBuffer(GL_ARRAY_BUFFER, buffer_ids[buffer_position]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertex_count, verts, GL_STATIC_DRAW);
    
    // Tell OpenGL how to read this
    glEnableVertexAttribArray(buffer_position);
    glVertexAttribPointer(buffer_position, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, buffer_ids[buffer_normal]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertex_count, normals, GL_STATIC_DRAW);

    // Tell OpenGL how to read this
    glEnableVertexAttribArray(buffer_normal);
    glVertexAttribPointer(buffer_normal, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, buffer_ids[buffer_tex_coord]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * vertex_count, tex_coords, GL_STATIC_DRAW);
    
    // Tell OpenGL how to read this
    glEnableVertexAttribArray(buffer_tex_coord);
    glVertexAttribPointer(buffer_tex_coord, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    
    // Tell OpenGL how to read this
    glBindBuffer(GL_ARRAY_BUFFER, buffer_ids[buffer_tangent]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertex_count, tangents, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(buffer_tangent);
    glVertexAttribPointer(buffer_tangent, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    
    // Fill up the indicies buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_ids[buffer_indicies]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::ivec3) * face_count, indicies->data(), GL_STATIC_DRAW);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    unload();

    // Make sure we know if we uploaded
    *uploaded = true;

}

void SModelUpload::unload() {
    
    // Delete the buffers in regular RAM
    delete verts;
    delete normals;
    delete tex_coords;
    delete tangents;
    delete indicies;
    
}

/******************************************************************************
 *  Functions for model unload                                                *
 ******************************************************************************/

void SModelUnload::upload() {
    
    // Delete model data
    glDeleteBuffers(buffer_count, buffer_ids);
    glDeleteVertexArrays(1, &array_id);
}

void SModelUnload::unload() { /* nothing */ }
