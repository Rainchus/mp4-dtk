#include "board_unsplit.h"
#include "game/gamework_data.h"
#include "math.h"
#include "game/object.h"
#include "game/flag.h"
#include "game/data.h"
#include "game/wipe.h"
#include "string.h"
#include "game/hsfman.h"
#include "game/board/main.h"
#include "game/board/player.h"

typedef struct camera_view {
	s16 x_rot;
	s16 zoom;
	s16 fov;
} CameraView;


omObjData *boardMainObj;
u32 boardRandSeed;
static omObjData *last5GfxObj;
static omObjData *confettiObj;
static omObjData *filterObj;
void *boardTurnStartFunc;
u32 lbl_801D3F00;
u32 lbl_801D3EFC;
u32 lbl_801D3EF8;
BoardFunc boardTurnFunc;
BoardLightHook boardLightSetHook;
BoardLightHook boardLightResetHook;
static BoardFunc destroyFunc;
static BoardFunc createFunc;
static BOOL cameraUseBackup;
static omObjData *tauntObj;
static omObjData *cameraObj;
Process *boardObjMan;
Process *boardMainProc;

static BoardCameraData cameraBackup;
BoardCameraData boardCamera;

static OverlayID nextOvl = OVL_INVALID;

static CameraView camViewTbl[] = {
	{ 0, 0, 25 },
	{ -33, 3200, 25 },
	{ -33, 2100, 25 },
	{ -33, 1800, 25 },
	{ -33, 12640, 25 },
	{ -33, 3200, 25 },
};


// extern void BoardModelPosGet(s16 model, Vec *pos);
// extern void BoardSpacePosGet(s32 layer, s32 space, Vec *pos);

extern void fn_800A4A7C(void);
extern void fn_800A6EE4(void);

extern s8 boardTutorialF;
extern s16 boardPlayerMdl[4];

void BoardKill(void);
void BoardCameraInit(void);
void BoardCameraMotionWait(void);
void BoardCameraTargetPlayerSet(s32 player);
void BoardCameraViewSet(s32 type);
void BoardCameraOffsetSet(float x, float y, float z);
void BoardCameraMoveSet(s32 move);
void BoardCameraMotionStartEx(s16 model_target, Vec *rot_target, Vec *offset_end, float zoom_target, float fov_target, s16 max_time);
void BoardRandInit(void);
float BoardRandFloat(void);
s32 BoardDataDirReadAsync(s32 data_num);
void BoardDataAsyncWait(s32 status);

static void InitBoardFunc(omObjData *object);
static void ExecBoardFunc(omObjData *object);
static void KillBoardFunc(omObjData *object);
static void UpdateCamera(omObjData *object);

static void CalcCameraTarget(BoardCameraData *camera);
static void CalcCameraPos(BoardCameraData *camera);


static void MainFunc(void);
static void DestroyMainFunc(void);

static void CreateBoard(void);
static void DestroyBoard(void);

static inline int GWMGTypeGet()
{
	return GWSystem.mg_type;
}

static inline int GWMessSpeedGet()
{
	return GWSystem.mess_speed;
}

#define BOARD_FABS(value) ((value < 0) ? -(value) : (value))

void BoardCommonInit(BoardFunc create, BoardFunc destroy)
{
	omSysPauseEnable(FALSE);
	if(!_CheckFlag(FLAG_ID_MAKE(1, 0))) {
		_SetFlag(FLAG_ID_MAKE(1, 0));
		_ClearFlag(FLAG_ID_MAKE(2, 1));
		_ClearFlag(FLAG_ID_MAKE(2, 3));
		_ClearFlag(FLAG_ID_MAKE(2, 0));
		_ClearFlag(FLAG_ID_MAKE(2, 2));
		_ClearFlag(FLAG_ID_MAKE(2, 4));
		_ClearFlag(FLAG_ID_MAKE(2, 5));
		_ClearFlag(FLAG_ID_MAKE(1, 6));
		_ClearFlag(FLAG_ID_MAKE(1, 9));
		_ClearFlag(FLAG_ID_MAKE(0, 8));
		_ClearFlag(FLAG_ID_MAKE(0, 10));
		fn_800A4A7C();
	}
	
	nextOvl = OVL_INVALID;
	_SetFlag(FLAG_ID_MAKE(1, 28));
	_SetFlag(FLAG_ID_MAKE(1, 14));
	_ClearFlag(FLAG_ID_MAKE(1, 16));
	_ClearFlag(FLAG_ID_MAKE(1, 17));
	_ClearFlag(FLAG_ID_MAKE(1, 18));
	_ClearFlag(FLAG_ID_MAKE(1, 19));
	_ClearFlag(FLAG_ID_MAKE(1, 20));
	_ClearFlag(FLAG_ID_MAKE(1, 21));
	_ClearFlag(FLAG_ID_MAKE(1, 23));
	_ClearFlag(FLAG_ID_MAKE(1, 24));
	_ClearFlag(FLAG_ID_MAKE(1, 25));
	_ClearFlag(FLAG_ID_MAKE(1, 27));
	createFunc = create;
	destroyFunc = destroy;
	boardTurnFunc = NULL;
	lbl_801D3EFC = 0;
	lbl_801D3F00 = 0;
	lbl_801D3EF8 = 0;
	boardTurnStartFunc = NULL;
	boardObjMan = omInitObjMan(64, 8192);
	omSystemKeyCheckSetup(boardObjMan);
	boardMainObj = omAddObjEx(boardObjMan, 0, 0, 0, -1, InitBoardFunc);
	switch(omcurovl) {
		case OVL_W01:
			GWSystem.board = 0;
			break;
			
		case OVL_W02:
			GWSystem.board = 1;
			break;
			
		case OVL_W03:
			GWSystem.board = 2;
			break;
			
		case OVL_W04:
			GWSystem.board = 3;
			break;
			
		case OVL_W05:
			GWSystem.board = 4;
			break;
			
		case OVL_W06:
			GWSystem.board = 5;
			break;
			
		case OVL_W10:
			GWSystem.board = 6;
			break;
			
		case OVL_W20:
			GWSystem.board = 7;
			break;
			
		case OVL_W21:
			GWSystem.board = 8;
			break;
	}
}

