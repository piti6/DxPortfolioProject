// INCLUDES ///////////////////////////////////////////////
#define WIN32_LEAN_AND_MEAN  
#define INITGUID


#include <windows.h>   // include important windows stuff
#include <windowsx.h> 
#include <memory>

#include "gpdump.h"


 


/////////////////////////////////////////////////////////////
LPDIRECTINPUT8       lpdi      = NULL;    // dinput object
LPDIRECTINPUTDEVICE8 lpdikey   = NULL;    // dinput keyboard
LPDIRECTINPUTDEVICE8 lpdimouse = NULL;    // dinput mouse

LPDIRECT3D9             g_pD3D       = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // Our rendering device
LPDIRECT3DTEXTURE9		g_pTexture[MAX_TEXTURE];
LPD3DXSPRITE			g_pSprite;
LPD3DXFONT				g_pFont;
LPD3DXFONT				g_pFont2;
LPD3DXFONT				g_pFont3;
LPD3DXFONT				g_pFont4;

// directsound stuff
LPDIRECTSOUND		lpds;           // directsound interface pointer
DSBUFFERDESC		dsbd;           // directsound description
DSCAPS				dscaps;         // directsound caps
HRESULT				dsresult;       // general directsound result
DSBCAPS				dsbcaps;        // directsound buffer caps
LPDIRECTSOUNDBUFFER	lpdsbprimary;   // the primary mixing buffer
pcm_sound			sound_fx[MAX_SOUNDS];    // the array of secondary sound buffers

WAVEFORMATEX		pcmwf;          // generic waveformat structure


IGraphBuilder   *g_pGraphBuilder;
IMediaControl   *g_pMediaControl;
IMediaSeeking   *g_pMediaSeeking;
IMediaEventEx   *g_pMediaEventEx;

bool	g_bMusicEnabled;

extern	IGraphBuilder   *g_pGraphBuilder;
extern	IMediaControl   *g_pMediaControl;
extern	IMediaSeeking   *g_pMediaSeeking;
extern	IMediaEventEx   *g_pMediaEventEx;

extern	bool	g_bMusicEnabled;


int mouse_x,mouse_y;
UCHAR keyboard_state[256]; // contains keyboard state table
DIMOUSESTATE mouse_state;  // contains state of mouse

extern int mouse_x,mouse_y;
extern UCHAR keyboard_state[256]; // contains keyboard state table
extern DIMOUSESTATE mouse_state;  // contains state of mouse






int MP3_Init()
{
    HRESULT h;

    // Initialise COM, so that we can use CoCreateInstance.
    ::CoInitialize(NULL);
    
    // Create an IGraphBuilder object, through which 
    //  we will create a DirectShow graph.
    h = CoCreateInstance(CLSID_FilterGraph, NULL,
                                      CLSCTX_INPROC, IID_IGraphBuilder,
                                      (void **)&g_pGraphBuilder);
    if (FAILED(h)) return (0);

    // Get the IMediaControl Interface
    h = g_pGraphBuilder->QueryInterface(IID_IMediaControl,
                                 (void **)&g_pMediaControl);
    if (FAILED(h)) return (0);

    // Get the IMediaSeeking Interface
    h = g_pGraphBuilder->QueryInterface(IID_IMediaSeeking,
                                 (void **)&g_pMediaSeeking);
    if (FAILED(h)) return (0);
 
    // Get the IMediaEventEx Interface
    h = g_pGraphBuilder->QueryInterface(IID_IMediaEventEx,
                                 (void **)&g_pMediaEventEx);
    if (FAILED(h)) return (0);
    g_pMediaEventEx->SetNotifyWindow((OAHWND)main_window_handle, WM_APP, 0);

    return (1);
}

IBaseFilter *LoadMP3(wchar_t *wszFilename)
{
    IBaseFilter *pSource;
    HRESULT     h;

    // Add the new source filter to the graph.
    h = g_pGraphBuilder->AddSourceFilter(wszFilename, wszFilename, &pSource);
    if (FAILED(h))
        return NULL;

    return pSource;
}

