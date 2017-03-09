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

/******************************************************************************
 *  Definition for serializer que items									      *
 ******************************************************************************/

struct SSerializerQueueItem {
	
	virtual ~SSerializerQueueItem() { /* intentionally emptry */ }
	
	virtual size_t getSize() = 0;
	virtual void copy(void* data) = 0;
	
};

template <class T>
struct SSerializerQueueItemDerrived : SSerializerQueueItem {
	
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
			item_queue.emplace_back(new SSerializerQueueItemDerrived<T>(item));
			size = size + sizeof(*item);
		
		}

		SSerializedData* serialize();
	
	private:
	
		std::vector<SSerializerQueueItem*> item_queue;
		size_t size;
	
};

/******************************************************************************
 *  Definition for de-serializer											  *
 ******************************************************************************/

class SDeserializer {
	
	public:
	
		SDeserializer(SSerializedData* _data);
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
	
	private:
	
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
