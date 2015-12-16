#include "stdafx.h"
#include "AnimationController.h"
#include "FbxToDxTranslation.h"

//デバッグするため、行列をプリントします。
void PrintFbxMatrix(FbxAMatrix a){
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			cout << "Data [" << i << "][" << j << "] : " << a.mData[i][j] << endl;
		}
		cout << endl;
	}
}

CAnimationController::CAnimationController()
{
	m_fCurrentAnimTime = 0;
}

//現在アニメーションの時間を更新します。
void CAnimationController::UpdateTime(float fElapsedTime)
{
	if (m_bIsPlaying){
		m_fCurrentAnimTime += fElapsedTime;
		if (m_fCurrentAnimLength < m_fCurrentAnimTime)
			m_fCurrentAnimTime = 0;
	}
}
void CAnimationController::SetAnimationData(string _AnimationName, float _fAnimLength)
{
	m_vAnimationTimeData.push_back(make_pair(_AnimationName, _fAnimLength));
}

//アニメーションコントロール
void CAnimationController::Play(string _AnimationName)
{
	for (int i = 0; i < m_vAnimationTimeData.size(); ++i)
	{
		if (m_vAnimationTimeData[i].first == _AnimationName)
		{
			m_CurrentPlayingAnimationName = _AnimationName;
			m_fCurrentAnimLength = m_vAnimationTimeData[i].second;
			m_fCurrentAnimTime = 0;
			m_bIsPlaying = true;
			break;
		}
	}
}
void CAnimationController::Stop()
{
	m_bIsPlaying = false;
	m_fCurrentAnimTime = 0;
}
void CAnimationController::Pause()
{
	m_bIsPlaying = false;
}






