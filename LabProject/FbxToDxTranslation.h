#pragma once

D3DXMATRIX* D3DXMatrixReflect_Fixed(__inout D3DXMATRIX *pOut, __in const D3DXPLANE *pPlane);
FbxAMatrix* D3DXMatrixReflect_Fixed(__inout FbxAMatrix *pOut, __in const D3DXPLANE *pPlane);
D3DXMATRIX GetD3DMatrix( FbxAMatrix SrcFbxMtx );
FbxAMatrix& GetFbxMatrix( D3DXMATRIX & SrcFbxMtx );
FbxAMatrix GetGeometry(FbxNode* pNode);
FbxAMatrix GetPoseMatrix(FbxPose* pPose, int pNodeIndex);


// Get the global position of the node for the current pose.
// If the specified node is not part of the pose or no pose is specified, get its global position at the current time.
FbxAMatrix GetGlobalPosition(FbxNode* pNode, const FbxTime& pTime, FbxPose* pPose=NULL, FbxAMatrix* pParentGlobalPosition=NULL);

D3DXMATRIX GetD3DMatrixFromPxMat(PxMat44* pPxMat);