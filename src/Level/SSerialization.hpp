//
//  SSerialization.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 3/6/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#ifndef SSerialization_hpp
#define SSerialization_hpp

#include "SLog.hpp"
#include "SLevelFactory.hpp"
#include "SResourceManager.hpp"

/******************************************************************************
 *  Definition for serializer que items									      *
 ******************************************************************************/

struct SSerializerQueueItem {
	
	virtual ~SSerializerQueueItem() { /* intentionally emptry */ }
	
	virtual size_t getSize() = 0;
	virtual void copy(void* data) = 0;
	
};

template <class T>
struct SSerializerQueueItemDerrived : public SSerializerQueueItem {
	
	SSerializerQueueItemDerrived(T* _item) {
		
		// Save the item
		item = _item;
		
	}
	
	virtual size_t getSize() { return sizeof(*item); }
	
	virtual void copy(void* data) {
		
		// Assume the block that we are going into is big enough to hold the item
		// Copy it into memory
		memcpy(data, item, sizeof(*item));
		
	}
	
	T* item;
	
};

template <class T>
struct SSerializerQueueItemStorage : public SSerializerQueueItem {
	
	T value;
	
	virtual T getSize() { return sizeof(T); }
	
	virtual void copy(void* data) {
		
		// Copy the hash
		memcpy(data, &value, sizeof(T));
		
	}
	
};

/******************************************************************************
 *  Definition for serialized data											  *
 ******************************************************************************/

struct SSerializedData {
	
	SSerializedData(size_t _size);
	~SSerializedData();
	
	char* data;
	size_t size;
	
};

/******************************************************************************
 *  Definition for serializer											      *
 ******************************************************************************/

class SSerializer {
	
	public:
	
		template <class T>
		void addItem(T* item) {
		
			// Create a new queue node to hold this item and add it
			SSerializerQueueItemDerrived<T>* storage = new SSerializerQueueItemDerrived<T>(item);
			item_queue.emplace_back(storage);
			size = size + storage->getSize();
		
		}
	
		template <class T>
		void addValue(const T& value) {
		
			// Go to the level factory and retrieve the hash
			SSerializerQueueItemStorage<T>* storage = new SSerializerQueueItemStorage<T>();
			storage->value = value;
		
			item_queue.emplace_back(storage);
			size = size + storage->getSize();
		
		}
	
		template <class T>
		void startClass() {
		
			// Go to the level factory and retrieve the hash
			SSerializerQueueItemStorage<size_t>* class_storage = new SSerializerQueueItemStorage<size_t>();
			class_storage->value = SLevelFactory::getClassHash<T>();
			
			item_queue.emplace_back(class_storage);
			size = size + class_storage->getSize();
		
		}

		SSerializedData* serialize();
	
		void addResource(std::shared_ptr<SResource> resource);
		const std::vector<std::string>& getPaths() const;
	
	private:
	
		std::vector<std::string> resource_paths;
		std::vector<SSerializerQueueItem*> item_queue;
		size_t size;
	
};

/******************************************************************************
 *  Definition for de-serializer											  *
 ******************************************************************************/

class SDeserializer {
	
	public:
	
		SDeserializer(SSerializedData* _data, const std::vector<std::string>& paths);
		~SDeserializer();
	
		template <class T>
		void deserialize(T* location) {
		
			// Make sure we have enough data left to get this kind of object
			if (data->size - offset >= sizeof(T)) {
		
				// Copy the memory into the destination
				memcpy(location, data->data + offset, sizeof(T));
				
				// Add offset
				offset = offset + sizeof(T);
				
			} else SLog::verboseLog(SVerbosityLevel::Critical, "Did not have enough data to serialize object\n");
		
		}
	
		template <class T>
		std::shared_ptr<T> deserializeResource() {
			
			if (data->size - offset >= sizeof(size_t)) {
				
				// Get the hash of the resource
				size_t hash;
				memcpy(&hash, data->data + offset, sizeof(size_t));
				
				// Add offset
				offset = offset + sizeof(size_t);
				
				// Access the resource (assume it to be loaded)
				return SResourceManager::getResource<T>(hashed_paths[hash]);
				
			}
			
			SLog::verboseLog(SVerbosityLevel::Critical, "Did not have enough data to serialize object\n");
			return nullptr;
			
		}
	
	private:
	
		std::map<size_t, std::string> hashed_paths;
		SSerializedData* data;
		size_t offset = 0;
	
};

/******************************************************************************
 *  Definition for serializable class                                         *
 ******************************************************************************/

class SSerializable {
	
	public:
	
		virtual void serialize(SSerializer& serializer) = 0;
		virtual void deserialize(SDeserializer& deserializer) = 0;
	
};

#endif /* SSerialization_hpp */
