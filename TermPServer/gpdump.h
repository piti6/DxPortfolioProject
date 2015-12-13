#pragma once

#include <d3d9.h>  // directX includes
#include <d3dx9tex.h>
#include <dinput.h>
#include <objbase.h>
#include <MMSystem.h>
#include <dsound.h>
#include <dshow.h>


#define MAX_SOUNDS     64 // max number of sounds in system at once 

#define SOUND_NULL     0
#define SOUND_LOADED   1
#define SOUND_PLAYING  2
#define SOUND_STOPPED  3

#define INITGUID

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

// defines for BOBs
#define BOB_STATE_DEAD     0      // this is a dead bob
#define BOB_STATE_ALIVE    1      // this is a live bob
#define BOB_STATE_LOADED   2      // the bob has been loaded

#define MAX_TEXTURE 64

#define MOUSE_INIT_X 400
#define MOUSE_INIT_Y 300

#define BITMAP_EXTRACT_MODE_CELL 0
#define DSVOLUME_TO_DB(volume) ((DWORD)(-30*(100 - volume)))
#ifndef DSBCAPS_CTRLDEFAULT
#define DSBCAPS_CTRLDEFAULT (DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME )
#endif

// defines for BOBs
#define BOB_STATE_ANIM_DONE    1    // done animation state
#define MAX_BOB_FRAMES         64   // maximum number of bob frames
#define MAX_BOB_ANIMATIONS     16   // maximum number of animation sequeces

#define BOB_ATTR_SINGLE_FRAME   1   // bob has single frame
#define BOB_ATTR_MULTI_FRAME    2   // bob has multiple frames
#define BOB_ATTR_MULTI_ANIM     4   // bob has multiple animations
#define BOB_ATTR_ANIM_ONE_SHOT  8   // bob will perform the animation once
#define BOB_ATTR_VISIBLE        16  // bob is visible
#define BOB_ATTR_BOUNCE         32  // bob bounces off edges
#define BOB_ATTR_WRAPAROUND     64  // bob wraps around edges
#define BOB_ATTR_LOADED         128 // the bob has been loaded

#define ALIVE 1
#define DEAD 0

#ifndef DSBCAPS_CTRLDEFAULT
#define DSBCAPS_CTRLDEFAULT (DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME )
#endif






extern	IGraphBuilder   *g_pGraphBuilder;
extern	IMediaControl   *g_pMediaControl;
extern	IMediaSeeking   *g_pMediaSeeking;
extern	IMediaEventEx   *g_pMediaEventEx;

extern	bool	g_bMusicEnabled;

typedef struct BITMAP_OBJ_TYP
{
	int state; // the state of the object (general)
	int attr;  // attributes pertaining to the object (general)
	int x,y;   // position bitmap will be displayed at
	int xv,yv; // velocity of object
	int width, height; // the width and height of the bitmap
	int image; // the bitmap surface itself
	
	int my_cx,my_cy;
	RECT frame[MAX_BOB_FRAMES];
	int curr_frame;
	int max_frame;
	int anim_speed;
	int anim_speed_count;
	int varsI[10];

	wchar_t my_name[256];
} BITMAP_OBJ, *BITMAP_OBJ_PTR;
typedef struct pcm_sound_typ
	{
	LPDIRECTSOUNDBUFFER dsbuffer;   // the ds buffer containing the sound
	int state;                      // state of the sound
	int rate;                       // playback rate
	int size;                       // size of sound
	int id;                         // id number of the sound
	} pcm_sound, *pcm_sound_ptr;

int Name_BOB16(BITMAP_OBJ_PTR bob, wchar_t *name);
int Draw_BOB16(BITMAP_OBJ_PTR bob,int texture_id);
int Draw_BOB16(BITMAP_OBJ_PTR bob);
int Destroy_BOB16(BITMAP_OBJ_PTR bob);
int Create_BOB16(BITMAP_OBJ_PTR bob,int width, int height,int attr);
int Load_BOB16(BITMAP_OBJ_PTR bob,int texture_id ,int cx,int cy,int mode); 
int Load_Texture(wchar_t *filename, int texture_id);
int Draw_Text(wchar_t *text, int x, int y, D3DCOLOR color);
wchar_t *ToWchar(char *c);
int Draw_Small_Text(char *text, int x, int y, D3DCOLOR color);
int Draw_MID_Text(wchar_t *text, int x, int y, D3DCOLOR color);
int Draw_BIG_Text(wchar_t *text, int x, int y, D3DCOLOR color);
int DrawName_BOB16(BITMAP_OBJ_PTR bob);
int Load_Frame_BOB16(BITMAP_OBJ_PTR bob,int texture_id,int frame_id, int cx,int cy,int mode); 
void Set_Anim_Speed_BOB16(BITMAP_OBJ_PTR bob,int speed);
void Set_Vel_BOB16(BITMAP_OBJ_PTR bob, int xv, int xy);
void Set_Pos_BOB16(BITMAP_OBJ_PTR bob, int x, int y);
void Animate_BOB16(BITMAP_OBJ_PTR bob);
void Animate_BOB16(BITMAP_OBJ_PTR bob,int start_frame,int max_frame);


int Engine_init();
int Engine_Shutdown();
int Flip();
int BackBuffer_Clear();
int Draw_Begin();
int Draw_End();

void GetInputData();
void MoveMouse();
void Move_Enemy_BOB16(BITMAP_OBJ_PTR bob);

int Load_WAV(wchar_t *filename, int control_flags=DSBCAPS_CTRLDEFAULT);
int Replicate_Sound(int source_id);
int DSound_Init(void);
int DSound_Shutdown(void);
int Play_Sound(int id, int flags=0, int volume=0, int rate=0, int pan=0);

int Stop_Sound(int id);
int Delete_All_Sounds(void);
int Delete_Sound(int id);
int Stop_All_Sounds(void);
int Status_Sound(int id);




int MP3_Init();
void MP3_Shutdown();

IBaseFilter *LoadMP3(wchar_t *fname);
void PlayMP3(IBaseFilter *pSource);
void ReplayMP3();
void StopMP3();

void HandleMP3Events();

// User-defined function
void OnMP3Finish(long);

extern HWND main_window_handle;
extern HINSTANCE main_instance;
