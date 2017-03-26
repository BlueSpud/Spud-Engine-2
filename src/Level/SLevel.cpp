//
//  SLevel.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 3/24/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#include "SLevel.hpp"

/******************************************************************************
 *  Registration for supported level extensions                               *
 ******************************************************************************/

REGISTER_RESOURCE_CLASS(slevel, SLevel)

/******************************************************************************
 *  Implementation for level												  *
 ******************************************************************************/

bool SLevel::load(const SPath& path) {

	// Open up the file
	SFile* file = SFileSystem::loadFile(path, true);
	if (file) {
		
		// The first number we need is the number of paths we are going to read
		unsigned int path_num;
		file->read(&path_num, sizeof(unsigned int));
		
		std::vector<std::string> paths;
		
		for (int i = 0; i < path_num; i++) {
			
			// Read the size of the path and make space for it
			// When we save the level format we dont save the null terminator so we make room for it here
			unsigned int path_size;
			file->read(&path_size, sizeof(unsigned int));
			char* path = new char[path_size + 1];
			path[path_size] = 0;
			
			// Skip the null terminator
			file->read(path, path_size);
			paths.push_back(path);
			
			delete [] path;
			
		}
		
		// Read the num objects
		unsigned int object_count;
		file->read(&object_count, sizeof(unsigned int));
		
		// Read the num lights
		unsigned int light_count;
		file->read(&light_count, sizeof(unsigned int));
		
		// Read the size of the serialized data
		size_t data_size;
		file->read(&data_size, sizeof(size_t));
		
		// Get the serialized data
		SSerializedData* data_s = new SSerializedData(data_size);
		file->read(data_s->data, data_size);
		
		// Create the scene graph
		scene_graph = new SOctreeSceneGraph();
		
		// Now we create the objects, these come from the serialized data
		SDeserializer deserializer = SDeserializer(data_s, paths);
		
		for (int i = 0; i < object_count; i++) {
		
			// Get the class hash
			size_t class_hash;
			deserializer.deserialize(&class_hash);
			
			SObject* object = SLevelFactory::allocate<SObject>(class_hash);
			object->deserialize(deserializer);
		
			scene_graph->addObject(object);
		
		}
		
		// Create the light graph and the lights
		SSimpleLightGraph* light_graph = new SSimpleLightGraph();
	
		for (int i = 0; i < light_count; i++) {
			
			// Get the class hash
			size_t class_hash;
			deserializer.deserialize(&class_hash);
			
			SLight* light = SLevelFactory::allocate<SLight>(class_hash);
			light->deserialize(deserializer);
			
			light_graph->addLight(light);
			
		}
		
		// TEMP, set as current
		scene_graph->makeCurrent();
		SRenderSystem::current_light_graph = light_graph;
		
		return true;
		
	}
	
	return false;

}

void SLevel::unload() {}
