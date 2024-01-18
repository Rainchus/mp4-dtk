#include "dolphin.h"
#include "REL/executor.h"
#include "game/process.h"
#include "game/data.h"
#include "game/gamework_data.h"
#include "REL/w03Dll.h"
#include "game/board/space.h"
#include "game/object.h"
#include "game/board/player.h"
#include "math.h"
#include "board_unsplit.h"

void BoardModelPosSet(s16, f32, f32, f32);
s32 BoardModelMotionStart(s16, s32, s32);

void fn_8005B150(void*, void*);
void fn_1_740(void);
void fn_1_E0(void);
void fn_1_2930(void);
void fn_1_884(void);
void fn_1_8F0(void);
s32 fn_1_910(void);
s32 fn_1_A74(void);
void fn_1_AF8(void);
void fn_1_DEC(void);
void fn_1_10B0(void);
s32 fn_1_12C8(void);
void fn_8006DDE8(s16, f32);
void fn_1_10E4(omObjData* arg0);
extern Process *boardObjMan;
typedef void (*VoidFunc)(void);
extern const VoidFunc _ctors[];
extern const VoidFunc _dtors[];


f32 BoardModelMotionTimeGet(s16);
s16 BoardModelCreate(s32, void*, s32);
void BoardModelKill(s16);
void BoardModelMotionTimeRangeSet(s16, s32, s32);
void Hu3DFogSet(f32, f32, u8, u8, u8);
void HuAudFXListnerSetEX(Vec* arg0, Vec* arg1, float sndDist, float sndSpeed, float arg4, float arg5, float arg6);
u8 WipeStatGet(void);

