#include "StdInc.h"

using namespace boost;
using namespace boost::algorithm;
using namespace boost::filesystem;
using namespace Assimp;

Assimp::Exporter::ExportFormatEntry XacExporter::Format ( "xac", "EMotionFX actor exporter", ".xac", XacExporter::Export );
std::string XacExporter::GameBaseFolderPath;

void XacExporter::Export ( const char* pFilePath, IOSystem* pIOHandler, const aiScene* pScene )
{
    IOStream* pStream = nullptr;
    try
    {
        if ( GameBaseFolderPath.empty () )
            throw std::string ( "GameBaseFolderPath not set" );

        if ( !pScene->mRootNode || pScene->mRootNode->mNumChildren != 1 )
            throw std::string ( "Scene must have exactly one root node (make sure to remove any lights and cameras)" );

        for ( int i = 0; i < pScene->mNumMeshes; ++i )
        {
            AssimpUtil::MergeVertices ( pScene->mMeshes[i] );
        }

        aiNode* pFileNode = pScene->mRootNode->mChildren[0];

        ActorFile actor;
        ConversionContext context;
        context.Scene = pScene;
        context.Actor = &actor;
        for ( int i = 0; i < pFileNode->mNumChildren; ++i )
        {
            std::shared_ptr < ActorNode > pActorNode = ConvertNode ( pFileNode->mChildren[i], context );
            if ( pActorNode )
                actor.RootNodes.push_back ( pActorNode );
        }
        AssignMeshesToNodes ( context );
        ConvertBoneInfluences ( context );
        ConvertMorphTargets ( context );

        actor.Materials.resize ( context.Materials.size () );
        for ( auto it = context.Materials.begin (); it != context.Materials.end (); ++it )
        {
            actor.Materials[it->second].Name = it->first;
        }

        pStream = pIOHandler->Open ( pFilePath, "wb+" );
        if ( !pStream )
            throw (format("Failed to open %s for writing") % pFilePath).str ();

        actor.Write ( pStream );
        pIOHandler->Close ( pStream );
    }
    catch ( std::string error )
    {
        if ( pStream )
            pIOHandler->Close ( pStream );

        throw DeadlyExportError ( error );
    }
}

std::shared_ptr < ActorNode > XacExporter::ConvertNode ( aiNode* pAiNode, ConversionContext& context )
{
    std::cmatch match;
    if ( std::regex_match ( pAiNode->mName.C_Str (), match, std::regex("(\\w+?)X(visual|collision)X\\d+") ) )
    {
        // Mesh node
        if ( pAiNode->mNumMeshes != 1 )
            throw ( format("Node %s must have exactly one mesh assigned") % pAiNode->mName.C_Str() ).str ();

        std::string nodeName = match[1].str ();
        std::map < std::string, std::shared_ptr<ActorMesh> >& meshMap = (match[2].str () == "visual" ? context.VisualMeshes : context.CollisionMeshes);
        std::shared_ptr < ActorMesh > pActorMesh;
        auto meshIt = meshMap.find ( nodeName );
        if ( meshIt != meshMap.end () )
        {
            pActorMesh = meshIt->second;
        }
        else
        {
            pActorMesh = std::make_shared < ActorMesh > ();
            meshMap [ nodeName ] = pActorMesh;
        }

        aiMesh* pAiMesh = context.Scene->mMeshes[pAiNode->mMeshes[0]];
        context.ConvertedSubMeshes[pAiMesh] = SubMeshEntry ( pActorMesh.get (), pActorMesh->SubMeshes.size () );
        pActorMesh->SubMeshes.push_back ( ConvertSubMesh ( pAiMesh, context ) );
        return std::shared_ptr < ActorNode > ();
    }
    else
    {
        // Regular node/bone
        std::shared_ptr < ActorNode > pActorNode = std::make_shared < ActorNode > ();
        pActorNode->Name = pAiNode->mName.C_Str ();
        
        aiVector3D position;
        aiQuaternion rotation;
        aiVector3D scale;
        pAiNode->mTransformation.Decompose ( scale, rotation, position );
        pActorNode->Position = position;
        pActorNode->Rotation = rotation;
        pActorNode->Scale = scale;

        for ( int i = 0; i < pAiNode->mNumChildren; ++i )
        {
            std::shared_ptr < ActorNode > pChildActorNode = ConvertNode ( pAiNode->mChildren[i], context );
            if ( !pChildActorNode )
                continue;

            pChildActorNode->Parent = pActorNode.get ();
            pActorNode->Children.push_back ( pChildActorNode );
        }
        return pActorNode;
    }
}

