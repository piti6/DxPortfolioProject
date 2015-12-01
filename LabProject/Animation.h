#pragma once


class CBoneContainer			// Bone Container
{
public:
	CBoneContainer(){}
	~CBoneContainer(){}

	vector<D3DXMATRIX>			m_vBoneList;
};

class CAnimationDataContainer					
{
public:
	CAnimationDataContainer(){}
	~CAnimationDataContainer(){}

	vector<CBoneContainer> m_vBoneContainer;
};

class CAnimation
{
public:
	CAnimation(bool _isLooping=false){m_bIsLooping = _isLooping;}
	~CAnimation(){}

	CAnimationDataContainer		m_vAnimation;
	bool						m_bIsLooping;
	float						m_fLength;
};

class CAnimationList
{
public:
	CAnimationList(){}
	~CAnimationList(){}

	unordered_map<string,CAnimation>				  m_Animation;
};