void PlayMP3(IBaseFilter *pSource)
{
    IPin            *pPin = NULL;
    IEnumFilters    *pFilterEnum = NULL;
    IBaseFilter     **ppFilters;
    IBaseFilter     *pFilterTemp = NULL;
    int             iFiltCount;
    int             iPos;

    // Get the first output pin of the new source filter. Audio sources 
    // typically have only one output pin, so for most audio cases finding 
    // any output pin is sufficient.
    pSource->FindPin(L"Output", &pPin);  

    // Stop the graph
    g_pMediaControl->Stop();

    // Break all connections on the filters. You can do this by adding 
    // and removing each filter in the graph

    g_pGraphBuilder->EnumFilters(&pFilterEnum);
    
    // Need to know how many filters. If we add/remove filters during the
    // enumeration we'll invalidate the enumerator
    for (iFiltCount = 0; pFilterEnum->Skip(1) == S_OK; iFiltCount++)
        ;

    // Allocate space, then pull out all of the 
    ppFilters = (IBaseFilter **)malloc(sizeof(IBaseFilter *) * iFiltCount);

    pFilterEnum->Reset();

    for (iPos = 0; pFilterEnum->Next(1, &ppFilters[iPos], NULL) == S_OK; iPos++)
        ;
    
    pFilterEnum->Release();

    for (iPos = 0; iPos < iFiltCount; iPos++) 
    {
        g_pGraphBuilder->RemoveFilter(ppFilters[iPos]);
        
        // Put the filter back
        g_pGraphBuilder->AddFilter(ppFilters[iPos], NULL);

        ppFilters[iPos]->Release();
    }

    // We have the new output pin. Render it
    g_pGraphBuilder->Render(pPin);

    pPin->Release();

    // Re-seek the graph to the beginning
    LONGLONG llPos = 0;
    g_pMediaSeeking->SetPositions(&llPos, AM_SEEKING_AbsolutePositioning,
                                &llPos, AM_SEEKING_NoPositioning);

    // Start the graph
    g_pMediaControl->Run();
}

void ReplayMP3()
{
    if (g_pMediaControl != NULL)
    {
        // Re-seek the graph to the beginning
        LONGLONG llPos = 0;
        g_pMediaSeeking->SetPositions(&llPos, AM_SEEKING_AbsolutePositioning,
                                    &llPos, AM_SEEKING_NoPositioning);

        // Start the graph
        g_pMediaControl->Run();
    }
}

void StopMP3()
{
    // Stop the graph
    if (g_pMediaControl != NULL)
        g_pMediaControl->Stop();
}

void MP3_Shutdown()
{
    if (g_pMediaEventEx != NULL)
    {
        g_pMediaEventEx->Release();
        g_pMediaEventEx = NULL;
    }
    if (g_pMediaSeeking != NULL)
    {
        g_pMediaSeeking->Release();
        g_pMediaSeeking = NULL;
    }
    if (g_pMediaControl != NULL)
    {
        g_pMediaControl->Release();
        g_pMediaControl = NULL;
    }
    if (g_pGraphBuilder != NULL)
    {
        g_pGraphBuilder->Release();
        g_pGraphBuilder = NULL;
    }
}

void HandleMP3Events()
{
    long    evCode, param1, param2;
    HRESULT h;

    for (;;)
    { 
        h = g_pMediaEventEx->GetEvent(&evCode, &param1, &param2, 0);
        if (FAILED(h))
            return;
        
        g_pMediaEventEx->FreeEventParams(evCode, param1, param2);
        OnMP3Finish(evCode);
    } 
}

void OnMP3Finish(long)
{
    // Restart background music if it has stopped
    if (g_bMusicEnabled)
        ReplayMP3();
}




