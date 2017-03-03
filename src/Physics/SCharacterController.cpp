//
//  SCharacterController.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 12/22/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SCharacterController.hpp"

/******************************************************************************
 *  Data for cylinder to prevent capsule sliding                              *
 ******************************************************************************/

physx::PxVec3 cylinder_data[16] = {physx::PxVec3( 0.000000, -0.500000, -1.000000),
								   physx::PxVec3( 0.000000,  0.500000, -1.000000),
								   physx::PxVec3( 0.707107, -0.500000, -0.707107),
								   physx::PxVec3( 0.707107,  0.500000, -0.707107),
								   physx::PxVec3( 1.000000, -0.500000,  0.000000),
								   physx::PxVec3( 1.000000,  0.500000,  0.000000),
								   physx::PxVec3( 0.707107, -0.500000,  0.707107),
								   physx::PxVec3( 0.707107,  0.500000,  0.707107),
								   physx::PxVec3(-0.000000, -0.500000,  1.000000),
								   physx::PxVec3(-0.000000,  0.500000,  1.000000),
								   physx::PxVec3(-0.707107, -0.500000,  0.707107),
								   physx::PxVec3(-0.707107,  0.500000,  0.707107),
								   physx::PxVec3(-1.000000, -0.500000, -0.000000),
								   physx::PxVec3(-1.000000,  0.500000, -0.000000),
								   physx::PxVec3(-0.707107, -0.500000, -0.707107),
								   physx::PxVec3(-0.707107,  0.500000, -0.707107)};

void SCharacterController::createCylinder(glm::vec2& size) {
	
	// Modify the data to have the proper scale
	physx::PxVec3 cylinder_data_mod[16];
	
	for (int i = 0; i < 16; i++)
		cylinder_data_mod[i] = physx::PxVec3(cylinder_data[i].x * size.x, cylinder_data[i].y * size.y, cylinder_data[i].z * size.x);
	
	// Cook the cylinder
	physx::PxConvexMeshDesc convex_desc;
	convex_desc.points.count = 16;
	convex_desc.points.data = cylinder_data_mod;
	convex_desc.points.stride = sizeof(physx::PxVec3);
	convex_desc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;
	convex_desc.vertexLimit = 255;
	
	// Cook the dynamic mesh
	physx::PxDefaultMemoryOutputStream convex_cooked_buffer;
	if (SPhysicsSystem::getCooking()->cookConvexMesh(convex_desc, convex_cooked_buffer)) {
		
		// Cooking sucess
		physx::PxDefaultMemoryInputData read_buffer = physx::PxDefaultMemoryInputData(convex_cooked_buffer.getData(),
																					  convex_cooked_buffer.getSize());
		
		cylinder_mesh = PxGetPhysics().createConvexMesh(read_buffer);
		cylinder = new physx::PxConvexMeshGeometry(cylinder_mesh);
		
	}
	
	
//	physx::PxShape* physx_shape = PxGetPhysics().createShape(*cylinder, *PxGetPhysics().createMaterial(0.5, 0.5, 0.1));
//	
//	// Create a rigid body, static or dynamic, with an identity transform
//	physx::PxTransform transform = physx::PxTransform(physx::PxVec3(0.0, 0.0, 0.0));
//	physx::PxRigidStatic* rigid_body = PxGetPhysics().createRigidStatic(transform);
//	rigid_body->attachShape(*physx_shape);
//	
//	physx_controller->getScene()->addActor(*rigid_body);

	
}

/******************************************************************************
 *  Implementation for character controller                                   *
 ******************************************************************************/

SCharacterController::SCharacterController(SPhysicsGraph* physics_graph,
                                           physx::PxMaterial* material,
                                           glm::vec2 size,
                                           float _step_size,
                                           float _slope_limit,
                                           STransform* _parent_transform) {
    
    parent_transform = _parent_transform;

	// Create the cylinder
	createCylinder(size);
	//cylinder = new physx::PxSphereGeometry(0.5);

	// Create a rigid body, static or dynamic, with an identity transform
	transform = physx::PxTransform(physx::PxVec3(parent_transform->translation.x, parent_transform->translation.y, parent_transform->translation.z));
	
    // Have the event listener listen to the physics ticks
    event_listener.listenToEvent(EVENT_PHYSICS_PREUPDATE, EVENT_MEMBER(SCharacterController::prePhysicsUpdate));
    event_listener.listenToEvent(EVENT_PHYSICS_POSTUPDATE, EVENT_MEMBER(SCharacterController::postPhysicsUpdate));
    
}

