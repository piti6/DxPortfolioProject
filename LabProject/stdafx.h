// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

//#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C의 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <d3d11.h>
#include <d3dx11.h>
#include <D3DX10Math.h>
#include <d3dcompiler.h>
#include <xnamath.h>

#include <D3D9Types.h>

#include <Mmsystem.h>

#include <vector>
#include <fbxsdk.h>
#include <PxPhysicsAPI.h>
#include <iostream>



#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console" )
/*
#pragma comment(lib, "PhysX3_x86.lib")
#pragma comment(lib, "PhysX3Common_x86.lib")
#pragma comment(lib, "PhysX3Extensions.lib")
*/
using namespace physx;
using namespace std;

#define GET_SET_FUNC_IMPL(TYPE, FUNC_NAME, PROP) \
	TYPE Get##FUNC_NAME() \
	{ \
		return PROP; \
	} \
	void Set##FUNC_NAME(TYPE _PROP) \
	{ \
		PROP = _PROP; \
	}
// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#define PS_SLOT_LIGHT		0x00
#define PS_SLOT_MATERIAL	0x01

#define PS_SLOT_TEXTURE		0x00
#define PS_SLOT_SAMPLER_STATE		0x00


#define VS_SLOT_VIEWPROJECTION_MATRIX	0x00
#define VS_SLOT_WORLD_MATRIX			0x01

#define MAX_BONE 60