int Engine_init()
{
	if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return 0;

	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
		main_window_handle,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice ) ) )
		return(0);	// return fail

	MP3_Init();
	DSound_Init();
	if (DirectInput8Create(main_instance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&lpdi,NULL)!=DI_OK)
		return(0);

	// create a keyboard device  //////////////////////////////////
	if (lpdi->CreateDevice(GUID_SysKeyboard, &lpdikey, NULL)!=DI_OK)
		return(0);
	if (lpdi->CreateDevice(GUID_SysMouse, &lpdimouse, NULL)!=DI_OK)
		return(0);

	// set cooperation level
	if (lpdikey->SetCooperativeLevel(main_window_handle, 
		DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)!=DI_OK)
		return(0);
	if (lpdimouse->SetCooperativeLevel(main_window_handle, DISCL_EXCLUSIVE | DISCL_FOREGROUND)!=DI_OK)
		return(0);

	// set data format
	if (lpdikey->SetDataFormat(&c_dfDIKeyboard)!=DI_OK)
		return(0);
	if (lpdimouse->SetDataFormat(&c_dfDIMouse)!=DI_OK)
	// acquire the keyboard
	if (lpdikey->Acquire()!=DI_OK)
		return(0);
	if (lpdimouse->Acquire()!=DI_OK)
		return(0);



	mouse_x=MOUSE_INIT_X;
	mouse_y=MOUSE_INIT_Y;

	D3DXCreateSprite(g_pd3dDevice, &g_pSprite);

	D3DXCreateFont(g_pd3dDevice, 20, 0, FW_BOLD, 
		0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		TEXT("±¼¸²"), &g_pFont);
	D3DXCreateFont(g_pd3dDevice, 100, 0, FW_BOLD, 
		0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		TEXT("±¼¸²"), &g_pFont2);
	D3DXCreateFont(g_pd3dDevice, 40, 0, FW_BOLD, 
		0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		TEXT("±¼¸²"), &g_pFont3);
	D3DXCreateFont(g_pd3dDevice, 15, 0, FW_BOLD, 
		0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		TEXT("±¼¸²"), &g_pFont4);
		
	return 1;

}

