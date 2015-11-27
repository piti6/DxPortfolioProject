#pragma once

D3DXMATRIX* D3DXMatrixReflect_Fixed(__inout D3DXMATRIX *pOut, __in const D3DXPLANE *pPlane);
FbxAMatrix* D3DXMatrixReflect_Fixed(__inout FbxAMatrix *pOut, __in const D3DXPLANE *pPlane);
D3DXMATRIX GetD3DMatrix( FbxAMatrix SrcFbxMtx );
FbxAMatrix& GetFbxMatrix( D3DXMATRIX & SrcFbxMtx );
FbxAMatrix GetGeometry(FbxNode* pNode);
FbxAMatrix GetPoseMatrix(FbxPose* pPose, int pNodeIndex);