#pragma once
// TODO destroy this monstrosity
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <unordered_map>
#include <regex>
#include <set>
#include <string>
#include <vector>
#include <stdexcept>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/range.hpp>


#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#include <assimp/ai_assert.h>
#include <assimp/IOSystem.hpp>
#include <assimp/ProgressHandler.hpp>
#include <assimp/importerdesc.h>
#include <assimp/Exceptional.h>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/SceneCombiner.h>
#include "zlib.h"

#include "../Vendor/half/half.hpp"
#include "Types.h"

#include "DirectX.h"
#include "Util/DXUtil.h"
#include "Util/BinaryReader.h"
#include "Util/BinaryWriter.h"
#include "Util/AssimpUtil.h"

#include "Material.h"
#include "MaterialCollection.h"
#include "MaterialLibrary.h"

#include "Xmf/XmfHeader.h"
#include "Xmf/XmfVertexElement.h"
#include "Xmf/XmfMaterial.h"
#include "Xmf/XmfDataBufferDesc.h"
#include "Xmf/XmfDataBuffer.h"
#include "Xmf/XuMeshFile.h"
#include "ComponentPartLod.h"
#include "ComponentPart.h"
#include "Component.h"

#include "Xmf/XmfImporter.h"
#include "Xmf/XmfExporter.h"

//#include "EmotionFX.h"
//#include "Xac/XacHeader.h"
//#include "Xac/XacChunkHeader.h"
//#include "Xac/XacMetaDataChunkv2Header.h"
//#include "Xac/XacNodeHierarchyChunkv1NodeHeader.h"
//#include "Xac/XacMaterialDefinitionChunkv2Header.h"
//#include "Xac/XacMaterialDefinitionChunkv2Layer.h"
//#include "Xac/XacMaterialTotalsChunkv1.h"
//#include "Xac/XacMeshChunkv1Header.h"
//#include "Xac/XacMeshChunkv1VertexElement.h"
//#include "Xac/XacMeshChunkv1SubMesh.h"
//#include "Xac/XacSkinningChunkv3Header.h"
//#include "Xac/XacSkinningChunkv3Influence.h"
//#include "Xac/XacSkinningChunkv3InfluenceRange.h"
//#include "Xac/XacMorphTargetsChunkv1MorphTarget.h"
//#include "Xac/XacMorphTargetsChunkv1Deformation.h"
//#include "Xac/XacDeformVec3D16.h"
//#include "Xac/XacDeformVec3D8.h"
//#include "Xac/XacMorphTargetsChunkv1Transformation.h"
//#include "Xac/BoneInfluence.h"
//#include "Xac/ActorSubMesh.h"
//#include "Xac/ActorMesh.h"
//#include "Xac/ActorMaterialLayer.h"
//#include "Xac/ActorMaterial.h"
//#include "Xac/ActorNodeDeformation.h"
//#include "Xac/ActorMorphTarget.h"
//#include "Xac/ActorNode.h"
//#include "Xac/ActorFile.h"
//
//#include "Xac/XacImporter.h"
//#include "Xac/XacExporter.h"

#include "API.h"