static void InitBoardFunc(omObjData *object)
{
	boardMainProc = HuPrcChildCreate(MainFunc, 8194, 0x6000, 0, boardObjMan);
	HuPrcDestructorSet2(boardMainProc, DestroyMainFunc);
	object->func = ExecBoardFunc;
}

static void ExecBoardFunc(omObjData *object)
{
	if(!_CheckFlag(FLAG_ID_MAKE(1, 16))) {
		return;
	}
	if(!_CheckFlag(FLAG_ID_MAKE(1, 17))) {
		if(omSysExitReq) {
			BoardKill();
		} else {
			if(BoardPauseReqCheck()) {
				BoardPauseStart();
			} else {
				return;
			}
		}
	} else {
		if(boardMainProc) {
			HuPrcKill(boardMainProc);
		}
		object->func = KillBoardFunc;
	}
}

static void KillBoardFunc(omObjData *object)
{
	if(boardMainProc) {
		return;
	}
	if(WipeStatGet()) {
		return;
		return_alt:
		return;
	} else {
		if(HuARDMACheck()) {
			goto return_alt;
		}
		if(nextOvl != OVL_INVALID) {
			omOvlCallEx(nextOvl, 1, 0, 0);
		} else {
			if(_CheckFlag(FLAG_ID_MAKE(1, 11))) {
				BoardTutorialKill();
				BoardTutorialWorkRestore();
			}
			_ClearFlag(FLAG_ID_MAKE(1, 0));
			HuARDirFree(MAKE_DIR_NUM(DATADIR_BOARD));
			if(_CheckFlag(FLAG_ID_MAKE(1, 27))) {
				omOvlReturnEx(2, 1);
			} else {
				omOvlReturnEx(1, 1);
			}
		}
	}
}

void BoardKill(void)
{
	_SetFlag(FLAG_ID_MAKE(1, 28));
	_SetFlag(FLAG_ID_MAKE(1, 14));
	if(_CheckFlag(FLAG_ID_MAKE(1, 11))) {
		BoardTutorialHookExec(29, 0);
	}
	HuAudFXAllStop();
	if(!BoardStartCheck()) {
		WipeCreate(WIPE_MODE_OUT, WIPE_TYPE_NORMAL, -1);
		boardTutorialF = 1;
		while(WipeStatGet()) {
			HuPrcVSleep();
		}
	}
	_SetFlag(FLAG_ID_MAKE(1, 17));
	HuPrcKill(boardMainProc);
	DestroyBoard();
}

s32 BoardIsKill(void)
{
	return (_CheckFlag(FLAG_ID_MAKE(1, 17))) ? 1 : 0;
}

void BoardPauseEnableSet(s32 value)
{
	if(_CheckFlag(FLAG_ID_MAKE(1, 11))) {
		_SetFlag(FLAG_ID_MAKE(1, 25));
		omSysPauseEnable(FALSE);
	} else {
		if(value) {
			_SetFlag(FLAG_ID_MAKE(1, 25));
		} else {
			_ClearFlag(FLAG_ID_MAKE(1, 25));
		}
	}
}

s32 BoardPauseEnableGet()
{
	return (_CheckFlag(FLAG_ID_MAKE(1, 25))) ? 1 : 0;
}

void BoardSaveInit(s32 board)
{
	int i;
	GWSystem.board = board;
	_ClearFlag(FLAG_ID_MAKE(1, 0));
	_ClearFlag(FLAG_ID_MAKE(1, 1));
	_ClearFlag(FLAG_ID_MAKE(1, 5));
	_ClearFlag(FLAG_ID_MAKE(1, 4));
	_ClearFlag(FLAG_ID_MAKE(1, 6));
	_ClearFlag(FLAG_ID_MAKE(1, 2));
	_ClearFlag(FLAG_ID_MAKE(1, 9));
	_ClearFlag(FLAG_ID_MAKE(1, 7));
	_ClearFlag(FLAG_ID_MAKE(1, 8));
	_ClearFlag(FLAG_ID_MAKE(0, 8));
	_ClearFlag(FLAG_ID_MAKE(1, 12));
	_ClearFlag(FLAG_ID_MAKE(1, 3));
	GWSystem.turn = 1;
	GWSystem.star_flag = 0;
	GWSystem.star_pos = 0;
	GWSystem.star_total = 1;
	GWSystem.last5_effect = 0;
	GWSystem.player_curr = -1;
	GWSystem.field31_bit0 = 0;
	GWSystem.field31_bit4 = 0;
	GWSystem.unk_32 = 1;
	GWSystem.mg_next = 0;
	GWSystem.mg_next_extra = 0;
	GWSystem.unk_38 = 0;
	GWSystem.block_pos = 0;
	memset(GWSystem.board_data, 0, 32);
	for(i=0; i<4; i++) {
		s32 party_flag;
		BoardPlayerAutoSizeSet(i, 0);
		GWPlayer[i].field00_bit9 = 0;
		GWPlayer[i].color = 0;
		GWPlayer[i].bowser_suit = 0;
		GWPlayer[i].field08_bit3 = 0;
		GWPlayer[i].space_shock = 0;
		GWPlayer[i].field02_bit1 = 0;
		GWPlayer[i].show_next = 1;
		BoardPlayerCoinsSet(i, 0);
		if(_CheckFlag(FLAG_ID_MAKE(1, 11))) {
			GWSystem.team = 0;
		}
		GWPlayer[i].blue_count = 0;
		GWPlayer[i].red_count = 0;
		GWPlayer[i].question_count = 0;
		GWPlayer[i].fortune_count = 0;
		GWPlayer[i].bowser_count = 0;
		GWPlayer[i].battle_count = 0;
		GWPlayer[i].mushroom_count = 0;
		GWPlayer[i].warp_count = 0;
		GWPlayer[i].coins_mg = 0;
		GWPlayer[i].coins_total = 0;
		GWPlayer[i].coins_max = 0;
		GWPlayer[i].stars_max = 0;
		GWPlayer[i].coins_battle = 0;
		GWPlayer[i].unk_26 = 0;
		GWPlayer[i].coins_mg_gain = 0;
		GWPlayer[i].items[0] = -1;
		GWPlayer[i].items[1] = -1;
		GWPlayer[i].items[2] = -1;
		if(BoardPartyFlagGet() == 0 || _CheckFlag(FLAG_ID_MAKE(1, 11))) {
			GWStarsSet(i, 0);
		} else {
			GWStarsSet(i, BoardPlayerHandicapGet(i));
		}
	}
}

