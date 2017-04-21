//
//  SLevel.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 3/24/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#include "SLevel.hpp"

SLevel* SLevelManager::current_level = nullptr;

/******************************************************************************
 *  Implementation for level												  *
 ******************************************************************************/

SLevel::SLevel() {
	
	// Create an empty level
	scene_graph = new SOctreeSceneGraph();
	scene_graph->light_graph = new SOctreeLightGraph();
	
	// Make this level current
	scene_graph->makeCurrent();
	SLevelManager::current_level = this;
	
	// We were good
	good = true;
	
}

SLevel::SLevel(const SPath& path) {

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
		scene_graph->light_graph = new SOctreeLightGraph();
	
		for (int i = 0; i < light_count; i++) {
			
			// Get the class hash
			size_t class_hash;
			deserializer.deserialize(&class_hash);
			
			SLight* light = SLevelFactory::allocate<SLight>(class_hash);
			light->deserialize(deserializer);
			
			scene_graph->addObject(light);
			
		}
		
		// Close the file, this is not a resource so we dont maintain it
		SFileSystem::closeFile(file);
		
		// We were good
		good = true;
		
	}

}

SLevel::~SLevel() {
	
	// Delete the scene graph
	if (scene_graph)
		delete scene_graph;
	
}

void SLevel::saveLevel(const SPath& path) {
	
	// Collect the objects
	std::vector<SObject*> scene_objects;
	std::vector <SObject*> lights;
	
	scene_graph->linearizeObjects(scene_objects);
	scene_graph->light_graph->linearizeLights(lights);
	
	// Serialize all the objects, order is regular objects and then lights
	SSerializer s;
	
	for (int i = 0; i < scene_objects.size(); i++)
		scene_objects[i]->serialize(s);
	
	for (int i = 0; i < lights.size(); i++)
		lights[i]->serialize(s);
	
	SFileWritable* file = SFileSystem::openWritableFile(path);
	
	// Collect the paths we used for resources
	const std::vector<std::string>& paths = s.getPaths();
	unsigned int writable_int = (unsigned int)paths.size();

	file->write(&writable_int, sizeof(unsigned int));
	
	// Write them out
	for (int i = 0; i < paths.size(); i++) {
	
		writable_int = (unsigned int)paths[i].length();
		file->write(&writable_int, sizeof(unsigned int));
	
		char* c = const_cast<char*>(paths[i].c_str());
		file->write(c, writable_int * sizeof(char));
	
	}
	
	// Object count
	writable_int = (unsigned int)scene_objects.size();
	file->write(&writable_int, sizeof(unsigned int));
	
	// Light count
	writable_int = (unsigned int)lights.size();
	file->write(&writable_int, sizeof(unsigned int));
	
	// Write out the object data
	SSerializedData* data_s = s.serialize();
	
	file->write(&data_s->size, sizeof(size_t));
	file->write(data_s->data, data_s->size);
	
	delete data_s;
		
	file->close();
	
}

/******************************************************************************
 *  Implementation for level manager										  *
 ******************************************************************************/

void SLevelManager::loadLevel(const SPath& path) {
	
	// Create a new thread for the level
	SLog::verboseLog(SVerbosityLevel::Debug, "Loading level %s", path.getPathAsString().c_str());
	boost::thread thread = boost::thread(&loadLevelThreaded, path);
	
}

void SLevelManager::loadLevelThreaded(const SPath& path) {
	
	// Keep track of the old level
	SLevel* old_level = current_level;
	
	// First we load up the level
	current_level = new SLevel(path);

	// If the level was good, we can change to that level
	if (current_level->good) {
		
		current_level->scene_graph->makeCurrent();
		
		// If there was an old level, unload it
		if (old_level) {
			
			// Delete the old level's physics graph, not done in the destructor because the physics system is responsible for the current graph upon shutdown
			delete old_level->scene_graph->physics_graph;
			delete old_level;
			
			// Unload any unused resources
			SResourceManager::purge();
			
		}
		
	} else {
		
		// Level was bad
		delete current_level;
		current_level = old_level;
		
		SLog::verboseLog(SVerbosityLevel::Critical, "Failed to load level %s", path.getPathAsString().c_str());
		
	}
	
	
}

void SLevelManager::saveLevel(const SPath& path) {
	
	// If there is a level, save it
	if (current_level) {
	
		// Create a new thread for the level
		SLog::verboseLog(SVerbosityLevel::Debug, "Saving level %s", path.getPathAsString().c_str());
		boost::thread thread = boost::thread(&saveLevelThreaded, path);
		
	}
	
}

void SLevelManager::saveLevelThreaded(const SPath& path) { current_level->saveLevel(path); }

void SLevelManager::createLevel() {

	// Create a new blank level
	current_level = new SLevel();
	current_level->scene_graph->makeCurrent();
	
}

SObject* SLevelManager::pickObject(const glm::vec3& origin, const glm::vec3& direction, float length) {
	
	// Pick from the current level if it exists
	if (current_level)
		return current_level->scene_graph->pickObject(origin, direction, length);
	
	return nullptr;
	
}
