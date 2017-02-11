//
//  SLightingConstants.h
//  Spud Engine 2
//
//  Created by Logan Pazol on 10/1/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SLightingConstants_h
#define SLightingConstants_h

/******************************************************************************
 *  Definition for lighting constants                                         *
 ******************************************************************************/

#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_DIRECTIONAL 2
#define LIGHT_TYPE_SPOT 3

#define SHADOW_MAP_ATLAS_SIZE 8192
#define SHADOW_MAP_ATLAS_TILE_SIZE 1024
#define SHADOW_MAP_ATLAS_TILE_COUNT SHADOW_MAP_ATLAS_SIZE / SHADOW_MAP_ATLAS_TILE_SIZE

#define LIGHT_DIRECTIONAL_NEAR -5000.0f
#define LIGHT_DIRECTIONAL_FAR 5000.0f

#define LIGHT_GRID_TILE_SIZE 32

#endif /* SLightingConstants_h */
