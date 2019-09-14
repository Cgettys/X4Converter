#include <X4ConverterTools/util/AssimpUtil.h>

using boost::numeric_cast;

void AssimpUtil::MergeVertices(aiMesh *pMesh) {
  std::unordered_map<VertexInfo, int> firstVertexIdxByPos;
  std::unordered_map<int, int> indexToFirstIndex;
  int targetIdx = 0;
  for (int srcIdx = 0; srcIdx < pMesh->mNumVertices; ++srcIdx) {
    VertexInfo vertexInfo;
    vertexInfo.Position = pMesh->mVertices[srcIdx];
    if (pMesh->mNormals)
      vertexInfo.Normal = pMesh->mNormals[srcIdx];

    for (int i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++i) {
      if (pMesh->mTextureCoords[i])
        vertexInfo.UV[i] = pMesh->mTextureCoords[i][srcIdx];
    }

    auto it = firstVertexIdxByPos.find(vertexInfo);
    if (it == firstVertexIdxByPos.end()) {
      firstVertexIdxByPos[vertexInfo] = targetIdx;
      if (targetIdx < srcIdx) {
        indexToFirstIndex[srcIdx] = targetIdx;

        pMesh->mVertices[targetIdx] = pMesh->mVertices[srcIdx];
        if (pMesh->mNormals)
          pMesh->mNormals[targetIdx] = pMesh->mNormals[srcIdx];

        if (pMesh->mTangents)
          pMesh->mTangents[targetIdx] = pMesh->mTangents[srcIdx];

        if (pMesh->mBitangents)
          pMesh->mBitangents[targetIdx] = pMesh->mBitangents[srcIdx];

        for (auto &mColor : pMesh->mColors) {
          if (mColor) {
            mColor[targetIdx] = mColor[srcIdx];
          }
        }

        for (auto &mTextureCoord : pMesh->mTextureCoords) {
          if (mTextureCoord)
            mTextureCoord[targetIdx] = mTextureCoord[srcIdx];
        }
      }
      targetIdx++;
    } else {
      indexToFirstIndex[srcIdx] = it->second;
    }
  }
  pMesh->mNumVertices = numeric_cast<uint32_t>(targetIdx);

  for (int faceIdx = 0; faceIdx < pMesh->mNumFaces; ++faceIdx) {
    aiFace *pFace = &pMesh->mFaces[faceIdx];
    for (int i = 0; i < pFace->mNumIndices; ++i) {
      auto it = indexToFirstIndex.find(pFace->mIndices[i]);
      if (it != indexToFirstIndex.end())
        pFace->mIndices[i] = numeric_cast<uint32_t>(it->second);
    }
  }
  // TODO can we get rid of this function or not?
}

bool AssimpUtil::VertexInfo::operator==(const VertexInfo &other) const {
  if (Position != other.Position || Normal != other.Normal)
    return false;

  for (int i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++i) {
    if (UV[i] != other.UV[i])
      return false;
  }
  return true;
}
