//
//  SLevel.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 3/24/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#ifndef SLevel_hpp
#define SLevel_hpp

#include <boost/thread.hpp>

#include "SOctreeSceneGraph.hpp"
#include "SOctreeLightGraph.hpp"

// Forward declarations
class SLevelManager;

/******************************************************************************
 *  Declaration for level													  *
 ******************************************************************************/

class SLevel {
	
	friend class SLevelManager;
	
	public:
	
		virtual ~SLevel();
	
	protected:
	
		SLevel();
		SLevel(const SPath& path);
	
		void saveLevel(const SPath& path);
	
		bool good = false;
		SOctreeSceneGraph* scene_graph = nullptr;
	
};

/******************************************************************************
 *  Declaration for level manager											  *
 ******************************************************************************/

class SLevelManager {
	
	friend class SLevel;
	
	public:
	
		template <class T>
		static void spawnObject(T* object) { current_level->scene_graph->addObject(object); }
		static void loadLevel(const SPath& path);
		static void saveLevel(const SPath& path);
		static void createLevel();
	
	private:
	
		static void loadLevelThreaded(const SPath& path);
		static void saveLevelThreaded(const SPath& path);
	
		static SLevel* current_level;
	
};


#endif /* SLevel_hpp */
