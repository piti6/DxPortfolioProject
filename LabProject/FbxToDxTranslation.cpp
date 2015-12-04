#include "stdafx.h"
#include "FbxToDxTranslation.h"

D3DXMATRIX* D3DXMatrixReflect_Fixed(__inout D3DXMATRIX *pOut, __in const D3DXPLANE *pPlane) 
{ 
        D3DXPLANE P; 
        D3DXPlaneNormalize(&P,pPlane); 
 
        *pOut = D3DXMATRIX 
        ( 
        -2.0f * P.a * P.a + 1.0f,  -2.0f * P.b * P.a        , -2.0f * P.c * P.a                 ,  0.0f , 
        -2.0f * P.a * P.b       ,  -2.0f * P.b * P.b + 1.0f , -2.0f * P.c * P.b                 ,  0.0f , 
        -2.0f * P.a * P.c       ,  -2.0f * P.b * P.c        , -2.0f * P.c * P.c + 1.0f			,  0.0f , 
        -2.0f * P.a * P.d       ,  -2.0f * P.b * P.d        , -2.0f * P.c * P.d                 ,  1.0f  
        ); 
 
        return pOut; 
}
FbxAMatrix* D3DXMatrixReflect_Fixed(__inout FbxAMatrix *pOut, __in const D3DXPLANE *pPlane) 
{ 
        D3DXPLANE P; 
        D3DXPlaneNormalize(&P,pPlane); 
 
		pOut->mData[0][0] = -2.0f * P.a * P.a + 1.0f;
		pOut->mData[0][1] = -2.0f * P.b * P.a;
		pOut->mData[0][2] =  -2.0f * P.c * P.a ;
		pOut->mData[0][3] =  0.0f;
		pOut->mData[1][0] = -2.0f * P.a * P.b ;
		pOut->mData[1][1] = -2.0f * P.b * P.b + 1.0f;
		pOut->mData[1][2] = -2.0f * P.c * P.b;
		pOut->mData[1][3] = 0.0f;
		pOut->mData[2][0] = -2.0f * P.a * P.c;
		pOut->mData[2][1] = -2.0f * P.b * P.c;
		pOut->mData[2][2] = -2.0f * P.c * P.c + 1.0f;
		pOut->mData[2][3] =  0.0f;
		pOut->mData[3][0] = -2.0f * P.a * P.d;
        pOut->mData[3][1] = -2.0f * P.b * P.d;
		pOut->mData[3][2] = -2.0f * P.c * P.d;
		pOut->mData[3][3] = 1.0f;

 
        return pOut; 
}




FbxAMatrix GetGeometry(FbxNode* pNode)  
{
    const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
    const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	//const FbxVector4 lS = FbxVector4(1, 1, 1);
	const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

    return FbxAMatrix(lT, lR, lS);
}

D3DXMATRIX GetD3DMatrix( FbxAMatrix SrcFbxMtx )
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

FbxAMatrix& GetFbxMatrix( D3DXMATRIX & SrcFbxMtx )
{
	FbxAMatrix Dst;
	Dst.mData[0][0] = (float)SrcFbxMtx._11;
	Dst.mData[0][1] = (float)SrcFbxMtx._12;
	Dst.mData[0][2] = (float)SrcFbxMtx._13;
	Dst.mData[0][3] = (float)SrcFbxMtx._14;
	Dst.mData[1][0] = (float)SrcFbxMtx._21;
	Dst.mData[1][1] = (float)SrcFbxMtx._22;
	Dst.mData[1][2] = (float)SrcFbxMtx._23;
	Dst.mData[1][3] = (float)SrcFbxMtx._24;
	Dst.mData[2][0] = (float)SrcFbxMtx._31;
	Dst.mData[2][1] = (float)SrcFbxMtx._32;
	Dst.mData[2][2] = (float)SrcFbxMtx._33;
	Dst.mData[2][3] = (float)SrcFbxMtx._34;
	Dst.mData[3][0] = (float)SrcFbxMtx._41;
	Dst.mData[3][1] = (float)SrcFbxMtx._42;
	Dst.mData[3][2] = (float)SrcFbxMtx._43;
	Dst.mData[3][3] = (float)SrcFbxMtx._44;
	return Dst;
}