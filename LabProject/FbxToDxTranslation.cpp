#include "stdafx.h"
#include "FbxToDxTranslation.h"

//反射平面を作ります。
FbxAMatrix* D3DXMatrixReflect_Fixed(__inout FbxAMatrix *pOut, __in const D3DXPLANE *pPlane)
{
	D3DXPLANE P;
	D3DXPlaneNormalize(&P, pPlane);

	pOut->mData[0][0] = -2.0f * P.a * P.a + 1.0f;
	pOut->mData[0][1] = -2.0f * P.b * P.a;
	pOut->mData[0][2] = -2.0f * P.c * P.a;
	pOut->mData[0][3] = 0.0f;
	pOut->mData[1][0] = -2.0f * P.a * P.b;
	pOut->mData[1][1] = -2.0f * P.b * P.b + 1.0f;
	pOut->mData[1][2] = -2.0f * P.c * P.b;
	pOut->mData[1][3] = 0.0f;
	pOut->mData[2][0] = -2.0f * P.a * P.c;
	pOut->mData[2][1] = -2.0f * P.b * P.c;
	pOut->mData[2][2] = -2.0f * P.c * P.c + 1.0f;
	pOut->mData[2][3] = 0.0f;
	pOut->mData[3][0] = -2.0f * P.a * P.d;
	pOut->mData[3][1] = -2.0f * P.b * P.d;
	pOut->mData[3][2] = -2.0f * P.c * P.d;
	pOut->mData[3][3] = 1.0f;

	return pOut;
}

//ジオメトリ情報を持って来ます。
FbxAMatrix GetGeometry(FbxNode* pNode)
{
	const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(lT, lR, lS);
}

//FBX SDKの行列からDirectX行列を作ります。
D3DXMATRIX GetD3DMatrix(FbxAMatrix SrcFbxMtx)
{
	D3DXMATRIX Dst;
	Dst._11 = (float)SrcFbxMtx.mData[0][0];
	Dst._12 = (float)SrcFbxMtx.mData[0][1];
	Dst._13 = (float)SrcFbxMtx.mData[0][2];
	Dst._14 = (float)SrcFbxMtx.mData[0][3];
	Dst._21 = (float)SrcFbxMtx.mData[1][0];
	Dst._22 = (float)SrcFbxMtx.mData[1][1];
	Dst._23 = (float)SrcFbxMtx.mData[1][2];
	Dst._24 = (float)SrcFbxMtx.mData[1][3];
	Dst._31 = (float)SrcFbxMtx.mData[2][0];
	Dst._32 = (float)SrcFbxMtx.mData[2][1];
	Dst._33 = (float)SrcFbxMtx.mData[2][2];
	Dst._34 = (float)SrcFbxMtx.mData[2][3];
	Dst._41 = (float)SrcFbxMtx.mData[3][0];
	Dst._42 = (float)SrcFbxMtx.mData[3][1];
	Dst._43 = (float)SrcFbxMtx.mData[3][2];
	Dst._44 = (float)SrcFbxMtx.mData[3][3];
	return Dst;
}

