/*
 * Copyright 2019 Side Effects Software Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Produced by:
 *	Side Effects Software Inc.
 *	123 Front Street West, Suite 1401
 *	Toronto, Ontario
 *      Canada   M5J 2M2
 *	416-504-9876
 *
 */

#ifndef __HUSD_LockedStageRegistry_h__
#define __HUSD_LockedStageRegistry_h__

#include "HUSD_API.h"
#include "HUSD_LockedStage.h"
#include <UT/UT_Map.h>
#include <utility>

// This singleton class is used to generate safely locked, unalterable copies
// of stages generated by LOP nodes. This is primarily for LOP nodes that
// introduce references to the stages output from other LOP nodes. See the
// HUSD_LockedStage class for more information.
class HUSD_API HUSD_LockedStageRegistry
{
public:
    static HUSD_LockedStageRegistry	&getInstance();

    HUSD_LockedStagePtr		 getLockedStage(int nodeid,
					const HUSD_DataHandle &data,
					bool strip_layers,
					HUSD_StripLayerResponse response);
    void			 clearLockedStage(int nodeid);

private:
				 HUSD_LockedStageRegistry();
				~HUSD_LockedStageRegistry();

    // Locked stages are identified by an integer node id, and bool flag
    // indicating whether that node's stage was flattened with or without
    // layers from above layer breaks stripped out.
    typedef std::pair<int, bool> LockedStageId;

    UT_Map<LockedStageId, HUSD_LockedStageWeakPtr> myLockedStageMap;
};

#endif