void BoardStoryConfigSet(s32 mg_type, s32 diff_story)
{
	GWSystem.party = 0;
	GWSystem.team = 0;
	GWSystem.diff_story = diff_story;
	GWSystem.bonus_star = 0;
	GWSystem.mg_type = mg_type;
	GWPlayer[0].handicap = 0;
	GWPlayer[1].handicap = 0;
	GWPlayer[2].handicap = 0;
	GWPlayer[3].handicap = 0;
	GWSystem.max_turn = 15;
	memset(GWPlayer, 0, 4*sizeof(PlayerState));
	_ClearFlag(FLAG_ID_MAKE(0, 2));
	_ClearFlag(FLAG_ID_MAKE(0, 3));
	_ClearFlag(FLAG_ID_MAKE(0, 4));
	_ClearFlag(FLAG_ID_MAKE(0, 5));
	_ClearFlag(FLAG_ID_MAKE(0, 6));
	_ClearFlag(FLAG_ID_MAKE(0, 7));
	_ClearFlag(FLAG_ID_MAKE(1, 11));
	_SetFlag(FLAG_ID_MAKE(0, 11));
	_SetFlag(FLAG_ID_MAKE(1, 10));
}

void BoardPartyConfigSet(s32 team, s32 bonus_star, s32 mg_type, s32 max_turn, s32 p1_handicap, s32 p2_handicap, s32 p3_handicap, s32 p4_handicap)
{
	GWSystem.party = 1;
	GWSystem.team = team;
	GWSystem.diff_story = 0;
	GWSystem.bonus_star = bonus_star;
	GWSystem.mg_type = mg_type;
	GWSystem.max_turn = max_turn;
	memset(GWPlayer, 0, 4*sizeof(PlayerState));
	GWPlayer[0].handicap = p1_handicap;
	GWPlayer[1].handicap = p2_handicap;
	GWPlayer[2].handicap = p3_handicap;
	GWPlayer[3].handicap = p4_handicap;
	_ClearFlag(FLAG_ID_MAKE(1, 11));
	_SetFlag(FLAG_ID_MAKE(0, 11));
	_SetFlag(FLAG_ID_MAKE(1, 10));
}

static void DestroyMainFunc(void)
{
	boardMainProc = NULL;
}