ActorSubMesh XacExporter::ConvertSubMesh ( aiMesh* pAiMesh, ConversionContext& context )
{
    if ( pAiMesh->mPrimitiveTypes & ~aiPrimitiveType_TRIANGLE )
        throw (format("Mesh %s contains nontriangular polygons") % pAiMesh->mName.C_Str()).str ();

    ActorSubMesh subMesh;

    subMesh.VertexPositions.reserve ( pAiMesh->mNumVertices );
    for ( int i = 0; i < pAiMesh->mNumVertices; ++i )
    {
        subMesh.VertexPositions.push_back ( pAiMesh->mVertices[i] );
    }

    if ( pAiMesh->mNormals )
    {
        subMesh.VertexNormals.reserve ( pAiMesh->mNumVertices );
        for ( int i = 0; i < pAiMesh->mNumVertices; ++i )
        {
            subMesh.VertexNormals.push_back ( pAiMesh->mNormals[i] );
        }
    }

    if ( pAiMesh->mTangents )
    {
        subMesh.VertexTangents.reserve ( pAiMesh->mNumVertices );
        for ( int i = 0; i < pAiMesh->mNumVertices; ++i )
        {
            subMesh.VertexTangents.push_back ( pAiMesh->mTangents[i] );
        }
    }

    if ( pAiMesh->mBitangents )
    {
        subMesh.VertexBitangents.reserve ( pAiMesh->mNumVertices );
        for ( int i = 0; i < pAiMesh->mNumVertices; ++i )
        {
            subMesh.VertexBitangents.push_back ( pAiMesh->mBitangents[i] );
        }
    }

    for ( int i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++i )
    {
        if ( !pAiMesh->mTextureCoords[i] )
            continue;

        subMesh.VertexUVs.resize ( subMesh.VertexUVs.size () + 1 );
        subMesh.VertexUVs.back ().reserve ( pAiMesh->mNumVertices );
        for ( int j = 0; j < pAiMesh->mNumVertices; ++j )
        {
            aiVector3D& uv = pAiMesh->mTextureCoords[i][j];
            subMesh.VertexUVs.back ().push_back ( Vec2D ( uv.x, 1.0f - uv.y ) );
        }
    }

    for ( int i = 0; i < AI_MAX_NUMBER_OF_COLOR_SETS; ++i )
    {
        if ( !pAiMesh->mColors[i] )
            continue;

        subMesh.VertexColors32.resize ( subMesh.VertexColors32.size () + 1 );
        subMesh.VertexColors32.back ().reserve ( pAiMesh->mNumVertices );
        for ( int j = 0; j < pAiMesh->mNumVertices; ++j )
        {
            subMesh.VertexColors32.back ().push_back ( pAiMesh->mColors[i][j] );
        }
    }

    subMesh.Indices.reserve ( pAiMesh->mNumFaces * 3 );
    for ( int i = 0; i < pAiMesh->mNumFaces; ++i )
    {
        for ( int j = 0; j < 3; ++j )
        {
            subMesh.Indices.push_back ( pAiMesh->mFaces[i].mIndices[j] );
        }
    }

    aiMaterial* pMaterial = context.Scene->mMaterials[pAiMesh->mMaterialIndex];
    aiString aiMaterialName;
    pMaterial->Get ( AI_MATKEY_NAME, aiMaterialName );
    std::string materialName = replace_all_copy ( std::string(aiMaterialName.C_Str()), "X", "." );
    auto itMaterial = context.Materials.find ( materialName );
    if ( itMaterial != context.Materials.end () )
    {
        subMesh.MaterialId = itMaterial->second;
    }
    else
    {
        subMesh.MaterialId = context.Materials.size ();
        context.Materials[materialName] = subMesh.MaterialId;
    }

    return subMesh;
}

void XacExporter::AssignMeshesToNodes ( ConversionContext& context )
{
    for ( auto it = context.VisualMeshes.begin (); it != context.VisualMeshes.end (); ++it )
    {
        ActorNode* pActorNode = context.Actor->GetNode ( it->first );
        if ( !pActorNode )
            throw (format("Cannot find node %s referenced by visual mesh") % it->first).str ();

        pActorNode->VisualMesh = it->second;
    }

    for ( auto it = context.CollisionMeshes.begin (); it != context.CollisionMeshes.end (); ++it )
    {
        ActorNode* pActorNode = context.Actor->GetNode ( it->first );
        if ( !pActorNode )
            throw (format("Cannot find node %s referenced by collision mesh") % it->first).str ();

        pActorNode->CollisionMesh = it->second;
    }
}

