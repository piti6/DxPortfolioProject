#pragma once

/////////////////////////////////////////////////////////////////////////////////////////////////
//時間の経過とともに変化する一つのボーンの行列ベクトルです。

class CBoneContainer
{
public:
	CBoneContainer(){}
	~CBoneContainer(){}

	vector<D3DXMATRIX>* GetBoneList(){ return &m_vBoneList; }

private:
	vector<D3DXMATRIX>			m_vBoneList;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//複数のボーンのベクトルです。アニメーションそのものです。

class CAnimationDataContainer
{
public:
	CAnimationDataContainer(){}
	~CAnimationDataContainer(){}

	vector<CBoneContainer>* GetBoneContainerVector(){ return &m_vBoneContainer; }

private:
	vector<CBoneContainer> m_vBoneContainer;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//アニメーションを管理するクラスです。

class CAnimation
{
public:
	CAnimation(bool _isLooping = false){ m_bIsLooping = _isLooping; }
	~CAnimation(){}

	CAnimationDataContainer* GetAnimationData(){ return &m_vAnimation; }
	void SetAnimationData(CAnimationDataContainer pAnimationData){ m_vAnimation = pAnimationData; }

	GET_SET_FUNC_IMPL(bool, Loop, m_bIsLooping);
	GET_SET_FUNC_IMPL(float, Length, m_fLength);

private:
	CAnimationDataContainer		m_vAnimation;
	bool						m_bIsLooping;
	float						m_fLength;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//複数のアニメーションを管理するクラスです。

class CAnimationList
{
public:
	CAnimationList(){}
	~CAnimationList(){}

	unordered_map<string, CAnimation>* GetAnimationMap(){ return &m_Animation; }

private:
	unordered_map<string, CAnimation>				  m_Animation;
};


