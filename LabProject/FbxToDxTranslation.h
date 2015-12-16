#pragma once

//FBX SDKの行列をDirectXの行列で変化させる関数です。
FbxAMatrix* D3DXMatrixReflect_Fixed(__inout FbxAMatrix *pOut, __in const D3DXPLANE *pPlane);
D3DXMATRIX GetD3DMatrix(FbxAMatrix SrcFbxMtx);
FbxAMatrix GetGeometry(FbxNode* pNode);