static void MainFunc(void)
{
	s32 i;
	s32 fade_enable, turn_cont, fade_type;
	fade_enable = 0;
	turn_cont = 0;
	BoardPauseEnableSet(1);
	if(_CheckFlag(FLAG_ID_MAKE(0, 10))) {
		_ClearFlag(FLAG_ID_MAKE(0, 10));
		_SetFlag(FLAG_ID_MAKE(1, 16));
		BoardKill();
		HuPrcSleep(-1);
	}
	_ClearFlag(FLAG_ID_MAKE(0, 10));
	BoardTutorialInit();
	if(_CheckFlag(FLAG_ID_MAKE(1, 11))) {
		BoardTutorialWorkSave();
		BoardTutorialPlayerInit();
		GWSystem.mess_speed = 1;
		GWSystem.mess_delay = 32;
	}
	CreateBoard();
	if(!_CheckFlag(FLAG_ID_MAKE(1, 1))) {
		GWSystem.player_curr = -1;
		fn_800A6EE4();
		GWSystem.player_curr = 0;
		fade_enable = 1;
		_SetFlag(FLAG_ID_MAKE(1, 1));
	} else {
		fade_enable = 0;
	}
	if(ExecTurnStart()) {
		turn_cont = 1;
	}
	if((int)(GWSystem.max_turn-GWSystem.turn) < 5 && GWSystem.player_curr == 0 && !turn_cont) {
		if(!_CheckFlag(FLAG_ID_MAKE(0, 8))) {
			fn_800A9708();
			_SetFlag(FLAG_ID_MAKE(0, 8));
		} else {
			BoardLast5GfxInit();
		}
	}
	if(!turn_cont) {
		fade_type = 1;
	} else {
		fade_type = 0;
	}
	_ClearFlag(FLAG_ID_MAKE(1, 28));
	do {
		BoardStatusShowSetAll(1);
		if(BoardCurrGet() == 5 && GWSystem.player_curr == 0 && !turn_cont && boardTurnFunc) {
			GWSystem.player_curr = -1;
			boardTurnFunc();
			GWSystem.player_curr = 0;
		}
		fn_80070D84();
		for(i=GWSystem.player_curr; i<4; i++) {
			if(BoardCurrGet() == 7 || BoardCurrGet() == 8) {
				if((int)(GWSystem.max_turn-GWSystem.turn) < 5 && i == 0 && !turn_cont) {
					BoardLast5GfxInit();
					_SetFlag(FLAG_ID_MAKE(0, 8));
				}
			}
			if(_CheckFlag(FLAG_ID_MAKE(1, 6))) {
				BoardPlayerTurnExec(i);
			} else {
				if(!turn_cont) {
					BoardCameraMoveSet(0);
					GWSystem.player_curr = i;
					BoardCameraTargetPlayerSet(i);
					BoardCameraMotionWait();
					{
						Vec pos;
						BoardSpacePosGet(0, GWPlayer[i].space_curr, &pos);
						BoardPlayerPosSetV(i, &pos);
					}
					while(WipeStatGet()) {
						HuPrcVSleep();
					}
					if(!fade_enable) {
						if(!fade_type) {
							WipeCreate(WIPE_MODE_IN, WIPE_TYPE_CROSS, 30);
							while(WipeStatGet()) {
								HuPrcVSleep();
							}
						} else {
							fade_type = 0;
							WipeCreate(WIPE_MODE_IN, WIPE_TYPE_NORMAL, 21);
							while(WipeStatGet()) {
								HuPrcVSleep();
							}
						}
					} else {
						fade_enable = 0;
					}
					BoardPlayerTurnExec(i);
				}
			}
			turn_cont = 0;
			if(_CheckFlag(FLAG_ID_MAKE(1, 11))) {
				BoardTutorialHookExec(9, 0);
			}
			if(i != 3 || _CheckFlag(FLAG_ID_MAKE(1, 11))) {
				WipeColorSet(255, 255, 255);
				WipeCreate(WIPE_MODE_OUT, WIPE_TYPE_CROSS, 1);
				while(WipeStatGet()) {
					HuPrcVSleep();
				}
				BoardPlayerMoveAwayStartCurr(GWPlayer[i].space_curr, 1);
				fade_type = 0;
			} else {
				if(BoardCurrGet() == 7 || BoardCurrGet() == 8) {
					if(GWSystem.turn != GWSystem.max_turn) {
						WipeColorSet(255, 255, 255);
						WipeCreate(WIPE_MODE_OUT, WIPE_TYPE_CROSS, 1);
						while(WipeStatGet()) {
							HuPrcVSleep();
						}
						BoardPlayerMoveAwayStartCurr(GWPlayer[i].space_curr, 1);
						fade_type = 0;
					}
				} else {
					WipeColorSet(0, 0, 0);
					WipeCreate(WIPE_MODE_OUT, WIPE_TYPE_NORMAL, 21);
					while(WipeStatGet()) {
						HuPrcVSleep();
					}
				}
			}
			BoardPlayerPostTurnHookExec(i);
		}
		if(BoardCurrGet() == 7 || BoardCurrGet() == 8) {
			GWSystem.player_curr = 0;
			if(BoardTurnNext()) {
				fn_80070EE8(0, 500);
				BoardKill();
				HuPrcEnd();
				HuPrcSleep(-1);
			}
		} else {
			_SetFlag(FLAG_ID_MAKE(1, 28));
			_SetFlag(FLAG_ID_MAKE(1, 14));
			BoardPauseEnableSet(1);
			_ClearFlag(FLAG_ID_MAKE(1, 9));
			if(_CheckFlag(FLAG_ID_MAKE(2, 0)) || _CheckFlag(FLAG_ID_MAKE(1, 11)) ) {
				for(i=0; i<4; i++) {
					GWPlayer[i].color = 0;
				}
				GWSystem.player_curr = (GWSystem.player_curr+1)&3;
			} else {
				fn_800A1A34();
				HuPrcSleep(-1);
			}
		}
	} while(1);
}

BOOL BoardTurnNext(void)
{
	s32 i;
	for(i=0; i<4; i++) {
		GWPlayer[i].color = 0;
	}
	GWSystem.turn++;
	if(GWSystem.turn > GWSystem.max_turn) {
		return 1;
	} else {
		return 0;
	}
}

void BoardNextOvlSet(OverlayID overlay)
{
	nextOvl = overlay;
	fn_80070EE8(0, 1000);
	BoardKill();
}

s32 BoardStartCheck(void)
{
	if(_CheckFlag(FLAG_ID_MAKE(1, 2)) || _CheckFlag(FLAG_ID_MAKE(1, 3)) || _CheckFlag(FLAG_ID_MAKE(1, 4)) || _CheckFlag(FLAG_ID_MAKE(1, 5)) || _CheckFlag(FLAG_ID_MAKE(1, 6))) {
		return 1;
	} else {
		return 0;
	}
}

