#ifndef _BOARD_SPACE_H
#define _BOARD_SPACE_H

#include "dolphin.h"

#define BOARD_SPACE_LINKMAX 4

#ifndef _BOARD_SPACE_EVENT_FUNC
#define _BOARD_SPACE_EVENT_FUNC
typedef s32 (*BoardSpaceEventFunc)(void);
#endif

typedef struct board_space {
	Vec pos;
	u32 flag;
	Vec scale;
	Vec rot;
	u16 type;
	u16 link_cnt;
	u16 link[BOARD_SPACE_LINKMAX+1];
} BoardSpace;

void BoardSpaceWalkEventFuncSet(BoardSpaceEventFunc func);
void BoardSpaceWalkMiniEventFuncSet(BoardSpaceEventFunc func);
void BoardSpaceLandEventFuncSet(BoardSpaceEventFunc func);
s32 BoardSpaceWalkEventExec(void);
s32 BoardSpaceWalkMiniEventExec(void);
s32 BoardSpaceCountGet(s32 layer);
BoardSpace *BoardSpaceGet(s32 layer, s32 index);
void BoardSpaceAttrSet(s32 layer, u32 attr);
void BoardSpaceAttrReset(s32 layer, u32 attr);
u32 BoardSpaceFlagGet(s32 layer, s32 index);
s32 BoardSpaceTypeGet(s32 layer, s32 index);
void BoardSpaceTypeSet(s32 layer, s32 index, s32 type);
s32 BoardSpacePosGet(s32 layer, s32 index, Vec *pos);
void BoardSpaceCornerPosGet(s32 index, s32 corner, Vec *pos);
s32 BoardSpaceRotGet(s32 layer, s32 index, Vec *rot);
s32 BoardSpaceFlagSearch(s32 layer, u32 flag);
s32 BoardSpaceFlagPosGet(s32 layer, u32 flag, Vec *pos);
s32 BoardSpaceLinkFlagSearch(s32 layer, s32 index, u32 flag);
s32 BoardSpaceLinkTypeListGet(s32 layer, s32 index, s32 type, s16 *list);
s32 BoardSpaceLinkTargetListGet(s32 layer, s32 target, s16 *list);
s32 BoardSpaceLinkTypeSearch(s32 layer, s32 target, u16 type);
s32 BoardSpaceLinkTransformGet(s32 flag, Vec *pos, Vec *rot, Vec *scale);
void BoardSpaceStarSet(s32 space);
void BoardSpaceStarSetIndex(s32 index);
s32 BoardSpaceStarGetNext(void);
s32 BoardSpaceStarGetRandom(s32 excl_pos);
void BoardSpaceStarMove(void);
s32 BoardSpaceStarGet(s32 index);
s32 BoardSpaceStarGetCurr(void);
s32 BoardSpaceStarCheck(s32 index);
void BoardSpaceLandExec(s32 player, s32 space);
void BoardSpaceBlockPosSet(void);

#endif
