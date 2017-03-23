//
//  SSerialization.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 3/6/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#include "SSerialization.hpp"

/******************************************************************************
 *  Implemetation for serialized data										  *
 ******************************************************************************/

SSerializedData::SSerializedData(size_t _size) {

	// Create a data block of the proper size
	size = _size;
	data = (char*)malloc(size);

}

SSerializedData::~SSerializedData() { free(data); }

/******************************************************************************
 *  Implemetation for serializer							     		      *
 ******************************************************************************/

SSerializedData* SSerializer::serialize() {
	
	// The first thing we need to do is create a block of memory large enough
	SSerializedData* data = new SSerializedData(size);
	
	// Now go through each of the items and have them copy in their data
	size_t offset = 0;
	
	for (int i = 0; i < item_queue.size(); i++) {
		
		item_queue[i]->copy(data->data + offset);
		offset = offset + item_queue[i]->getSize();
		
		// Delete it (dynamically allocated)
		delete item_queue[i];
		
	}
	
	// Clear the queue
	item_queue.clear();
	
	// Clear the paths
	resource_paths.clear();
	
	// Return the data
	return data;
	
}

void SSerializer::addResource(SResource* resource) {
	
	// Add the resource hash
	SSerializerQueueItemStorage<size_t>* resource_storage = new SSerializerQueueItemStorage<size_t>();
	resource_storage->value = resource->getHash();
	
	item_queue.emplace_back(resource_storage);
	size = size + resource_storage->getSize();

	// Save the path
	resource_paths.push_back(resource->getPath().getPathAsString());
	
}

const std::vector<std::string>& SSerializer::getPaths() const { return resource_paths; }

/******************************************************************************
 *  Implementation for de-serializer									      *
 ******************************************************************************/

SDeserializer::SDeserializer(SSerializedData* _data) { data = _data; }
SDeserializer::~SDeserializer() { delete data; }