static void CreateBoard(void)
{
	s32 guest_status;
	int mess_speed;
	s32 clear_flags;
	s32 reset_unk32;
	
	GWSystem.mg_next = -1;
	if(!GWGameStat.field10E_bit5) {
		s32 type_temp;
		if(GWSystem.mg_type == 3) {
			GWSystem.mg_type = 0;
		}
		if(GWMGTypeGet() == 2) {
			GWSystem.mg_type = 0;
		}
	}
	if(GWSystem.mess_speed == 3) {
		GWSystem.mess_speed = 1;
	}
	mess_speed = GWMessSpeedGet();
	GWSystem.mess_speed = mess_speed;
	switch(mess_speed) {
		case 0:
			GWSystem.mess_delay = 16;
			break;
			
		case 2:
			GWSystem.mess_delay = 48;
			break;
			
		default:
			GWSystem.mess_delay = 32;
			break;
	}
	if((int)GWSystem.explain_mg != 0) {
		_SetFlag(FLAG_ID_MAKE(0, 11));
	} else {
		_ClearFlag(FLAG_ID_MAKE(0, 11));
	}
	if(_CheckFlag(FLAG_ID_MAKE(1, 2)) || _CheckFlag(FLAG_ID_MAKE(1, 3)) || _CheckFlag(FLAG_ID_MAKE(1, 4)) || _CheckFlag(FLAG_ID_MAKE(1, 5)) || _CheckFlag(FLAG_ID_MAKE(1, 6))) {
		clear_flags = 1;
	} else {
		clear_flags = 0;
	}
	if(!clear_flags) {
		_ClearFlag(FLAG_ID_MAKE(1, 2));
		_ClearFlag(FLAG_ID_MAKE(1, 3));
		_ClearFlag(FLAG_ID_MAKE(1, 4));
		_ClearFlag(FLAG_ID_MAKE(1, 5));
		_ClearFlag(FLAG_ID_MAKE(1, 6));
	}
	_ClearFlag(FLAG_ID_MAKE(1, 8));
	if(_CheckFlag(FLAG_ID_MAKE(1, 2)) || _CheckFlag(FLAG_ID_MAKE(1, 3)) || _CheckFlag(FLAG_ID_MAKE(1, 4)) || _CheckFlag(FLAG_ID_MAKE(1, 5)) || _CheckFlag(FLAG_ID_MAKE(1, 6))) {
		reset_unk32 = 1;
	} else {
		reset_unk32 = 0;
	}
	if(!reset_unk32) {
		GWSystem.unk_32 = 1;
	}
	guest_status = BoardDataDirReadAsync(MAKE_DIR_NUM(DATADIR_BGUEST));
	if(guest_status != -1) {
		BoardDataAsyncWait(guest_status);
	}
	fn_8007111C();
	BoardModelInit();
	BoardRandInit();
	BoardWinInit();
	BoardPlayerModelInit();
	createFunc();
	BoardLightSetExec();
	fn_8007A8D8();
	fn_80077ABC();
	fn_800A4F7C();
	BoardCameraInit();
	BoardStatusCreate();
	CharModelKillIndex(-1);
	BoardPlayerInit();
	if(GWSystem.last5_effect == 2) {
		BoardSpaceTypeForce(2, 3);
	}
	if(GWSystem.last5_effect == 3) {
		BoardSpaceTypeForce(2, 7);
	}
	BoardCameraMoveSet(0);
	BoardCameraTargetPlayerSet(0);
	BoardCameraViewSet(2);
	BoardCameraMotionWait();
	BoardTauntInit();
	_SetFlag(FLAG_ID_MAKE(1, 14));
	HuDataDirClose(MAKE_DIR_NUM(DATADIR_BKOOPASUIT));
	HuDataDirClose(MAKE_DIR_NUM(DATADIR_BBATTLE));
	HuDataDirClose(MAKE_DIR_NUM(DATADIR_BKOOPA));
	HuDataDirClose(MAKE_DIR_NUM(DATADIR_BKUJIYA));
	HuDataDirClose(MAKE_DIR_NUM(DATADIR_BYOKODORI));
	HuDataDirClose(MAKE_DIR_NUM(DATADIR_BPAUSE));
	HuDataDirClose(MAKE_DIR_NUM(DATADIR_BLAST5));
	HuDataDirClose(MAKE_DIR_NUM(DATADIR_EFFECT));
	_SetFlag(FLAG_ID_MAKE(1, 16));
}

static void DestroyBoard(void)
{
	s32 dir_table[] = {
		MAKE_DIR_NUM(DATADIR_W01),
		MAKE_DIR_NUM(DATADIR_W02),
		MAKE_DIR_NUM(DATADIR_W03),
		MAKE_DIR_NUM(DATADIR_W04),
		MAKE_DIR_NUM(DATADIR_W05),
		MAKE_DIR_NUM(DATADIR_W06),
		MAKE_DIR_NUM(DATADIR_W10),
		MAKE_DIR_NUM(DATADIR_W20),
		MAKE_DIR_NUM(DATADIR_W21),
	};
	BoardTauntKill();
	fn_8007116C();
	HuAudAllStop();
	fn_80085EB4();
	BoardStatusKill();
	fn_800A5030();
	fn_80077B90();
	fn_8007AFF4(); 
	BoardSpaceDestroy();
	BoardPlayerModelKill();
	if(destroyFunc) {
		destroyFunc();
	}
	BoardModelKillAll();
	BoardWinKillAll();
	HuDataDirClose(dir_table[BoardCurrGet()]);
	HuDataDirClose(MAKE_DIR_NUM(DATADIR_EFFECT));
	HuDataDirClose(MAKE_DIR_NUM(DATADIR_BGUEST));
	HuDataDirClose(MAKE_DIR_NUM(DATADIR_BKOOPASUIT));
	HuDataDirClose(MAKE_DIR_NUM(DATADIR_BBATTLE));
	HuDataDirClose(MAKE_DIR_NUM(DATADIR_BKOOPA));
	HuDataDirClose(MAKE_DIR_NUM(DATADIR_BKUJIYA));
	HuDataDirClose(MAKE_DIR_NUM(DATADIR_BYOKODORI));
	HuDataDirClose(MAKE_DIR_NUM(DATADIR_BOARD));
	createFunc = destroyFunc = NULL;
}

void BoardCameraBackup(void)
{
	cameraUseBackup = TRUE;
	memcpy(&cameraBackup, &boardCamera, sizeof(BoardCameraData));
}

void BoardCameraRestore(void)
{
	if(cameraUseBackup) {
		memcpy(&boardCamera, &cameraBackup, sizeof(BoardCameraData));
		cameraUseBackup = FALSE;
	}
}

void BoardCameraScissorSet(s32 x, s32 y, s32 w, s32 h)
{
	BoardCameraData *camera = &boardCamera;
	Hu3DCameraScissorSet(camera->mask, x, y, w, h);
}

void BoardCameraViewSet(s32 type)
{
	BoardCameraData *camera = &boardCamera;
	BoardFocusData *focus;
	float size;
	if(!cameraObj) {
		return;
	}
	focus = &camera->focus;
	if(type == 0) {
		focus->view_type = 0;
		return;
	}
	OSs16tof32(&camViewTbl[type].fov, &focus->fov_end);
	focus->fov_start = camera->fov;
	OSs16tof32(&camViewTbl[type].zoom, &focus->zoom_end);
	focus->zoom_start = camera->zoom;
	OSs16tof32(&camViewTbl[type].x_rot, &focus->rot_end.x);
	focus->rot_end.y = 0;
	focus->rot_end.z = 0;
	focus->rot_start = camera->rot;
	focus->target_start = camera->target;
	if(BoardPlayerSizeGet(GWSystem.player_curr) == 2 || GWPlayer[GWSystem.player_curr].bowser_suit) {
		focus->zoom_end += 400.0f;
		size = 2.5f;
	} else {
		size = 1.0f;
	}
	BoardCameraTargetModelSet(BoardPlayerModelGetCurr());
	BoardPlayerPosGet(GWSystem.player_curr, &focus->target_end);
	BoardCameraOffsetSet(0.0f, 100.0f*size, 0.0f);
	focus->target_end.y += 100.0f*size;
	_SetFlag(FLAG_ID_MAKE(1, 21));
	focus->view_type = type;
	focus->time = 0;
	if(camera->moving) {
		focus->max_time = 21;
	} else {
		focus->max_time = 1;
	}
}

