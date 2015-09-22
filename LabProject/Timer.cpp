//-----------------------------------------------------------------------------
// File: CGameTimer.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Timer.h"

CGameTimer::CGameTimer()
{
	if (QueryPerformanceFrequency((LARGE_INTEGER *)&m_PerformanceFrequency)) 
    { 
		m_bHardwareHasPerformanceCounter = TRUE;
		QueryPerformanceCounter((LARGE_INTEGER *)&m_nLastTime); 
		m_fTimeScale = 1.0f / m_PerformanceFrequency;
	} 
    else 
    { 
		m_bHardwareHasPerformanceCounter = FALSE;
		m_nLastTime	= ::timeGetTime(); 
		m_fTimeScale = 0.001f;	
    }

    m_nSampleCount = 0;
	m_nCurrentFrameRate = 0;
	m_FramePerSecond = 0;
	m_fFPSTimeElapsed = 0.0f;
}

CGameTimer::~CGameTimer()
{
}

void CGameTimer::Tick(float fLockFPS)
{
    float fTimeElapsed; 

	if (m_bHardwareHasPerformanceCounter) 
    {
		QueryPerformanceCounter((LARGE_INTEGER *)&m_nCurrentTime);
	} 
    else 
    {
		m_nCurrentTime = ::timeGetTime();
	} 

	fTimeElapsed = (m_nCurrentTime - m_nLastTime) * m_fTimeScale;

    if (fLockFPS > 0.0f)
    {
        while (fTimeElapsed < (1.0f / fLockFPS))
        {
	        if (m_bHardwareHasPerformanceCounter) 
            {
		        QueryPerformanceCounter((LARGE_INTEGER *)&m_nCurrentTime);
	        } 
            else 
            {
				m_nCurrentTime = ::timeGetTime();
	        } 
	        // Calculate elapsed time in seconds
	        fTimeElapsed = (m_nCurrentTime - m_nLastTime) * m_fTimeScale;
        }
    } 

	m_nLastTime = m_nCurrentTime;

    if (fabsf(fTimeElapsed - m_fTimeElapsed) < 1.0f)
    {
        // Wrap FIFO frame time buffer.
        memmove(&m_fFrameTime[1], m_fFrameTime, (MAX_SAMPLE_COUNT - 1) * sizeof(float));
        m_fFrameTime[0] = fTimeElapsed;
        if (m_nSampleCount < MAX_SAMPLE_COUNT) m_nSampleCount++;
    }

	// Calculate Frame Rate
	m_FramePerSecond++;
	m_fFPSTimeElapsed += fTimeElapsed;
	if (m_fFPSTimeElapsed > 1.0f) 
    {
		m_nCurrentFrameRate	= m_FramePerSecond;
		m_FramePerSecond = 0;
		m_fFPSTimeElapsed = 0.0f;
	} 

    // Count up the new average elapsed time
    m_fTimeElapsed = 0.0f;
    for (ULONG i = 0; i < m_nSampleCount; i++) m_fTimeElapsed += m_fFrameTime[i];
    if (m_nSampleCount > 0) m_fTimeElapsed /= m_nSampleCount;
}

unsigned long CGameTimer::GetFrameRate(LPTSTR lpszString, int nCharacters) const
{
    if (lpszString)
    {
        _itow_s(m_nCurrentFrameRate, lpszString, nCharacters, 10);
        wcscat_s(lpszString, nCharacters, _T(" FPS)"));
    } 

    return(m_nCurrentFrameRate);
}

float CGameTimer::GetTimeElapsed() const
{
    return(m_fTimeElapsed);
}
