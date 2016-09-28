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

/******************************************************************************
 *  Functions for camera                                                      *
 ******************************************************************************/

glm::mat4 SCamera::getCameraMatrix() {
    
    glm::mat4 to_return = glm::mat4(1.0);
    
    //Perform the operations on the matrix (similar to the one in SGL but inverse and no scale)
    to_return = glm::rotate(to_return, -(float)(M_PI - transform.rotation.z), z_axis);
    to_return = glm::rotate(to_return, -(float)(M_PI - transform.rotation.x), x_axis);
    to_return = glm::rotate(to_return, -(float)(M_PI - transform.rotation.y), y_axis);
    to_return = glm::translate(to_return, -transform.translation);
    
    return to_return;
    
}

glm::mat4 SCamera::getCameraMatrix(double interpolation) {
    
    glm::mat4 to_return = glm::mat4(1.0);
    
    //Perform the operations on the matrix (similar to the one in SGL but inverse and no scale)
    to_return = glm::rotate(to_return, -(float)(M_PI - transform.rotation.z - transform.rotation_velocity.z * interpolation), z_axis);
    to_return = glm::rotate(to_return, -(float)(M_PI - transform.rotation.x - transform.rotation_velocity.x * interpolation), x_axis);
    to_return = glm::rotate(to_return, -(float)(M_PI - transform.rotation.y - transform.rotation_velocity.y * interpolation), y_axis);
    to_return = glm::translate(to_return, -transform.translation - transform.translation_velocity * (float)interpolation);
    
    return to_return;
    
}

void SCamera::translateToCameraSpace() {
    
    // Get the matrix and upload it
    glm::mat4 mat = getCameraMatrix();
    SGL::mulMatrix(mat, MAT_VIEW_MATRIX);
    
    // Store the view position where everyone can see it
    view_position = transform.translation;
    
}

void SCamera::translateToCameraSpace(double interpolation) {
    
    // Get the matrix and upload it
    glm::mat4 mat = getCameraMatrix(interpolation);
    SGL::mulMatrix(mat, MAT_VIEW_MATRIX);
    
    // Store the view position where everyone can see it
    view_position = transform.translation;
    
}