s32 BoardCameraPosGet(Vec *dst)
{
	BoardCameraData *camera;
	if(!dst) {
		return -1;
	}
	camera = &boardCamera;
	*dst = camera->pos;
	return 0;
}

s32 BoardCameraTargetGet(Vec *dst)
{
	BoardCameraData *camera;
	if(!dst) {
		return -1;
	}
	camera = &boardCamera;
	*dst = camera->target;
	return 0;
}

s32 BoardCameraRotGet(Vec *dst)
{
	BoardCameraData *camera;
	if(!dst) {
		return -1;
	}
	camera = &boardCamera;
	*dst = camera->rot;
	return 0;
}

float BoardCameraZoomGet()
{
	BoardCameraData *camera;
	camera = &boardCamera;
	return camera->zoom;
}

s32 BoardCameraDirGet(Vec *dst)
{
	BoardCameraData *camera;
	if(!dst) {
		return -1;
	}
	camera = &boardCamera;
	if(!camera) {
		return -1;
	}
	VECSubtract(&camera->target, &camera->pos, dst);
	if(dst->x != 0 || dst->y != 0 || dst->z != 0) {
		VECNormalize(dst, dst);
	}
	return 0;
}

s32 BoardCameraPointDirGet(Vec *point, Vec *dst)
{
	BoardCameraData *camera;
	if(!dst || !point) {
		return -1;
	}
	camera = &boardCamera;
	if(!camera) {
		return -1;
	}
	VECSubtract(point, &camera->pos, dst);
	if(dst->x != 0 || dst->y != 0 || dst->z != 0) {
		VECNormalize(dst, dst);
	}
	return 0;
}

void BoardCameraMaskSet(u16 mask)
{
	BoardCameraData *camera = &boardCamera;
	if(!camera) {
		return;
	}
	camera->mask = mask;
}

void BoardCameraMoveSet(s32 move)
{
	BoardCameraData *camera = &boardCamera;
	if(!camera) {
		return;
	}
	if(!move) {
		camera->moving = 0;
	} else {
		camera->moving = 1;
	}
}

void BoardCameraOffsetSet(float x, float y, float z)
{
	BoardCameraData *camera = &boardCamera;
	if(!camera) {
		return;
	}
	camera->offset.x = x;
	camera->offset.y = y;
	camera->offset.z = z;
}

void BoardCameraTargetPlayerSet(s32 player)
{
	PlayerState *player_ptr = BoardPlayerGet(player);
	if(!player_ptr) {
		BoardCameraTargetModelSet(-1);
		return;
	}
	BoardCameraTargetModelSet(BoardPlayerModelGet(player));
	BoardCameraOffsetSet(0, 100, 0);
}

void BoardCameraTargetModelSet(s16 model)
{
	BoardCameraData *camera = &boardCamera;
	if(!camera) {
		return;
	}
	camera->target_mdl = model;
	camera->target_space = -1;
	camera->offset.x = camera->offset.y = camera->offset.z = 0;
}

void BoardCameraTargetSpaceSet(s32 space)
{
	BoardCameraData *camera = &boardCamera;
	if(!camera) {
		return;
	}
	camera->target_mdl = -1;
	camera->target_space = space;
	camera->offset.x = camera->offset.y = camera->offset.z = 0;
}

void BoardCameraPosCalcFuncSet(BoardCameraPosCalcFunc func)
{
	BoardCameraData *camera = &boardCamera;
	if(!camera) {
		return;
	}
	camera->pos_calc = func;
}

void BoardCameraQuakeSet(s32 duration, float strength)
{
	BoardCameraData *camera = &boardCamera;
	if(!camera) {
		return;
	}
	camera->quaking = 1;
	camera->quake_strength = strength;
	camera->quake_timer = duration;
}

void BoardCameraQuakeReset()
{
	BoardCameraData *camera = &boardCamera;
	camera->quaking = 0;
	camera->quake_strength = 0;
	camera->quake_timer = 0;
}

void BoardCameraTargetSet(float x, float y, float z)
{
	BoardCameraData *camera = &boardCamera;
	camera->target.x = x;
	camera->target.y = y;
	camera->target.z = z;
}

void BoardCameraPosSet(float x, float y, float z)
{
	BoardCameraData *camera = &boardCamera;
	camera->pos.x = x;
	camera->pos.y = y;
	camera->pos.z = z;
}

void BoardCameraXRotZoomSet(float zoom, float x_rot)
{
	BoardCameraData *camera = &boardCamera;
	camera->zoom = zoom;
	camera->rot.x = x_rot;
}

void BoardCameraZoomSet(float zoom)
{
	BoardCameraData *camera = &boardCamera;
	camera->zoom = zoom;
}

void BoardCameraRotSet(float x, float y)
{
	BoardCameraData *camera = &boardCamera;
	camera->rot.x = x;
	camera->rot.y = y;
}

void BoardCameraNearFarSet(float near, float far)
{
	BoardCameraData *camera = &boardCamera;
	camera->near = near;
	camera->far = far;
}

void BoardCameraNearFarGet(float *near, float *far)
{
	BoardCameraData *camera = &boardCamera;
	if(near) {
		*near = camera->near;
	}
	if(far) {
		*far = camera->far;
	}
}

