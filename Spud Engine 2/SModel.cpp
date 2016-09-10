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

REGISTER_RESOURCE_CLASS(obj, SModel);

/******************************************************************************
 *  Functions for model                                                       *
 ******************************************************************************/

// TEMP need to switch to indexed VBOs

SResource* SModel::allocate() { return new SModel(); }

bool SModel::load(const SPath& path) {

    SFile* file = SFileSystem::loadFile(path);
    
    if (file) {
    
        std::string line;
        
        std::vector<glm::vec3>_verts;
        std::vector<glm::vec3>_normals;
        std::vector<glm::vec2>_tex_coords;
        std::vector<std::string>_indicies;
        
        // WARNING:
        // .obj indicies start at one and therefore we need to add 1 to every one we parse
        _verts.push_back(glm::vec3());
        _normals.push_back(glm::vec3());
        _tex_coords.push_back(glm::vec2());
        
        // Itterate over every line in the file
        while (file->getNextTokenWithDeliminator('\n', line)) {
            
            // Parse verticies
            if (line.compare(0, 2, "v ") == 0) {
                
                glm::vec3 v;
                sscanf(line.c_str(), "v %f %f %f",&v.x,&v.y,&v.z);
                
                _verts.push_back(v);
            }
            
            // Parse normals
            if (line.compare(0, 2, "vn") == 0) {
                
                glm::vec3 n;
                sscanf(line.c_str(), "vn %f %f %f",&n.x,&n.y,&n.z);
                
                _normals.push_back(n);
            }
            
            // Parse texture coordinates
            if (line.compare(0, 2, "vt") == 0) {
                
                glm::vec2 t;
                sscanf(line.c_str(), "vt %f %f",&t.x,&t.y);
                
                _tex_coords.push_back(t);
            }
            
            // Parse indicies
            if (line.compare(0, 1, "f") == 0) {
                
                // Scan later because we need all of the other data to be ready
                _indicies.push_back(line);
            }
            
        }
        
        // We know how many verticies, normals and text coordinates that we have that are unique so we can allocate them
        verts = new float[_indicies.size() * 9];
        normals = new float[_indicies.size() * 9];
        tex_coords = new float[_indicies.size() * 6];
        
        // Save the number of faces we are going to have
        face_count = (unsigned int)_indicies.size();
        
        for (int i = 0; i < _indicies.size(); i++) {
            
            // Parse the face
            glm::ivec3 vi,ni,ti;
            sscanf(_indicies[i].c_str(), "f %i/%i/%i %i/%i/%i %i/%i/%i",&vi.x,&ti.x,&ni.x, &vi.y,&ti.y,&ni.y, &vi.z,&ti.z,&ni.z);
            
            // Fill all the float buffers
            verts[i*9] = _verts[vi.x].x; verts[i*9 + 1] = _verts[vi.x].y; verts[i*9 + 2] = _verts[vi.x].z;
            verts[i*9 + 3] = _verts[vi.y].x; verts[i*9 + 4] = _verts[vi.y].y; verts[i*9 + 5] = _verts[vi.y].z;
            verts[i*9 + 6] = _verts[vi.z].x; verts[i*9 + 7] = _verts[vi.z].y; verts[i*9 + 8] = _verts[vi.z].z;
            
            normals[i*9] = _normals[ni.x].x; normals[i*9 + 1] = _normals[ni.x].y; normals[i*9 + 2] = _normals[ni.x].z;
            normals[i*9 + 3] = _normals[ni.y].x; normals[i*9 + 4] = _normals[ni.y].y; normals[i*9 + 5] = _normals[ni.y].z;
            normals[i*9 + 6] = _normals[ni.z].x; normals[i*9 + 7] = _normals[ni.z].y; normals[i*9 + 8] = _normals[ni.z].z;
            
            tex_coords[i*6] = _tex_coords[ti.x].x; tex_coords[i*6 + 1] = _tex_coords[ti.x].y;
            tex_coords[i*6 + 2] = _tex_coords[ti.y].x; tex_coords[i*6 + 3] = _tex_coords[ti.y].y;
            tex_coords[i*6 + 4] = _tex_coords[ti.z].x; tex_coords[i*6 + 5] = _tex_coords[ti.z].y;
            
        }
        
        face_count = (unsigned int)_indicies.size() * 3;

        return true;
        
    }

    return false;
    
}

void SModel::unload() {

    // Check if we havent already deleted these already, we might not have uploaded it yet
    if (!uploded) {
        
        delete verts;
        delete normals;
        delete tex_coords;
        //delete indicies;
        
    } else {
        
        // We had already uploaded the GPU
        glDeleteBuffers(1, &verts_id);
        glDeleteBuffers(1, &normals_id);
        glDeleteBuffers(1, &tex_coords_id);
        //glDeleteBuffers(1, &indicies_id);
        
    }

}

void SModel::upload() {

    // Generate the buffers and fill them with their data
    glGenBuffers(1, &verts_id);
    glBindBuffer(GL_ARRAY_BUFFER, verts_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * face_count, verts, GL_STATIC_DRAW);
    
    
    glGenBuffers(1, &normals_id);
    glBindBuffer(GL_ARRAY_BUFFER, normals_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * face_count, normals, GL_STATIC_DRAW);
    
    glGenBuffers(1, &tex_coords_id);
    glBindBuffer(GL_ARRAY_BUFFER, tex_coords_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * face_count, tex_coords, GL_STATIC_DRAW);
    
    //glGenBuffers(1, &indicies_id);
    
    // Clear the buffer just in case
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // Delete the buffers in regular RAM
    delete verts;
    delete normals;
    delete tex_coords;
    //delete indicies;

    // Make sure we know if we uploaded
    uploded = true;

}