int Load_WAV(wchar_t *filename, int control_flags)
{
// this function loads a .wav file, sets up the directsound 
// buffer and loads the data into memory, the function returns 
// the id number of the sound


HMMIO 			hwav;    // handle to wave file
MMCKINFO		parent,  // parent chunk
                child;   // child chunk
WAVEFORMATEX    wfmtx;   // wave format structure

int	sound_id = -1,       // id of sound to be loaded
	index;               // looping variable

UCHAR *snd_buffer,       // temporary sound buffer to hold voc data
      *audio_ptr_1=NULL, // data ptr to first write buffer 
	  *audio_ptr_2=NULL; // data ptr to second write buffer

DWORD audio_length_1=0,  // length of first write buffer
	  audio_length_2=0;  // length of second write buffer
			
// step one: are there any open id's ?
for (index=0; index < MAX_SOUNDS; index++)
	{	
    // make sure this sound is unused
	if (sound_fx[index].state==SOUND_NULL)
	   {
	   sound_id = index;
	   break;
	   } // end if

	} // end for index

// did we get a free id?
if (sound_id==-1)
	return(-1);

// set up chunk info structure
parent.ckid 	    = (FOURCC)0;
parent.cksize 	    = 0;
parent.fccType	    = (FOURCC)0;
parent.dwDataOffset = 0;
parent.dwFlags		= 0;

// copy data
child = parent;

// open the WAV file
if ((hwav = mmioOpen(filename, NULL, MMIO_READ | MMIO_ALLOCBUF))==NULL)
    return(-1);

// descend into the RIFF 
parent.fccType = mmioFOURCC('W', 'A', 'V', 'E');

if (mmioDescend(hwav, &parent, NULL, MMIO_FINDRIFF))
    {
    // close the file
    mmioClose(hwav, 0);

    // return error, no wave section
    return(-1); 	
    } // end if

// descend to the WAVEfmt 
child.ckid = mmioFOURCC('f', 'm', 't', ' ');

if (mmioDescend(hwav, &child, &parent, 0))
    {
    // close the file
    mmioClose(hwav, 0);

    // return error, no format section
    return(-1); 	
    } // end if

// now read the wave format information from file
if (mmioRead(hwav, (char *)&wfmtx, sizeof(wfmtx)) != sizeof(wfmtx))
    {
    // close file
    mmioClose(hwav, 0);

    // return error, no wave format data
    return(-1);
    } // end if

// make sure that the data format is PCM
if (wfmtx.wFormatTag != WAVE_FORMAT_PCM)
    {
    // close the file
    mmioClose(hwav, 0);

    // return error, not the right data format
    return(-1); 
    } // end if

// now ascend up one level, so we can access data chunk
if (mmioAscend(hwav, &child, 0))
   {
   // close file
   mmioClose(hwav, 0);

   // return error, couldn't ascend
   return(-1); 	
   } // end if

// descend to the data chunk 
child.ckid = mmioFOURCC('d', 'a', 't', 'a');

if (mmioDescend(hwav, &child, &parent, MMIO_FINDCHUNK))
    {
    // close file
    mmioClose(hwav, 0);

    // return error, no data
    return(-1); 	
    } // end if

// finally!!!! now all we have to do is read the data in and
// set up the directsound buffer

// allocate the memory to load sound data
snd_buffer = (UCHAR *)malloc(child.cksize);

// read the wave data 
mmioRead(hwav, (char *)snd_buffer, child.cksize);

// close the file
mmioClose(hwav, 0);

// set rate and size in data structure
sound_fx[sound_id].rate  = wfmtx.nSamplesPerSec;
sound_fx[sound_id].size  = child.cksize;
sound_fx[sound_id].state = SOUND_LOADED;

// set up the format data structure
memset(&pcmwf, 0, sizeof(WAVEFORMATEX));

pcmwf.wFormatTag	  = WAVE_FORMAT_PCM;  // pulse code modulation
pcmwf.nChannels		  = 1;                // mono 
pcmwf.nSamplesPerSec  = 11025;            // always this rate
pcmwf.nBlockAlign	  = 1;                
pcmwf.nAvgBytesPerSec = pcmwf.nSamplesPerSec * pcmwf.nBlockAlign;
pcmwf.wBitsPerSample  = 8;
pcmwf.cbSize		  = 0;

// prepare to create sounds buffer
dsbd.dwSize			= sizeof(DSBUFFERDESC);
dsbd.dwFlags		= control_flags | DSBCAPS_STATIC | DSBCAPS_LOCSOFTWARE;
dsbd.dwBufferBytes	= child.cksize;
dsbd.lpwfxFormat	= &pcmwf;

// create the sound buffer
if (lpds->CreateSoundBuffer(&dsbd,&sound_fx[sound_id].dsbuffer,NULL)!=DS_OK)
   {
   // release memory
   free(snd_buffer);

   // return error
   return(-1);
   } // end if

// copy data into sound buffer
if (sound_fx[sound_id].dsbuffer->Lock(0,					 
								      child.cksize,			
								      (void **) &audio_ptr_1, 
								      &audio_length_1,
								      (void **)&audio_ptr_2, 
								      &audio_length_2,
								      DSBLOCK_FROMWRITECURSOR)!=DS_OK)
								 return(0);

// copy first section of circular buffer
memcpy(audio_ptr_1, snd_buffer, audio_length_1);

// copy last section of circular buffer
memcpy(audio_ptr_2, (snd_buffer+audio_length_1),audio_length_2);

// unlock the buffer
if (sound_fx[sound_id].dsbuffer->Unlock(audio_ptr_1, 
									    audio_length_1, 
									    audio_ptr_2, 
									    audio_length_2)!=DS_OK)
 							     return(0);

// release the temp buffer
free(snd_buffer);

// return id
return(sound_id);

} // end Load_WAV