void SCharacterController::prePhysicsUpdate(const SEvent& event) {
	
	 const SEventPhysicsUpdate& event_p = (const SEventPhysicsUpdate&)event;
	
	// The direction we plan to move in
	physx::PxVec3 movement_direction = walking_direction * movement_speed;
	
	physx::PxVec3 normal;
	if (isOnGround(normal)) {
		
		movement_direction = movement_direction - normal * normal.dot(movement_direction);
		
	}
	
	
	performMoveSweep((physx::PxVec3(0.0, y_velocity, 0.0) + movement_direction) * event_p.time_elapsed);
	
	if (!isOnGround())
		y_velocity = y_velocity - PHYSICS_G * event_p.time_elapsed;
	else y_velocity = 0.0;
	
	std::cout << transform.p.y << std::endl;

}

void SCharacterController::performMoveSweep(physx::PxVec3 movement_direction) {
	
	//if (movement_direction.magnitude()) {
		
		physx::PxSweepHit hit;
	
		physx::PxVec3 normalized = movement_direction;
		normalized.normalize();
		
		if (SPhysicsSystem::current_physics_graph->physx_scene->sweepSingle(*cylinder, transform, normalized, movement_direction.magnitude(), physx::PxHitFlag::eDISTANCE | physx::PxHitFlag::eNORMAL, hit)) {
		
			//std::cout << "Collision " << hit.distance << std::endl;
			
			if (hit.hadInitialOverlap() || hit.distance == 0.0) {
				
				// Resolve, this may cause some jumping, but its better than a stack overflow
				SPhysicsSystem::current_physics_graph->physx_scene->sweepSingle(*cylinder, transform, normalized, movement_direction.magnitude(),  physx::PxHitFlag::eMTD, hit);
				
				transform.p = transform.p - hit.normal * hit.distance;
				return;
				
			}

			
			// If there was a collision, we need to move as close as we can
			//physx::PxF32 shape_size = 0.2f;
			//physx::PxF32 collision_distance = physx::PxClamp(hit.distance - shape_size, 0.0f, hit.distance);
		
			transform.p = transform.p + movement_direction.getNormalized() * (hit.distance - 0.001f);
			
			physx::PxF64 remaining_dist = physx::PxClamp(movement_direction.magnitude() - (hit.distance - 0.001f), 0.0f, movement_direction.magnitude());
			physx::PxVec3 slide_response = movement_direction - hit.normal * hit.normal.dot(movement_direction);
				
			slide_response = slide_response.getNormalized() * remaining_dist;
			performMoveSweep(slide_response);
	
			
			
			
			
			
			
			
			
		
		
		} else transform.p = transform.p + movement_direction;
		
	//}
	
	
}

void SCharacterController::postPhysicsUpdate(const SEvent& event) {
    
    // Set the parent transform translation, velocity is 0
    // Make sure we're sane
    physx::PxVec3 position = transform.p;
    parent_transform->translation = glm::vec3(position.x, position.y, position.z);
    parent_transform->translation_velocity = glm::vec3(0.0);

}
void SCharacterController::setMoveDirection(glm::vec3 direction) {

    physx::PxVec3 new_walking_direction = physx::PxVec3(direction.x, 0.0, direction.z);
    
    // Check if the direction has a magnitude, if so normalize
	if (glm::length(direction)) {
		
		// Make sure that its normalized
        new_walking_direction.normalize();
		
	} else {
		
		// Fade out the velocity
		if (walking_direction.magnitude() > 0.01)
			new_walking_direction = walking_direction * 0.5;
		
	}
    
    walking_direction = new_walking_direction;

}

bool SCharacterController::isOnGround() {
	
	// Call is on ground and store normal in temp var
	physx::PxVec3 temp_normal;
	return isOnGround(temp_normal);
	
}

bool SCharacterController::isOnGround(physx::PxVec3& normal) {
	
	// Do a sweep downwards, what we are looking for is 0 distance, which would mean that we are on the ground
	physx::PxSweepHit hit;
	
	if (SPhysicsSystem::current_physics_graph->physx_scene->sweepSingle(*cylinder, transform, physx::PxVec3(0.0, -1.0, 0.0), 1.0, physx::PxHitFlag::eDISTANCE | physx::PxHitFlag::eNORMAL, hit)) {

		//std::cout << "Ground distance " << hit.distance << std::endl;
		
		// This means there was a hit, make sure it is what we are looking for
		if (hit.distance <= 0.01) {
		
			//std::cout << "Ground!\n";
			normal = hit.normal;
			return true;
			
		}
		
	}
	
	// There either wasnt a collision or it was too far, not on the ground
	return false;
	
}

void SCharacterController::jump() { y_velocity = y_velocity + jump_vel; }
void SCharacterController::setJumpHeight(float _jump_height) { jump_vel = sqrtf(_jump_height) * SQRT_2G; }