void BoardCameraMotionStart(s16 model_target, Vec *rot_target, float zoom_target, float fov_target)
{
	BoardCameraMotionStartEx(model_target, rot_target, 0, zoom_target, fov_target, 21);
}

void BoardCameraMotionStartEx(s16 model_target, Vec *rot_target, Vec *offset_end, float zoom_target, float fov_target, s16 max_time)
{
	BoardCameraData *camera = &boardCamera;
	BoardFocusData *focus;
	if(!cameraObj) {
		return;
	}
	focus = &camera->focus;
	focus->fov_start = camera->fov;
	focus->zoom_start = camera->zoom;
	focus->rot_start = camera->rot;
	focus->target_start = camera->target;
	if(fov_target == -1) {
		focus->fov_end = focus->fov_start;
	} else {
		focus->fov_end = fov_target;
	}
	if(zoom_target == -1) {
		focus->zoom_end = focus->zoom_start;
	} else {
		focus->zoom_end = zoom_target;
	}
	if(!rot_target) {
		focus->rot_end = focus->rot_start;
	} else {
		focus->rot_end = *rot_target;
	}
	if(model_target == -1) {
		focus->target_end = focus->target_start;
	} else {
		BoardCameraTargetModelSet(model_target);
		BoardModelPosGet(model_target, &focus->target_end);
	}
	if(offset_end) {
		BoardCameraOffsetSet(offset_end->x, offset_end->y, offset_end->z);
		VECAdd(offset_end, &focus->target_end, &focus->target_end);
	}
	_SetFlag(FLAG_ID_MAKE(1, 21));
	focus->view_type = 5;
	focus->time = 0;
	if(max_time < 0) {
		max_time = 1;
	}
	focus->max_time = max_time;
}

void BoardCameraFovSet(float fov)
{
	BoardCameraData *camera = &boardCamera;
	camera->fov = fov;
}

s32 BoardCameraCullCheck(Vec *point, float radius)
{
	Vec dir;
	Vec pos;
	float dist;
	float dot;
	BoardCameraData *camera = &boardCamera;
	if(!camera->hide_all) {
		return 0;
	}
	BoardCameraDirGet(&dir);
	VECSubtract(point, &camera->pos, &pos);
	dist = sqrtf((pos.x*pos.x)+(pos.y*pos.y)+(pos.z*pos.z));
	if(25000 < dist-(radius*2.0f)) {
		return 0;
	}
	BoardCameraPointDirGet(point, &pos);
	dot = VECDotProduct(&dir, &pos);
	if(BOARD_FABS(dot) < cos((camera->fov*M_PI)/180)) {
		return 0;
	} else {
		return 1;
	}
}

s32 BoardCameraMotionIsDone(void)
{
	return (_CheckFlag(FLAG_ID_MAKE(1, 21))) ? 0 : 1;
}

void BoardCameraMotionWait(void)
{
	while(!BoardCameraMotionIsDone()) {
		HuPrcVSleep();
	}
	HuPrcVSleep();
}

void BoardCameraInit(void)
{
	BoardCameraData *camera;
	cameraUseBackup = FALSE;
	memset(&cameraBackup, 0, sizeof(BoardCameraData));
	memset(&boardCamera, 0, sizeof(BoardCameraData));
	camera = &boardCamera;
	camera->fov = 25;
	camera->near = 100;
	camera->far = 13000;
	camera->aspect = 1.2;
	camera->viewport_x = 0;
	camera->viewport_y = 0;
	camera->viewport_h = 480;
	camera->viewport_w = 640;
	camera->viewport_near = 0;
	camera->viewport_far = 1;
	camera->up.y = 1;
	camera->moving = 0;
	camera->quaking = 0;
	camera->quake_timer = 0;
	camera->pos.x = 0;
	camera->pos.y = 5000;
	camera->pos.z = 5000;
	camera->offset.x = 0;
	camera->offset.y = 0;
	camera->offset.z = 0;
	camera->pos_calc = NULL;
	BoardSpaceFlagPosGet(0, 0x80000000, &camera->target);
	camera->rot.x = camViewTbl[1].x_rot;
	camera->zoom = camViewTbl[1].zoom;
	camera->hide_all = 1;
	camera->mask = 1;
	Hu3DCameraCreate(1);
	Hu3DCameraScissorSet(1, camera->viewport_x, camera->viewport_y, camera->viewport_w, camera->viewport_h);
	Hu3DCameraScissorSet(2, 0, 0, 0, 0);
	Hu3DCameraPerspectiveSet(2, -1, 100, 13000, 1.2);
	cameraObj = omAddObjEx(boardObjMan, 32258, 0, 0, -1, UpdateCamera);
}

static void CalcCameraView(void)
{
	BoardCameraData *camera = &boardCamera;
	CalcCameraTarget(camera);
	CalcCameraPos(camera);
}

static void UpdateCamera(omObjData *object)
{
	BoardCameraData *camera;
	Vec *target;
	float x, y, z;
	if(BoardIsKill()) {
		omDelObjEx(HuPrcCurrentGet(), object);
		return;
	}
	CalcCameraView();
	camera = &boardCamera;
	Hu3DCameraPerspectiveSet(camera->mask, camera->fov, camera->near, camera->far, camera->aspect);
	Hu3DCameraViewportSet(camera->mask, camera->viewport_x, camera->viewport_y, camera->viewport_w, camera->viewport_h, camera->viewport_near, camera->viewport_far);
	target = &camera->target;
	if(camera->pos_calc) {
		camera->pos_calc(camera);
	} else {
		
		x = camera->rot.x;
		y = camera->rot.y;
		camera->pos.x = target->x+(sin((y*M_PI)/180.0)*cos((x*M_PI)/180.0)*camera->zoom);
		camera->pos.y = target->y+(-sin((x*M_PI)/180.0)*camera->zoom);
		camera->pos.z = target->z+(cos((y*M_PI)/180.0)*cos((x*M_PI)/180.0)*camera->zoom);
		camera->up.x = sin((y*M_PI)/180.0)*sin((x*M_PI)/180.0);
		camera->up.y = cos((x*M_PI)/180.0);
		camera->up.z = cos((y*M_PI)/180.0)*sin((x*M_PI)/180.0);
		if(camera->quaking) {
			x = BoardRandFloat();
			y = BoardRandFloat();
			z = BoardRandFloat();
			camera->pos.x += (x-0.5f)*camera->quake_strength;
			camera->pos.y += (y-0.5f)*camera->quake_strength;
			camera->pos.z += (z-0.5f)*camera->quake_strength;
			if(--camera->quake_timer <= 0) {
				camera->quaking = 0;
				camera->quake_timer = 0;
				camera->quake_strength = 0;
			}
		}
	}
	Hu3DCameraPosSetV(camera->mask, &camera->pos, &camera->up, target);
}