int Replicate_Sound(int source_id)
{
// this function replicates the sent sound and sends back the
// id of the replicated sound, you would use this function
// to make multiple copies of a gunshot or something that
// you want to play multiple times simulataneously, but you
// only want to load once

if (source_id!=-1)
    {
    // duplicate the sound buffer
    // first hunt for an open id

    for (int id=0; id < MAX_SOUNDS; id++)
        {
        // is this sound open?
        if (sound_fx[id].state==SOUND_NULL)
            {
            // first make an identical copy
            sound_fx[id] = sound_fx[source_id];

            // now actually replicate the directsound buffer
            if (lpds->DuplicateSoundBuffer(sound_fx[source_id].dsbuffer,
                                           &sound_fx[id].dsbuffer)!=DS_OK)
                {
                // reset sound to NULL
                sound_fx[id].dsbuffer = NULL;
                sound_fx[id].state    = SOUND_NULL;

                // return error
                return(-1);
                } // end if

            // now fix up id
            sound_fx[id].id = id;
            
            // return replicated sound
            return(id);

            } // end if found
  
        } // end for id

    } // end if
else
   return(-1);
    
// else failure
return(-1);

} // end 


int DSound_Init(void)
{
// this function initializes the sound system
static int first_time = 1; // used to track the first time the function
                           // is entered

// test for very first time
if (first_time)
	{		
	// clear everything out
	memset(sound_fx,0,sizeof(pcm_sound)*MAX_SOUNDS);
	
	// reset first time
	first_time = 0;

	// create a directsound object
	if (DirectSoundCreate(NULL, &lpds, NULL)!=DS_OK )
		return(0);

	// set cooperation level
	if (lpds->SetCooperativeLevel((HWND)main_window_handle,DSSCL_NORMAL)!=DS_OK)
		return(0);

	} // end if

// initialize the sound fx array
for (int index=0; index<MAX_SOUNDS; index++)
	{
	// test if this sound has been loaded
	if (sound_fx[index].dsbuffer)
		{
		// stop the sound
		sound_fx[index].dsbuffer->Stop();

		// release the buffer
		sound_fx[index].dsbuffer->Release();
	
		} // end if

	// clear the record out
	memset(&sound_fx[index],0,sizeof(pcm_sound));

	// now set up the fields
	sound_fx[index].state = SOUND_NULL;
	sound_fx[index].id    = index;

	} // end for index

// return sucess
return(1);

} // end DSound_Init

///////////////////////////////////////////////////////////

int DSound_Shutdown(void)
{
// this function releases all the memory allocated and the directsound object
// itself

// first turn all sounds off
Stop_All_Sounds();

// now release all sound buffers
for (int index=0; index<MAX_SOUNDS; index++)
	if (sound_fx[index].dsbuffer)
		sound_fx[index].dsbuffer->Release();

// now release the directsound interface itself
lpds->Release();

// return success
return(1);

} // end DSound_Shutdown

///////////////////////////////////////////////////////////

int Play_Sound(int id, int flags, int volume, int rate, int pan)
{
// this function plays a sound, the only parameter that 
// works is the flags which can be 0 to play once or
// DSBPLAY_LOOPING

if (sound_fx[id].dsbuffer)
	{
	// reset position to start
	if (sound_fx[id].dsbuffer->SetCurrentPosition(0)!=DS_OK)
		return(0);
	
	// play sound
	if (sound_fx[id].dsbuffer->Play(0,0,flags)!=DS_OK)
		return(0);
	} // end if
	

// return success
return(1);

} // end Play_Sound

///////////////////////////////////////////////////////////

int Stop_Sound(int id)
{
// this function stops a sound from playing
if (sound_fx[id].dsbuffer)
   {
   sound_fx[id].dsbuffer->Stop();
   sound_fx[id].dsbuffer->SetCurrentPosition(0);
   } // end if

// return success
return(1);

} // end Stop_Sound

///////////////////////////////////////////////////////////

