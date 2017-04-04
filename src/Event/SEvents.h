//
//  SEvents.h
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/4/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SEvents_h
#define SEvents_h

/******************************************************************************
 *  Standard event definition                                                 *
 ******************************************************************************/

struct SEvent {};

#define EVENT_TICK 0
struct SEventTick : SEvent {};

#define EVENT_START_FRAME 1
struct SEventStartFrame : public SEvent {};

#define EVENT_END_FRAME 2
struct SEventEndFrame : public SEvent {};

#endif /* SEvents_h */
