#include "StdInc.h"

void AssimpUtil::MergeVertices ( aiMesh* pMesh )
{
    std::unordered_map < VertexInfo, int > firstVertexIdxByPos;
    std::unordered_map < int, int > indexToFirstIndex;
    int targetIdx = 0;
    for ( int srcIdx = 0; srcIdx < pMesh->mNumVertices; ++srcIdx )
    {
        VertexInfo vertexInfo;
        vertexInfo.Position = pMesh->mVertices[srcIdx];
        if ( pMesh->mNormals )
            vertexInfo.Normal = pMesh->mNormals[srcIdx];

        for ( int i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++i )
        {
            if ( pMesh->mTextureCoords[i] )
                vertexInfo.UV[i] = pMesh->mTextureCoords[i][srcIdx];
        }

        auto it = firstVertexIdxByPos.find ( vertexInfo );
        if ( it == firstVertexIdxByPos.end () )
        {
            firstVertexIdxByPos[vertexInfo] = targetIdx;            
            if ( targetIdx < srcIdx )
            {
                indexToFirstIndex[srcIdx] = targetIdx;

                pMesh->mVertices[targetIdx] = pMesh->mVertices[srcIdx];
                if ( pMesh->mNormals )
                    pMesh->mNormals[targetIdx] = pMesh->mNormals[srcIdx];

                if ( pMesh->mTangents )
                    pMesh->mTangents[targetIdx] = pMesh->mTangents[srcIdx];

                if ( pMesh->mBitangents )
                    pMesh->mBitangents[targetIdx] = pMesh->mBitangents[srcIdx];

                for ( int i = 0; i < AI_MAX_NUMBER_OF_COLOR_SETS; ++i )
                {
                    if ( pMesh->mColors[i] )
                        pMesh->mColors[i][targetIdx] = pMesh->mColors[i][srcIdx];
                }

                for ( int i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++i )
                {
                    if ( pMesh->mTextureCoords[i] )
                        pMesh->mTextureCoords[i][targetIdx] = pMesh->mTextureCoords[i][srcIdx];
                }
            }
            targetIdx++;
        }
        else
        {
            indexToFirstIndex[srcIdx] = it->second;
        }
    }
    pMesh->mNumVertices = targetIdx;

    for ( int faceIdx = 0; faceIdx < pMesh->mNumFaces; ++faceIdx )
    {
        aiFace* pFace = &pMesh->mFaces[faceIdx];
        for ( int i = 0; i < pFace->mNumIndices; ++i )
        {
            auto it = indexToFirstIndex.find ( pFace->mIndices[i] );
            if ( it != indexToFirstIndex.end () )
                pFace->mIndices[i] = it->second;
        }
    }

    for ( int boneIdx = 0; boneIdx < pMesh->mNumBones; ++boneIdx )
    {
        aiBone* pBone = pMesh->mBones[boneIdx];
        for ( int i = 0; i < pBone->mNumWeights; ++i )
        {
            auto it = indexToFirstIndex.find ( pBone->mWeights[i].mVertexId );
            if ( it != indexToFirstIndex.end () )
                pBone->mWeights[i].mVertexId = it->second;
        }
    }
}

bool AssimpUtil::VertexInfo::operator== ( const VertexInfo& other ) const
{
    if ( Position != other.Position || Normal != other.Normal )
        return false;

    for ( int i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++i )
    {
        if ( UV[i] != other.UV[i] )
            return false;
    }
    return true;
}