int Delete_All_Sounds(void)
{
// this function deletes all the sounds

for (int index=0; index < MAX_SOUNDS; index++)
    Delete_Sound(index);

// return success always
return(1);

} // end Delete_All_Sounds

///////////////////////////////////////////////////////////

int Delete_Sound(int id)
{
// this function deletes a single sound and puts it back onto the available list

// first stop it
if (!Stop_Sound(id))
   return(0);

// now delete it
if (sound_fx[id].dsbuffer)
   {
   // release the com object
   sound_fx[id].dsbuffer->Release();
   sound_fx[id].dsbuffer = NULL;
   
   // return success
   return(1);
   } // end if

// return success
return(1);

} // end Delete_Sound

///////////////////////////////////////////////////////////

int Stop_All_Sounds(void)
{
// this function stops all sounds

for (int index=0; index<MAX_SOUNDS; index++)
	Stop_Sound(index);	

// return success
return(1);

} // end Stop_All_Sounds

///////////////////////////////////////////////////////////

int Status_Sound(int id)
{
// this function returns the status of a sound
if (sound_fx[id].dsbuffer)
	{
	ULONG status; 

	// get the status
	sound_fx[id].dsbuffer->GetStatus(&status);

	// return the status
	return(status);

	} // end if
else // total failure
	return(-1);

} // end Status_Sound

void GetInputData(){
	lpdikey->GetDeviceState(256, (LPVOID)keyboard_state);
	lpdimouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouse_state);
}

void MoveMouse(){
	mouse_x+=(mouse_state.lX);
	mouse_y+=(mouse_state.lY);
}

int Engine_Shutdown()
{
	for (int i=0; i<MAX_TEXTURE; i++)
		if(g_pTexture[i]) g_pTexture[i]->Release();

	if (g_pSprite != NULL)
		g_pSprite->Release();

	if( g_pd3dDevice != NULL)
		g_pd3dDevice->Release();
	if( g_pD3D != NULL)
		g_pD3D->Release();

	lpdimouse->Unacquire();
	lpdimouse->Release();
	lpdikey->Unacquire();
	lpdikey->Release();
	lpdi->Release();
	DSound_Shutdown();
	MP3_Shutdown();
	return 1;
}

int BackBuffer_Clear()
{
	// clear secondary buffer
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET,
		D3DCOLOR_ARGB(255, 0, 0, 0), 1.0f, 0 );
	return 1;

}

int Flip()
{
	g_pd3dDevice->Present (NULL,NULL,NULL,NULL);
	return 1;
}
int Draw_Begin()
{
	if(SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		g_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
		return 1;
	}
	return 0;
}

int Draw_End()
{

	g_pSprite->End();
	g_pd3dDevice->EndScene();
	return 1;
}


///////////////////////////////////////////////////////////
int Draw_Text(wchar_t *text, int x, int y, D3DCOLOR color)
{
	int width;
	RECT board;
	width = wcslen(text) * 20;
	board.top = y;
	board.left = x;
	board.bottom = y + 20;
	board.right = x + width;
	g_pFont->DrawText(g_pSprite, text, -1, &board, NULL, color);
	return 0;
}
int Draw_BIG_Text(wchar_t *text, int x, int y, D3DCOLOR color)
{
	int width;
	RECT board;
	width = wcslen(text) * 100;
	board.top = y;
	board.left = x;
	board.bottom = y + 100;
	board.right = x + width;
	g_pFont2->DrawText(g_pSprite, text, -1, &board, NULL, color);
	return 0;
}
int Draw_Small_Text(char *text, int x, int y, D3DCOLOR color)
{
	int width;
	RECT board;
	width = strlen(text) * 15;
	board.top = y;
	board.left = x;
	board.bottom = y + 15;
	board.right = x + width;
	g_pFont4->DrawTextA(g_pSprite,text,-1, &board, NULL, color);
		//DrawText(g_pSprite, text, -1, &board, NULL, color);
	return 0;
}