//DATA
w03StructUnk0 lbl_1_data_0[] = {
{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, MAKE_DATA_NUM(0x0077, 0x0004)},
{{145.0988f, 65.6173f, -2004.14f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, MAKE_DATA_NUM(0x0077, 0x0005)},
{{-1548.14f, -1148.76f, -1095.54f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, MAKE_DATA_NUM(0x0077, 0x0006)},
{{1050.0f, 50.0f, -1810.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, MAKE_DATA_NUM(0x0077, 0x000B)},
{{-1950.0f, 50.0f, 1790.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.914f, 1.0f}, MAKE_DATA_NUM(0x0077, 0x000B)},
{{1050.0f, 0.0f, -1750.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, MAKE_DATA_NUM(0x0077, 0x000A)},
{{-1950.0f, 0.0f, 1850.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.914f, 1.0f}, MAKE_DATA_NUM(0x0077, 0x000A)},
{{1950.0f, 0.0f, -1800.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, MAKE_DATA_NUM(0x0077, 0x0010)},
{{-3150.0f, 0.0f, -600.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, MAKE_DATA_NUM(0x0077, 0x0011)},
{{-3150.0f, 270.0f, -600.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, MAKE_DATA_NUM(0x0077, 0x0012)},
{{2080.0f, 0.0f, -1635.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, MAKE_DATA_NUM(0x0002, 0x000D)},
{{-3000.0f, 0.0f, -500.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, MAKE_DATA_NUM(0x0002, 0x000D)},
};

s16 lbl_1_data_1E0 = -1;
s16 lbl_1_data_1E2 = -1;
s16 lbl_1_data_1E4 = -1;
s16 lbl_1_data_1E6 = -1;
s16 lbl_1_data_1E8 = -1;

typedef struct w03StructUnk1 {
    f32 unk_00;
    f32 unk_04;
    f32 unk_08;
    f32 unk_0C;
    f32 unk_10;
    f32 unk_14;
    f32 unk_18;
    f32 unk_1C;
} w03StructUnk1;

w03StructUnk1 lbl_1_data_1EC = {
	0.0f, 118.0f, 120.0f, 180.0f, 180.0f, 240.0f, 240.0f, 300.0f
};

s32 lbl_1_data_20C[] = {
    MAKE_DATA_NUM(0x0077, 0x001C),
	DATA_NUM_LISTEND
};

s32 lbl_1_data_214[] = {
    MAKE_DATA_NUM(0x0002, 0x000E),
	DATA_NUM_LISTEND
};

s32 lbl_1_data_21C[] = {
    -1, -1, -1, -1
};

Vec lbl_1_data_22C[] = {
    {3900.0f, 0.0f, 2100.0f},
    {3900.0f, 0.0f, 3600.0f},
    {2700.0f, 0.0f, 5400.0f},
    {2700.0f, 0.0f, 6300.0f},
};

s32 lbl_1_data_25C[] = {
    0x0000044B, 0x00000439, 0x00000439, 0x0000044D
};

s32 lbl_1_data_26C[] = {
    0x0000044C, 0x0000043A, 0x0000043A, 0x0000044E
};

s32 pad_27C[] = {0};

//BSS
s16 lbl_1_bss_C[14];
s32 lbl_1_bss_8;
omObjData* lbl_1_bss_4;
w03State* lbl_1_bss_0;

void fn_1_CF4(void);

void BoardCreate(void) {
    s32* var_r29;
    f32 var_f30;
    f32 var_f31;
    s32 var_r31;
    s32 var_r31_2;
    w03StructUnk0* temp_r30;
    s32 i;

    lbl_1_bss_0 = (w03State*)&GWSystem.board_data;
    lbl_1_bss_0->unk3 = 0;
    BoardSpaceInit(0x770000);
    lbl_1_data_1E0 = BoardModelCreate(0x770001, NULL, 0);
    fn_8006DDE8(lbl_1_data_1E0, -1.0f);
    BoardModelPosSet(lbl_1_data_1E0, 0.0f, 0.0f, 0.0f);
    BoardModelMotionStart(lbl_1_data_1E0, 0, 0x40000001);
    lbl_1_data_1E8 = BoardModelCreate(0x770003, NULL, 0);
    fn_8006DDE8(lbl_1_data_1E8, -1.0f);
    BoardModelPosSet(lbl_1_data_1E8, 0.0f, 0.0f, 0.0f);
    BoardModelLayerSet(lbl_1_data_1E8, 2);
    lbl_1_data_1E2 = BoardModelCreate(0x770002, NULL, 0);
    fn_8006DDE8(lbl_1_data_1E2, -1.0f);
    BoardModelPosSet(lbl_1_data_1E2, 0.0f, 0.0f, 0.0f);
    BoardModelMotionStart(lbl_1_data_1E2, 0, 0x40000001);
    lbl_1_data_1E4 = BoardModelCreate(0x77001B, &lbl_1_data_20C, 0);
    BoardModelPosSet(lbl_1_data_1E4, 0.0f, 0.0f, 0.0f);
    BoardModelMotionStart(lbl_1_data_1E4, 1, 0x40000001);
    lbl_1_data_1E6 = BoardModelCreate(0x2000D, &lbl_1_data_214, 0);
    BoardModelPosSet(lbl_1_data_1E6, 0.0f, 0.0f, 0.0f);
    BoardModelMotionStart(lbl_1_data_1E6, 1, 0x40000001);

    for (i = 0; i < 12; i++) {
        temp_r30 = &lbl_1_data_0[i];
        if (temp_r30->datanum != -1) {
            if ((i == 10) || (i == 11)) {
                var_r29 = lbl_1_data_214;
            } else {
                var_r29 = NULL;
            }
            lbl_1_bss_C[i]= BoardModelCreate(temp_r30->datanum, var_r29, 0);
            BoardModelPosSetV(lbl_1_bss_C[i], &temp_r30->unk_00);
            BoardModelRotSetV(lbl_1_bss_C[i], &temp_r30->unk_0C);
            BoardModelScaleSetV(lbl_1_bss_C[i], &temp_r30->unk_18);
            BoardModelVisibilitySet(lbl_1_bss_C[i], 1);
        }
    }
    BoardModelMotionStart(lbl_1_bss_C[10], 1, 0x40000001);
    BoardModelMotionStart(lbl_1_bss_C[11], 1, 0x40000001);
    BoardModelMotionStart(lbl_1_bss_C[7], 0, 0x40000002);
    BoardModelMotionStart(lbl_1_bss_C[9], 0, 0x40000002);
    BoardModelMotionStart(lbl_1_bss_C[1], 0, 0x40000002);
    fn_1_3058();
    fn_1_6494();
    fn_1_7ABC();
    fn_1_9A7C();
    BoardLightHookSet(&fn_1_884, &fn_1_8F0);
    BoardSpaceWalkEventFuncSet(&fn_1_910);
    BoardSpaceWalkMiniEventFuncSet((void*)&fn_1_A74);
    BoardSpaceLandEventFuncSet((void*)&fn_1_AF8);
    fn_80083EDC(lbl_1_data_1E4);
    fn_800A4F6C(lbl_1_data_1E6);
    fn_8007A83C(lbl_1_data_1E6);
    BoardShopHostSet(lbl_1_data_1E6);
    BoardJunctionMaskSet(0x0000C000);
    BoardJunctionMaskSet(0x3000);
    fn_1_CF4();
    BoardModelMotionStart(lbl_1_bss_C[0], 0, 0x40000001);
    if (lbl_1_bss_0->unk0 != 0) {
        var_f30 = lbl_1_data_1EC.unk_10;
        var_f31 = lbl_1_data_1EC.unk_14;
        fn_1_63F4(0);
    } else {
        var_f30 = lbl_1_data_1EC.unk_00;
        var_f31 = lbl_1_data_1EC.unk_04;
        fn_1_63F4(1);
    }
    BoardModelMotionTimeRangeSet(lbl_1_bss_C[0], var_f30, var_f31);
    BoardModelMotionStart(lbl_1_bss_C[2], 0, 0x40000001);
    BoardModelMotionTimeRangeSet(lbl_1_bss_C[2], 1, 0x64);
    fn_1_785C();
    if (lbl_1_bss_0->unk2 != 0) {
        for (i = 0; i < 4; i++) {
            if ((lbl_1_bss_0->unk2 & (1 << i)) != 0) {
                BoardPlayerPreTurnHookSet(i, &fn_1_2930);
                break;
            }
        }
    }
    fn_1_DEC();
}

void BoardDestroy(void) {
    s32 i;

    fn_1_10B0();
    fn_1_9A9C();
    fn_1_7B58();
    for (i = 0; i < 0x0C; i++) {
        BoardModelKill(lbl_1_bss_C[i]);
        lbl_1_bss_C[i] = -1;        
    }
    if (lbl_1_data_1E6 != -1) {
        BoardModelKill(lbl_1_data_1E6);
        lbl_1_data_1E6 = -1;
    }
    if (lbl_1_data_1E4 != -1) {
        BoardModelKill(lbl_1_data_1E4);
        lbl_1_data_1E4 = -1;
    }
    if (lbl_1_data_1E0 != -1) {
        BoardModelKill(lbl_1_data_1E0);
        lbl_1_data_1E0 = -1;
    }
    if (lbl_1_data_1E2 != -1) {
        BoardModelKill(lbl_1_data_1E2);
        lbl_1_data_1E2 = -1;
    }
}

void fn_1_884(void) {
    s16 var = BoardModelIDGet(lbl_1_data_1E0);
    Hu3DModelLightInfoSet(var, 1);
    Hu3DFogSet(5000.0f, 30000.0f, 0xE4U, 0xF0U, 0xFFU);
}

void fn_1_8F0(void) {
    Hu3DFogClear();
}

s32 fn_1_910(void) {
    u32 temp_r3;
    s32 cur_player_index;
    s32 space;

    cur_player_index = GWSystem.player_curr;
    space = GWPlayer[cur_player_index].space_curr;
    temp_r3 = BoardSpaceFlagGet(0, space);
    if (temp_r3 & 0xC000) {
        if ((BoardPlayerSizeGet(cur_player_index) == 2) || ((u8) (( GWPlayer[cur_player_index].bowser_suit)))) {
            return 0;
        }
        if (temp_r3 & 0x4000) {
            if (lbl_1_bss_0->unk1 == 0) {
                return 0;
            }
        }
        else if (temp_r3 & 0x8000 && lbl_1_bss_0->unk1 != 0) {
            return 0;
        }
        
        return fn_1_309C();
    }
    if (temp_r3 & 0x200) {
        if ((BoardPlayerSizeGet(cur_player_index) == 2) || ((u8) ((GWPlayer[cur_player_index].bowser_suit)) != 0)) {
            return 0;
        }
        return fn_1_675C();
    }
    return 0;
}

s32 fn_1_A74(void) {
    s16 space = GWPlayer[GWSystem.player_curr].space_curr;
    u32 var = BoardSpaceFlagGet(0, space) & 0x600000;
    if (var + -0x200000 == 0) {
        fn_1_7BA8();
        return;
    }
    if (var + -0x400000 == 0) {
        fn_1_9AA0();
    }
}

void fn_1_AF8(void) {
    if ((BoardPlayerSizeGet(GWSystem.player_curr) == 0) && (fn_1_12C8() != 0) && (lbl_1_bss_0->unk2 != 0)) {
        BoardJunctionMaskSet(0xC00);
    }
}
void fn_1_B5C(s32 arg0) {
    f32 var_f31;
    f32 var_f30;
    f32 var_f29;
    f32 var_f28;
    f32 temp;

    if (arg0 != 0) {
        var_f30 = lbl_1_data_1EC.unk_08;
        var_f29 = lbl_1_data_1EC.unk_0C;
        var_f31 = lbl_1_data_1EC.unk_10;
        var_f28 = lbl_1_data_1EC.unk_14;
        fn_1_63F4(0);
    } else {
        var_f30 = lbl_1_data_1EC.unk_18;
        var_f29 = lbl_1_data_1EC.unk_1C;
        var_f31 = lbl_1_data_1EC.unk_00;
        var_f28 = lbl_1_data_1EC.unk_04;
        fn_1_63F4(1);
    }
    BoardModelMotionStart(lbl_1_bss_C[0], 0, 0);
    BoardModelMotionTimeSet(lbl_1_bss_C[0], var_f30);
    while (1) {
        temp = BoardModelMotionTimeGet(lbl_1_bss_C[0]);
        if (temp >= var_f29) {
            break;
        }
        HuPrcVSleep();
    }
    BoardModelAttrSet(lbl_1_bss_C[0], 0x40000001);
    BoardModelMotionTimeSet(lbl_1_bss_C[0], var_f31);
    BoardModelMotionTimeRangeSet(lbl_1_bss_C[0], (s32) var_f31, (s32) var_f28);
}

void fn_1_CF4(void) {
    s16 var;
    s32 i, j;
    BoardSpace* temp_r28;
    BoardSpace* temp_r31;

    for (i = 0; i < BoardSpaceCountGet(0); i++) {
        var = i + 1;
        temp_r31 = BoardSpaceGet(0, var);
        for (j = 0; j < temp_r31->link_cnt; j++) {
            if (BoardSpaceFlagGet(0, temp_r31->link[j]) & 0xC00) {
                temp_r28 = BoardSpaceGet(0, temp_r31->link[j]);
                if (lbl_1_bss_0->unk0 != 0) {
                    temp_r28->flag |= 0x04000000;
                    BoardJunctionMaskSet(0xC00);
                } else {
                    temp_r28->flag &= ~0x04000000;
                    BoardJunctionMaskReset(0xC00);
                }
            }            
        }
    }
}

void fn_1_DEC(void) {
    Vec sp20;
    Vec sp14;
    Vec sp8;
    omObjData* temp_r3;
    s32 var_r31;
    s32 var_r31_2;
    w03UnkStruct2* temp_r29;
    s32 i;

    temp_r3 = omAddObjEx(boardObjMan, 0x101, 0, 0, -1, &fn_1_10E4);
    lbl_1_bss_4 = temp_r3;
    temp_r29 = (w03UnkStruct2*)&temp_r3->work[0];
    temp_r29->unk0 = 0;
    lbl_1_bss_8 = 0;
    BoardCameraPosGet(&sp20);
    BoardCameraDirGet(&sp14);
    HuAudFXListnerSetEX(&sp20, &sp14, 4800.0f, 36000.0f, 0.0f, 1200.0f, 1200.0f);
    for (i = 0; i < 4; i++) {
        sp8.x = lbl_1_data_22C[i].x - 3600.0f;
        sp8.y = lbl_1_data_22C[i].y;
        sp8.z = lbl_1_data_22C[i].z - 3600.0f;
        if (lbl_1_bss_0->unk2 != 0) {
            lbl_1_data_21C[i] = HuAudFXEmiterPlay(lbl_1_data_26C[i], &sp8);
        } else {
            lbl_1_data_21C[i] = HuAudFXEmiterPlay(lbl_1_data_25C[i], &sp8);
        }
    }

    for (i = 0; i < 4; i++) {
        if ((WipeStatGet() != 0) || (_CheckFlag(0x1001CU) != 0)) {
            HuAudFXVolSet(lbl_1_data_21C[i], 0);
        } else {
            HuAudFXVolSet(lbl_1_data_21C[i], 0x7F);
        }        
    }

    if (_CheckFlag(0x10010) == 0) {
        if (lbl_1_bss_8 == 0) {
            HuAudFXPauseAll(1);
            lbl_1_bss_8 = 1;
        }
    } else if (lbl_1_bss_8 != 0) {
        HuAudFXPauseAll(0);
        lbl_1_bss_8 = 0;
    }
}

void fn_1_10B0(void) {
    if (lbl_1_bss_4) {
        ((w03UnkStruct2*)&lbl_1_bss_4->work[0])->unk0 = 1;
    }
}

void fn_1_10E4(omObjData* arg0) {
    Vec sp14;
    Vec sp8;
    s32 i;
    w03UnkStruct2* temp_r29;

    temp_r29 = (w03UnkStruct2*)&arg0->work[0];
    if (temp_r29->unk0 != 0 || (BoardIsKill() != 0)) {
        for (i = 0; i < ARRAY_COUNT(lbl_1_data_21C); i++) {
            if (lbl_1_data_21C[i] != -1) {
                HuAudFXFadeOut(lbl_1_data_21C[i], 100);
            }            
        }
        HuAudFXListnerKill();
        lbl_1_bss_4 = 0;
        omDelObjEx(HuPrcCurrentGet(), arg0);
        return;
    }
    BoardCameraDirGet(&sp8);
    BoardCameraTargetGet(&sp14);

    for (i = 0; i < 4; i++) {
        if ((WipeStatGet() != 0) || (_CheckFlag(0x1001C) != 0)) {
            HuAudFXVolSet(lbl_1_data_21C[i], 0);
        } else {
            HuAudFXVolSet(lbl_1_data_21C[i], 0x7F);
        }
    }

    sp14.y += 2000.0f;
    if (_CheckFlag(0x10010) == 0) {
        if (lbl_1_bss_8 == 0) {
            HuAudFXPauseAll(1);
            lbl_1_bss_8 = 1;
        }
    } else if (lbl_1_bss_8 != 0) {
        HuAudFXPauseAll(0);
        lbl_1_bss_8 = 0;
    }
    HuAudFXListnerUpdate(&sp14, &sp8);
}