void XacExporter::ConvertBoneInfluences ( ConversionContext& context )
{
    for ( auto it = context.ConvertedSubMeshes.begin (); it != context.ConvertedSubMeshes.end (); ++it )
    {
        aiMesh* pAiMesh = it->first;
        ActorSubMesh* pActorSubMesh = &it->second.pActorMesh->SubMeshes[it->second.SubMeshIndex];
        pActorSubMesh->VertexInfluences.resize ( pAiMesh->mNumVertices );
        for ( int i = 0; i < pAiMesh->mNumBones; ++i )
        {
            aiBone* pBone = pAiMesh->mBones[i];
            ActorNode* pBoneNode = context.Actor->GetNode ( pBone->mName.C_Str () );
            if ( !pBoneNode )
                throw (format("Cannot find node %s referenced by bone") % pBone->mName.C_Str()).str ();

            for ( int j = 0; j < pBone->mNumWeights; ++j )
            {
                aiVertexWeight& influence = pBone->mWeights[j];
                pActorSubMesh->VertexInfluences[influence.mVertexId].push_back ( BoneInfluence ( pBoneNode, influence.mWeight ) );
            }
        }
    }
}

void XacExporter::ConvertMorphTargets ( ConversionContext& context )
{
    foreach ( std::shared_ptr < ActorNode > pActorNode, context.Actor->RootNodes )
    {
        ConvertMorphTargets ( pActorNode.get (), context );
    }
}

void XacExporter::ConvertMorphTargets ( ActorNode* pActorNode, ConversionContext& context )
{
    foreach ( std::shared_ptr<ActorNode> pChildActorNode, pActorNode->Children )
    {
        ConvertMorphTargets ( pChildActorNode.get (), context );
    }

    if ( !pActorNode->VisualMesh )
        return;

    ActorMesh* pActorMesh = pActorNode->VisualMesh.get ();
    int vertexOffset = 0;
    int subMeshIdx = 0;
    foreach ( ActorSubMesh& actorSubMesh, pActorMesh->SubMeshes )
    {
        aiMesh* pAiMesh = nullptr;
        for ( auto it = context.ConvertedSubMeshes.begin (); it != context.ConvertedSubMeshes.end (); ++it )
        {
            if ( it->second.pActorMesh == pActorMesh && it->second.SubMeshIndex == subMeshIdx )
            {
                pAiMesh = it->first;
                break;
            }
        }

        for ( int i = 0; i < pAiMesh->mNumMorphTargets; ++i )
        {
            ConvertMorphTarget ( pActorNode, pAiMesh, context.Scene->mMeshes[pAiMesh->mMorphTargets[i].mMesh], vertexOffset, context );
        }

        vertexOffset += pAiMesh->mNumVertices;
        subMeshIdx++;
    }
}

void XacExporter::ConvertMorphTarget ( ActorNode* pActorNode, aiMesh* pBaseAiMesh, aiMesh* pMorphTargetAiMesh, int vertexOffset, ConversionContext& context )
{
    if ( pBaseAiMesh->mNumVertices != pMorphTargetAiMesh->mNumVertices )
        throw (format("Morph target mesh %s must have the same number of vertices as base mesh %s") % pMorphTargetAiMesh->mName.C_Str() % pBaseAiMesh->mName.C_Str()).str ();

    ActorMorphTarget* pActorMorphTarget = context.Actor->GetMorphTarget ( pMorphTargetAiMesh->mName.C_Str () );
    if ( !pActorMorphTarget )
    {
        context.Actor->MorphTargets.resize ( context.Actor->MorphTargets.size () + 1 );
        pActorMorphTarget = &context.Actor->MorphTargets.back ();
        pActorMorphTarget->Name = pMorphTargetAiMesh->mName.C_Str ();
    }

    ActorNodeDeformation* pActorDeformation = pActorMorphTarget->GetDeformation ( pActorNode );
    if ( !pActorDeformation )
    {
        pActorMorphTarget->Deformations.resize ( pActorMorphTarget->Deformations.size () + 1 );
        pActorDeformation = &pActorMorphTarget->Deformations.back ();
        pActorDeformation->Node = pActorNode;
    }

    for ( int i = 0; i < pBaseAiMesh->mNumVertices; ++i )
    {
        pActorDeformation->VertexIndices.push_back ( vertexOffset + i );
        pActorDeformation->PositionOffsets.push_back ( pMorphTargetAiMesh->mVertices[i] - pBaseAiMesh->mVertices[i] );
        
        if ( pBaseAiMesh->mNormals && pMorphTargetAiMesh->mNormals )
            pActorDeformation->NormalOffsets.push_back ( pMorphTargetAiMesh->mNormals[i] - pBaseAiMesh->mNormals[i] );
        else
            pActorDeformation->NormalOffsets.push_back ( Vec3D () );

        if ( pBaseAiMesh->mTangents && pMorphTargetAiMesh->mTangents )
            pActorDeformation->TangentOffsets.push_back ( pMorphTargetAiMesh->mTangents[i] - pBaseAiMesh->mTangents[i] );
        else
            pActorDeformation->TangentOffsets.push_back ( Vec3D () );
    }
}