int Draw_MID_Text(wchar_t *text, int x, int y, D3DCOLOR color)
{
	int width;
	RECT board;
	width = wcslen(text) * 40;
	board.top = y;
	board.left = x;
	board.bottom = y + 40;
	board.right = x + width;
	g_pFont3->DrawText(g_pSprite, text, -1, &board, NULL, color);
	return 0;
}

int Name_BOB16(BITMAP_OBJ_PTR bob, wchar_t *name) 
{
	wcscpy(bob->my_name,name);
	return 0;
}

int Create_BOB16(BITMAP_OBJ_PTR bob,    // the bob to create
	int width, int height, // size of bob
	int attr            // attrs
	)         // memory flag
{
	// set state and attributes of BOB
	bob->state = BOB_STATE_ALIVE;
	bob->attr  = attr;
	bob->image = NULL;
	bob->curr_frame = 0;
	// set position and velocity to 0
	bob->x = bob->y = bob->xv = bob->yv = 0;

	// set dimensions of the new bitmap surface
	bob->width = width;
	bob->height = height;

	// return success
	return(1);

} // end Create_BOB16

int Create_BOB16(BITMAP_OBJ_PTR bob,    // the bob to create
	int width, int height, // size of bob
	int attr,int _x,int _y            // attrs
	)         // memory flag
{
	// set state and attributes of BOB
	bob->state = BOB_STATE_ALIVE;
	bob->attr  = attr;
	bob->image = NULL;
	bob->curr_frame = 0;
	// set position and velocity to 0
	bob->x = bob->y = bob->xv = bob->yv = 0;
	bob->x = _x; bob->y = _y;
	// set dimensions of the new bitmap surface
	bob->width = width;
	bob->height = height;

	// return success
	return(1);

} // end Create_BOB16
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
int Draw_BOB16(BITMAP_OBJ_PTR bob)       // bob to draw
{
// draw a bob at the x,y defined in the BOB
// on the destination surface defined in dest

RECT     source_rect; // the source rectangle                             
D3DXVECTOR3	dest_point;
// fill in the destination rect
dest_point.x = bob->x;
dest_point.y = bob->y;
dest_point.z = 0;


	g_pSprite->Draw(g_pTexture[bob->image],
		&bob->frame[bob->curr_frame], NULL, &dest_point,  
		D3DCOLOR_RGBA(255, 255, 255, 255));


// return success
return(1);
} // end Draw_BOB16

int Draw_BOB16(BITMAP_OBJ_PTR bob,int texture_id)       // bob to draw
{
	// draw a bob at the x,y defined in the BOB
	// on the destination surface defined in dest

	RECT     source_rect; // the source rectangle                             
	D3DXVECTOR3	dest_point;
	// fill in the destination rect
	dest_point.x = bob->x;
	dest_point.y = bob->y;
	dest_point.z = 0;


	// fill in the source rect
	source_rect.left    = bob->my_cx;
	source_rect.top     = bob->my_cy;
	source_rect.right   = bob->width + bob->my_cx;
	source_rect.bottom  = bob->height + bob->my_cy;

	g_pSprite->Draw(g_pTexture[texture_id],&source_rect, NULL, &dest_point,  D3DCOLOR_RGBA(255, 255,255,255));



	// return success
	return(1);
} // end Draw_BOB16


int DrawName_BOB16(BITMAP_OBJ_PTR bob)       // bob to draw
{
	Draw_Text(bob->my_name, bob->x, bob->y - 20, 
		D3DCOLOR_ARGB(255,255,255,255));
	return(1);
} // end Draw_BOB16

///////////////////////////////////////////////////////////
int Load_Texture(wchar_t *filename,int texture_id)
{
	D3DXCreateTextureFromFileEx(g_pd3dDevice, filename, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0,
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 		
		D3DX_FILTER_NONE, D3DX_DEFAULT, 
		D3DCOLOR_ARGB(255, 255,255,255), 
		NULL, NULL, &g_pTexture[texture_id]);

	return 1;

}