static void CalcCameraTarget(BoardCameraData *camera)
{
	Vec offset;
	Vec pos = { 0, 0, 0 };
	BoardFocusData *focus = &camera->focus;
	if(camera->target_mdl != -1) {
		BoardModelPosGet(camera->target_mdl, &pos);
	} else {
		if(camera->target_space != -1) {
			BoardSpacePosGet(0, camera->target_space, &pos);
		} else {
			return;
		}
	}
	VECAdd(&camera->offset, &pos, &pos);
	VECSubtract(&pos, &camera->target, &offset);
	if(camera->moving) {
		VECScale(&offset, &offset, 0.15f);
	}
	VECAdd(&offset, &camera->target, &camera->target);
}

#define CAM_LERP(t, x1, x2, out) \
{ \
	float result; \
	float offset; \
	float unit_scale; \
	float lerp_t; \
	lerp_x2 = x2; \
	lerp_x1 = x1; \
	lerp_t = t; \
	if(lerp_t2 == lerp_t1) { \
		result = lerp_x2; \
	} else { \
		unit_scale = (lerp_x1-lerp_x2)/((float)lerp_t1-(float)lerp_t2); \
		offset = lerp_x2-(unit_scale*lerp_t2); \
		result = offset+(unit_scale*lerp_t); \
	} \
	out = result; \
}

#define CAM_LERP_VEC(t, x1, x2, out) \
CAM_LERP(t, (x1).x, (x2).x, (out).x) \
CAM_LERP(t, (x1).y, (x2).y, (out).y) \
CAM_LERP(t, (x1).z, (x2).z, (out).z)

static void CalcCameraPos(BoardCameraData *camera)
{
	
	volatile unsigned int lerp_t1, lerp_t2;
	volatile float lerp_x1, lerp_x2;
	float time;
	
	BoardFocusData *focus = &camera->focus;
	if(focus->time > focus->max_time) {
		focus->view_type = 0;
		_ClearFlag(FLAG_ID_MAKE(1, 21));
		return;
	}
	OSs16tof32(&focus->time, &time);
	focus->time++;
	lerp_t2 = focus->max_time;
	lerp_t1 = 0;
	CAM_LERP(time, focus->zoom_start, focus->zoom_end, camera->zoom)
	CAM_LERP_VEC(time, focus->rot_start, focus->rot_end, camera->rot)
	CAM_LERP(time, focus->fov_start, focus->fov_end, camera->fov)
	CAM_LERP_VEC(time, focus->target_start, focus->target_end, camera->target)
}

float BoardArcSin(float value)
{
	float result;
	s32 sign;
	if(value < 0) {
		sign = 1;
		value = BOARD_FABS(value);
	} else {
		sign = 0;
	}
	if(value > 1.0f) {
		return 0;
	}
	if(value <= (float)(M_PI/2)) {
		result = atanf(value/(float)sqrtf(1-(value*value)));
	} else {
		result = 1.0f-atanf((float)sqrtf(1-(value*value))/value);
	}
	if(sign) {
		result = BOARD_FABS(result);
	}
	return result;
}

float BoardArcCos(float value)
{
	if(BOARD_FABS(value) > 1) {
		return 0;
	}
	return 1.0f-BoardArcSin(value);
}

void BoardRandInit(void)
{
	boardRandSeed = OSGetTime();
}

u32 BoardRand(void)
{
	boardRandSeed = (boardRandSeed*0x19660D)+(0x3C6EF35F);
	return boardRandSeed;
}

u32 BoardRandMod(u32 value)
{
	return (BoardRand() & 0x7FFFFFFF)%value;
}

float BoardRandFloat(void)
{
	float value;
	*((u32 *)&value) = (BoardRand() & 0x7FFFFF)|0x3F800000;
	return value-1.0f;
}

float BoardVecDistXZCalc(Vec *vec1, Vec *vec2)
{
	float x = vec1->x-vec2->x;
	float z = vec1->z-vec2->z;
	return sqrtf((x*x)+(z*z));
}

s32 BoardVecMaxDistXZCheck(Vec *vec1, Vec *vec2, float max_dist)
{
	float z = vec1->z-vec2->z;
	float x = vec1->x-vec2->x;
	float dist = sqrtf((x*x)+(z*z));
	if(dist <= max_dist) {
		return 1;
	} else {
		return 0;
	}
}

s32 BoardVecMinDistCheck(Vec *vec1, Vec *vec2, float min_dist)
{
	Vec temp;
	Mtx temp_mtx;
	VECSubtract(vec1, vec2, &temp);
	if(VECSquareMag(&temp) >= (min_dist*min_dist)) {
		return 0;
	} else {
		return 1;
	}
}

s32 BoardDataDirReadAsync(s32 data_num)
{
	s32 status = HuDataDirReadAsync(data_num);
	return status;
}

void BoardDataAsyncWait(s32 status)
{
	if(status == -1) {
		return;
	}
	while(!HuDataGetAsyncStat(status)) {
		HuPrcVSleep();
	}
}