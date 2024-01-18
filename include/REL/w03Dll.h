#include "game/gamework_data.h"
#include "dolphin.h"
#include "REL/executor.h"
#include "game/process.h"
#include "game/window.h"
#include "game/object.h"

#define ARRAY_COUNT(arr) (s32)(sizeof(arr) / sizeof(arr[0]))

typedef struct w03StructUnk0 {
/* 0x00 */ Vec unk_00;
/* 0x0C */ Vec unk_0C;
/* 0x18 */ Vec unk_18;
/* 0x24 */ s32 datanum;
} w03StructUnk0;

typedef struct w03UnkStruct2{
    u8 unk0 : 1;
} w03UnkStruct2;

typedef struct w03State {
           s8 unk0;
           u8 unk1;
           u8 unk2;
           s8 unk3;
           u16 unk4;
           u16 unk6;
           s16 unk8;
} w03State;

//statue.c defines
void fn_1_1358(void);
void fn_1_152C(void);
s32 fn_1_15E8(void);
s32 fn_1_1650(s32 arg0);
s32 fn_1_1D1C(void);
void fn_1_1ED4(s32 arg0);
void fn_1_20E0(s32 arg0);
void fn_1_26E0(s32 arg0);
void fn_1_2AC0(void);

u32 BoardSpaceFlagGet(s32, s32);
s32 BoardWinChoiceGet(void);
void fn_1_19DC(s32, s32);
void BoardFilterFadeInit(s32, s32);
void BoardPlayerRotYSet(s32, f32);
void BoardPlayerPosLerpStart(s32, Point3d*, Point3d*, s32);
void BoardRotateDiceNumbers(s32);
void BoardPlayerAnimBlendSet(s32, s32, s32);
s32 BoardPlayerAnimBlendCheck(s32);
void BoardWinCreateChoice(s32, s32, s32, s32);
s32 fn_800710C8(s32);
void BoardComKeySetLeft(void);
void BoardComKeySetRight(void);
s32 BoardSpaceLinkFlagSearch(s32 layer, s32 index, u32 flag);
void BoardCameraMotionStartEx(s16, Vec*, Vec*, f32, f32, s16);
void fn_1_2B44(void);
u32 BoardRandMod(s32);
void BoardFilterFadeOut(s32);
s32 BoardFilterFadeDoneCheck(void);
void BoardConfettiCreate(Point3d*, s32, f32);
void BoardConfettiKill(void);
void BoardModelPosGet(s16, Point3d*);
void omVibrate(s16, s32, s32, s32);
s32 HuAudFXPlayVol(s32 arg0, s16 arg1);
s32 HuAudFXPlayVolPan(s32 arg0, s16 arg1, s16 arg2);
void HuPrcSleep(int time);
f32 BoardPlayerRotYGet(s32);
void BoardPlayerVoiceEnableSet(s32, s16, s32);
s32 BoardPlayerMotionCreate(s32, s32);
s32 BoardPlayerMotionEndCheck(s32);
void BoardPlayerMotionShiftSet(s32 player, s32 arg1, float start, float end, u32 attr);
void fn_80071034(s32, s32, s32);
s32 HuAudFXPlay(s32 arg0);
void HuAudFXStop(s32 arg0);
void fn_1_2818(void);
void fn_1_2A40(void);
void fn_1_B5C(s32);
void BoardCameraViewSet(s32);
void BoardCameraQuakeSet(s32, f32);
void BoardCameraQuakeReset(void);
s32 BoardVecCalcDAngleMod(f32*, f32, f32);
void fn_80070DEC(s32, s32, s32, s32);
void BoardCameraTargetPlayerSet(s32);
void BoardPlayerPosGet(s32, Point3d*);
double atan2(double y, double x);
void BoardModelRotYSet(s16, f32);
f32 BoardModelRotYGet(s16);
s16 BoardModelCreate(s32, void*, s32);
f32 BoardModelMotionTimeGet(s16);
s16 Hu3DHookFuncCreate(void* hook);
void Hu3DModelLightInfoSet(s16, s16);