int Load_BOB16(BITMAP_OBJ_PTR bob, // bob to load with data
	int image, // bitmap to scan image data from
	int cx,int cy,   // cell or absolute pos. to scan image from
	int mode)        // if 0 then cx,cy is cell position, else 
	// cx,cy are absolute coords
{
	// this function extracts a bitmap out of a bitmap file

	// test the mode of extraction, cell based or absolute
	if (mode==0)
	{
		// re-compute x,y
		bob->my_cx = cx*(bob->width+1) + 1;
		bob->my_cy = cy*(bob->height+1) + 1;
	} // end if


	//D3DXCreateTextureFromFile( g_pd3dDevice, filename, &bob->image );

	// set state to loaded
	bob->state |= BOB_STATE_LOADED;

	return(1);
} // end Load_BOB16

int Load_Frame_BOB16(BITMAP_OBJ_PTR bob, // bob to load with data
             int texture_id, // bitmap to scan image data from
             int frame_id,
			 int cx,int cy,   // cell or absolute pos. to scan image from
             int mode)        // if 0 then cx,cy is cell position, else 
                              // cx,cy are absolute coords
{
// this function extracts a bitmap out of a bitmap file

// test the mode of extraction, cell based or absolute
if (mode==BITMAP_EXTRACT_MODE_CELL)
   {
  bob->my_cx = cx*(bob->width+1) + 1;
bob->my_cy = cy*(bob->height+1) + 1;
  bob->frame[frame_id].left = cx*(bob->width+1) + 1;
bob->frame[frame_id].top = cy*(bob->height+1) + 1;
   } // end if

bob->frame[frame_id].bottom = bob->frame[frame_id].top + bob->height;
bob->frame[frame_id].right = bob->frame[frame_id].left + bob->width;

	bob->image = texture_id;

	if (frame_id > bob->max_frame) bob->max_frame = frame_id;

// set state to loaded
bob->state |= BOB_STATE_LOADED;

return(1);
} // end Load_BOB16

void Set_Anim_Speed_BOB16(BITMAP_OBJ_PTR bob,int speed)
{
	bob->anim_speed = speed;
}
void Set_Vel_BOB16(BITMAP_OBJ_PTR bob, int xv, int yv)
{
	bob->xv = xv;
	bob->yv = yv;
}
void Set_Pos_BOB16(BITMAP_OBJ_PTR bob, int x, int y)
{
	bob->x = x; bob->y = y;
}

void Animate_BOB16(BITMAP_OBJ_PTR bob)
{
	if (bob->attr & BOB_ATTR_SINGLE_FRAME) {
		bob->curr_frame = 0;
		return;
	}
	if (bob->attr & BOB_ATTR_MULTI_FRAME) {
		bob->anim_speed_count++;
		if (bob->anim_speed_count <= bob->anim_speed) return;
		bob->anim_speed_count = 0;
		bob->curr_frame++;
		if (bob->curr_frame > bob->max_frame) bob->curr_frame = 0;
		return;
	}
}
void Animate_BOB16(BITMAP_OBJ_PTR bob,int start_frame,int max_frame)
{
	if (bob->attr & BOB_ATTR_SINGLE_FRAME) {
		bob->curr_frame = 0;
		return;
	}
	if (bob->attr & BOB_ATTR_MULTI_FRAME) {
		if(bob->curr_frame<start_frame)
			bob->curr_frame = start_frame;
		bob->anim_speed_count++;
		if (bob->anim_speed_count <= bob->anim_speed) return;
		bob->anim_speed_count = 0;
		bob->curr_frame++;
		if (bob->curr_frame > max_frame) bob->curr_frame = start_frame;
		return;
	}
}

void Move_Enemy_BOB16(BITMAP_OBJ_PTR bob)
{
	bob->x += bob->xv/2;
	bob->y += bob->yv/2;
}
