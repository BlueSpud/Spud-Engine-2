//
//  SCamera.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/10/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SCamera.hpp"

glm::vec3 SCamera::view_position;
REGISTER_UNIFORM(&SCamera::view_position, "view_position", UNIFORM_VEC3, 1);

SCamera* SCamera::current_camera;;

/******************************************************************************
 *  Implementation for camera                                                 *
 ******************************************************************************/

glm::mat4 SCamera::getCameraMatrix() const {
    
    glm::mat4 to_return = glm::mat4(1.0);
    
    //Perform the operations on the matrix (similar to the one in SGL but inverse and no scale)
    to_return = glm::rotate(to_return, -(float)(M_PI - transform.rotation.z), z_axis);
    to_return = glm::rotate(to_return, -(float)(M_PI - transform.rotation.x), x_axis);
    to_return = glm::rotate(to_return, -(float)(M_PI - transform.rotation.y), y_axis);
    to_return = glm::translate(to_return, -transform.translation);
    
    return to_return;
    
}

glm::mat4 SCamera::getCameraMatrix(double interpolation) const {
    
    glm::mat4 to_return = glm::mat4(1.0);
    
    //Perform the operations on the matrix (similar to the one in SGL but inverse and no scale)
    to_return = glm::rotate(to_return, -(float)(M_PI - transform.rotation.z - transform.rotation_velocity.z * interpolation), z_axis);
    to_return = glm::rotate(to_return, -(float)(M_PI - transform.rotation.x - transform.rotation_velocity.x * interpolation), x_axis);
    to_return = glm::rotate(to_return, -(float)(M_PI - transform.rotation.y - transform.rotation_velocity.y * interpolation), y_axis);
    to_return = glm::translate(to_return, -transform.translation - transform.translation_velocity * (float)interpolation);
    
    return to_return;
    
}

glm::mat4 SCamera::translateToCameraSpace() const {
    
    // Get the matrix and upload it
    glm::mat4 mat = getCameraMatrix();
    SGL::mulMatrix(mat, MAT_VIEW);
    
    // Store the view position where everyone can see it
    view_position = transform.translation;
    
    return mat;
    
}

glm::mat4 SCamera::translateToCameraSpace(double interpolation) const {
    
    // Get the matrix and upload it
    glm::mat4 mat = getCameraMatrix(interpolation);
    SGL::mulMatrix(mat, MAT_VIEW);
    
    // Store the view position where everyone can see it
    view_position = transform.translation + transform.translation_velocity * (float)interpolation;
    
    return mat;
    
}

glm::vec3* SCamera::getFrustumWithPlanes(glm::mat4& projection_matrix, glm::mat4& view_matrix, float near_plane, float far_plane) {
    
    // Create a place to store the frustrum corners
    glm::vec3* frustrum_corners = new glm::vec3[8];
    glm::mat4 inverse_projection_view_matrix = glm::inverse(projection_matrix * view_matrix);

    // Get the near and far planes that we specifies in world space in clip-space
    glm::vec4 near_plane_p = projection_matrix * glm::vec4(0.0, 0.0, -near_plane, 1.0);
    float near_plane_a = near_plane_p.z / near_plane_p.w;
    
    glm::vec4 far_plane_p = projection_matrix * glm::vec4(0.0, 0.0, -far_plane, 1.0);
    float far_plane_a = far_plane_p.z / far_plane_p.w;
    
    // Create the set of corners in clip space to be unprojected
    glm::vec4 corners[8] {
        glm::vec4(-1.0,  1.0, near_plane_a, 1.0),
        glm::vec4( 1.0,  1.0, near_plane_a, 1.0),
        glm::vec4( 1.0, -1.0, near_plane_a, 1.0),
        glm::vec4(-1.0, -1.0, near_plane_a, 1.0),
        glm::vec4(-1.0,  1.0, far_plane_a,  1.0),
        glm::vec4( 1.0,  1.0, far_plane_a,  1.0),
        glm::vec4( 1.0, -1.0, far_plane_a,  1.0),
        glm::vec4(-1.0, -1.0, far_plane_a,  1.0)
    };
    
    // Unproject the corners of the frustum
    for (int i = 0; i < 8; i++) {
        
        glm::vec4 corner_proj = inverse_projection_view_matrix * corners[i];
        frustrum_corners[i] = glm::vec3(corner_proj) / corner_proj.w;
    }

    return frustrum_corners;
}
