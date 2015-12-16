#pragma once
#include "Animation.h"

//アニメーションをプレイ、現在アニメーション時間を管理するクラスです。
class CAnimationController
{
public:
	CAnimationController();
	~CAnimationController(){}

	/////////////// Controller Part ///////////////

	void UpdateTime(float fElapsedTime);

	void Play(string _AnimationName);
	void SetAnimationData(string _AnimationName, float _fCurrentAnimLength);
	void Stop();
	void Pause();

	GET_SET_FUNC_IMPL(float, CurrentAnimationTime, m_fCurrentAnimTime);
	GET_SET_FUNC_IMPL(string, CurrentAnimationName, m_CurrentPlayingAnimationName);

private:
	string						m_CurrentPlayingAnimationName;
	float						m_fCurrentAnimTime;
	float						m_fCurrentAnimLength;
	bool						m_bIsPlaying;

	vector<pair<string, float>>	m_vAnimationTimeData;
};