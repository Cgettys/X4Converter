#include "StdInc.h"

using namespace boost;
using namespace boost::algorithm;
using namespace boost::filesystem;
using namespace Assimp;

XacImporter::XacImporter ( const std::string& gameBaseFolderPath )
    : _materialLibrary ( gameBaseFolderPath )
{

}

const aiImporterDesc* XacImporter::GetInfo () const
{
    static aiImporterDesc info;
    if ( !info.mAuthor )
    {
        info.mAuthor = "arc_";
        info.mFileExtensions = "xac";
        info.mName = "EmotionFX Actor importer";
        info.mMinMajor = 1;
        info.mMinMinor = 0;
        info.mMaxMajor = 1;
        info.mMaxMinor = 0;
    }
    return &info;
}

bool XacImporter::CanRead ( const std::string& filePath, IOSystem* pIOHandler, bool checkSig ) const
{
    return iends_with ( filePath, ".xac" );
}

void XacImporter::InternReadFile ( const std::string& filePath, aiScene* pScene, IOSystem* pIOHandler )
{
    IOStream* pStream = nullptr;
    try
    {
        pStream = pIOHandler->Open ( filePath );
        if ( !pStream )
            throw (format("%s doesn't exist") % filePath).str ();

        // Read .xac file
        ActorFile actor;
        actor.Read ( pStream );
        
        // Convert to Assimp data model
        ConversionContext context;
        context.Actor = &actor;
        std::vector < aiNode* > nodes;
        foreach ( std::shared_ptr < ActorNode > pActorNode, actor.RootNodes )
        {
            nodes.push_back ( ConvertNode ( pActorNode.get (), nullptr, context ) );
        }
        ConvertMorphTargets ( context );
        nodes.insert ( nodes.end (), context.MeshNodes.begin (), context.MeshNodes.end () );

        // Attach nodes
        aiNode* pRootNode = new aiNode ( path(filePath).stem ().string () );
        pRootNode->mChildren = new aiNode*[nodes.size ()];
        foreach ( aiNode* pNode, nodes )
        {
            pNode->mParent = pRootNode;
            pRootNode->mChildren[pRootNode->mNumChildren++] = pNode;
        }
        pScene->mRootNode = pRootNode;

        // Add meshes
        pScene->mMeshes = new aiMesh*[context.Meshes.size ()];
        foreach ( aiMesh* pMesh, context.Meshes )
        {
            AssimpUtil::MergeVertices ( pMesh );
            pScene->mMeshes[pScene->mNumMeshes++] = pMesh;
        }

        // Add materials
        std::string folderPath = path(filePath).parent_path ().string ();
        pScene->mMaterials = new aiMaterial*[actor.Materials.size ()];
        foreach ( ActorMaterial& material, actor.Materials )
        {
            pScene->mMaterials[pScene->mNumMaterials++] = ConvertMaterial ( &material, folderPath );
        }
    }
    catch ( std::string error )
    {
        if ( pStream )
            pIOHandler->Close ( pStream );

        throw DeadlyImportError ( error );
    }
    pIOHandler->Close ( pStream );
}

aiNode* XacImporter::ConvertNode ( ActorNode* pActorNode, aiNode* pParentAiNode, ConversionContext& context )
{
    aiNode* pAiNode = new aiNode ( pActorNode->Name );
    pAiNode->mParent = pParentAiNode;

    aiMatrix3x3 rotation = ((aiQuaternion)pActorNode->Rotation).GetMatrix ();
    pAiNode->mTransformation.a1 = rotation.a1 * pActorNode->Scale.X;
    pAiNode->mTransformation.a2 = rotation.a2 * pActorNode->Scale.Y;
    pAiNode->mTransformation.a3 = rotation.a3 * pActorNode->Scale.Z;
    pAiNode->mTransformation.b1 = rotation.b1 * pActorNode->Scale.X;
    pAiNode->mTransformation.b2 = rotation.b2 * pActorNode->Scale.Y;
    pAiNode->mTransformation.b3 = rotation.b3 * pActorNode->Scale.Z;
    pAiNode->mTransformation.c1 = rotation.c1 * pActorNode->Scale.X;
    pAiNode->mTransformation.c2 = rotation.c2 * pActorNode->Scale.Y;
    pAiNode->mTransformation.c3 = rotation.c3 * pActorNode->Scale.Z;

    pAiNode->mTransformation.a4 = pActorNode->Position.X;
    pAiNode->mTransformation.b4 = pActorNode->Position.Y;
    pAiNode->mTransformation.c4 = pActorNode->Position.Z;

    std::vector < aiNode* > childNodes;

    // Start with the assumption that the node is a bone
    pAiNode->mIsBone = true;

    foreach ( std::shared_ptr < ActorNode > pChildActorNode, pActorNode->Children )
    {
        aiNode* pChildAiNode = ConvertNode ( pChildActorNode.get (), pAiNode, context );
        childNodes.push_back ( pChildAiNode );

        // If one of the child nodes is at the same position as the current node, that would make
        // the current node a 0-length bone, which means it's not a bone but a dummy
        if ( pAiNode->mIsBone &&
             abs(pChildAiNode->mTransformation.a4) < 0.000001f &&
             abs(pChildAiNode->mTransformation.b4) < 0.000001f &&
             abs(pChildAiNode->mTransformation.c4) < 0.000001f )
        {
            pAiNode->mIsBone = false;
        }
    }

    // Root nodes with no children become dummies
    if ( !pParentAiNode && childNodes.empty () )
        pAiNode->mIsBone = false;

    if ( !pAiNode->mIsBone )
    {
        // If the current node is a dummy, then all its children are now root bone start points and the same rule as above applies
        foreach ( aiNode* pChildAiNode, childNodes )
        {
            if ( pChildAiNode->mNumChildren == 0 )
                pChildAiNode->mIsBone = false;
        }
    }

    // Add child nodes
    if ( !childNodes.empty () )
    {
        pAiNode->mChildren = new aiNode*[childNodes.size ()];
        foreach ( aiNode* pChildAiNode, childNodes )
        {
            pAiNode->mChildren[pAiNode->mNumChildren++] = pChildAiNode;
        }
    }

    // Convert meshes
    if ( pActorNode->VisualMesh || pActorNode->CollisionMesh )
    {
        if ( pActorNode->VisualMesh )
        {
            std::vector < aiNode* > meshNodes = ConvertMesh ( pActorNode, pActorNode->VisualMesh.get (), pAiNode, pActorNode->Name + "Xvisual", context );
            context.MeshNodes.insert ( context.MeshNodes.end (), meshNodes.begin (), meshNodes.end () );
        }

        if ( pActorNode->CollisionMesh )
        {
            std::vector < aiNode* > meshNodes = ConvertMesh ( pActorNode, pActorNode->CollisionMesh.get (), pAiNode, pActorNode->Name + "Xcollision", context );
            context.MeshNodes.insert ( context.MeshNodes.end (), meshNodes.begin (), meshNodes.end () );
        }
    }

    return pAiNode;
}

std::vector < aiNode* > XacImporter::ConvertMesh ( ActorNode* pActorNode, ActorMesh* pActorMesh, aiNode* pParentAiNode, const std::string& meshName, ConversionContext& context )
{
    std::vector < aiNode* > subMeshNodes;
    int subMeshIdx = 0;
    int vertexOffset = 0;
    foreach ( ActorSubMesh& subMesh, pActorMesh->SubMeshes )
    {
        std::string subMeshName = (format("%sX%d") % meshName % subMeshIdx).str ();

        int meshId = ConvertSubMesh ( subMeshName, &subMesh, pParentAiNode, context );
        context.SubMeshesByActorNode[pActorNode][vertexOffset] = context.Meshes[meshId];
        
        aiNode* pSubMeshNode = new aiNode ();
        pSubMeshNode->mName = subMeshName;
        pSubMeshNode->mNumMeshes = 1;
        pSubMeshNode->mMeshes = new uint[1];
        pSubMeshNode->mMeshes[0] = meshId;
        subMeshNodes.push_back ( pSubMeshNode );

        vertexOffset += subMesh.VertexPositions.size ();
        subMeshIdx++;
    }

    return subMeshNodes;
}

int XacImporter::ConvertSubMesh ( const std::string& subMeshName, ActorSubMesh* pSubMesh, aiNode* pParentAiNode, ConversionContext& context )
{
    aiMesh* pAiMesh = new aiMesh ();
    pAiMesh->mName = subMeshName;
    pAiMesh->mNumVertices = pSubMesh->VertexPositions.size ();

    aiMatrix4x4 bindShapeTransform;
    if ( pSubMesh->VertexInfluences.empty () && pParentAiNode->mIsBone )
        bindShapeTransform = pParentAiNode->GetWorldTransformation ();

    if ( !pSubMesh->VertexPositions.empty () )
    {
        pAiMesh->mVertices = new aiVector3D[pAiMesh->mNumVertices];
        for ( int i = 0; i < pAiMesh->mNumVertices; ++i )
        {
            pAiMesh->mVertices[i] = bindShapeTransform * pSubMesh->VertexPositions[i];
        }
    }

    bindShapeTransform.a4 = 0.0f;
    bindShapeTransform.b4 = 0.0f;
    bindShapeTransform.c4 = 0.0f;
    if ( !pSubMesh->VertexNormals.empty () )
    {
        pAiMesh->mNormals = new aiVector3D[pAiMesh->mNumVertices];
        for ( int i = 0; i < pAiMesh->mNumVertices; ++i )
        {
            pAiMesh->mNormals[i] = bindShapeTransform * pSubMesh->VertexNormals[i];
        }
    }

    if ( !pSubMesh->VertexTangents.empty () )
    {
        pAiMesh->mTangents = new aiVector3D[pAiMesh->mNumVertices];
        for ( int i = 0; i < pAiMesh->mNumVertices; ++i )
        {
            pAiMesh->mTangents[i] = bindShapeTransform * pSubMesh->VertexTangents[i];
        }
    }

    if ( !pSubMesh->VertexBitangents.empty () )
    {
        pAiMesh->mBitangents = new aiVector3D[pAiMesh->mNumVertices];
        for ( int i = 0; i < pAiMesh->mNumVertices; ++i )
        {
            pAiMesh->mBitangents[i] = bindShapeTransform * pSubMesh->VertexBitangents[i];
        }
    }

    if ( !pSubMesh->VertexUVs.empty () )
    {
        if ( pSubMesh->VertexUVs.size () > AI_MAX_NUMBER_OF_TEXTURECOORDS )
            throw std::string ( "Mesh has too many UV maps" );

        for ( int setIdx = 0; setIdx < pSubMesh->VertexUVs.size (); ++setIdx )
        {
            pAiMesh->mNumUVComponents[setIdx] = 2;
            pAiMesh->mTextureCoords[setIdx] = new aiVector3D[pAiMesh->mNumVertices];
            for ( int i = 0; i < pAiMesh->mNumVertices; ++i )
            {
                pAiMesh->mTextureCoords[setIdx][i] = aiVector3D (
                    pSubMesh->VertexUVs[setIdx][i].X,
                    1.0f - pSubMesh->VertexUVs[setIdx][i].Y,
                    0.0f
                );
            }
        }
    }

    if ( !pSubMesh->VertexColors32.empty () )
    {
        if ( pSubMesh->VertexColors32.size () > AI_MAX_NUMBER_OF_COLOR_SETS )
            throw std::string ( "Mesh has too many 32-bit color sets" );

        for ( int setIdx = 0; setIdx < pSubMesh->VertexColors32.size (); ++setIdx )
        {
            pAiMesh->mColors[setIdx] = new aiColor4D[pAiMesh->mNumVertices];
            for ( int i = 0; i < pAiMesh->mNumVertices; ++i )
            {
                pAiMesh->mColors[setIdx][i] = pSubMesh->VertexColors32[setIdx][i];
            }
        }
    }
    else if ( !pSubMesh->VertexColors128.empty () )
    {
        if ( pSubMesh->VertexColors128.size () > AI_MAX_NUMBER_OF_COLOR_SETS )
            throw std::string ( "Mesh has too many 128-bit color sets" );

        for ( int setIdx = 0; setIdx < pSubMesh->VertexColors128.size (); ++setIdx )
        {
            pAiMesh->mColors[setIdx] = new aiColor4D[pAiMesh->mNumVertices];
            for ( int i = 0; i < pAiMesh->mNumVertices; ++i )
            {
                pAiMesh->mColors[setIdx][i] = pSubMesh->VertexColors128[setIdx][i];
            }
        }
    }

    if ( !pSubMesh->Indices.empty () )
    {
        pAiMesh->mNumFaces = pSubMesh->Indices.size () / 3;
        pAiMesh->mFaces = new aiFace[pAiMesh->mNumFaces];
        for ( int i = 0; i < pAiMesh->mNumFaces; ++i )
        {
            aiFace* pFace = &pAiMesh->mFaces[i];
            pFace->mIndices = new uint[3];
            while ( pFace->mNumIndices < 3 )
            {
                pFace->mIndices[pFace->mNumIndices] = pSubMesh->Indices[3*i + pFace->mNumIndices];
                pFace->mNumIndices++;
            }
        }
    }

    pAiMesh->mMaterialIndex = pSubMesh->MaterialId;
    ConvertBoneInfluences ( pSubMesh, pAiMesh, pParentAiNode, context );
    
    int meshId = context.Meshes.size ();
    context.Meshes.push_back ( pAiMesh );
    return meshId;
}

void XacImporter::ConvertMorphTargets ( ConversionContext& context )
{
    std::map < aiMesh*, std::map < ActorMorphTarget*, aiMesh* > > deformedSubMeshes;
    foreach ( ActorMorphTarget& morphTarget, context.Actor->MorphTargets )
    {
        foreach ( ActorNodeDeformation deformation, morphTarget.Deformations )
        {
            for ( int i = 0; i < deformation.VertexIndices.size (); ++i )
            {
                int vertexIdx = deformation.VertexIndices[i];
                auto itSubMesh = context.SubMeshesByActorNode[deformation.Node].upper_bound ( vertexIdx );
                itSubMesh--;
                vertexIdx -= itSubMesh->first;
                aiMesh* pBaseSubMesh = itSubMesh->second;
                if ( vertexIdx >= pBaseSubMesh->mNumVertices )
                    throw std::string ( "Invalid vertex index in deformation" );

                aiMesh* pDeformedSubMesh = nullptr;
                auto itBaseMesh = deformedSubMeshes.find ( pBaseSubMesh );
                if ( itBaseMesh != deformedSubMeshes.end () )
                {
                    auto itDeformedMesh = itBaseMesh->second.find ( &morphTarget );
                    if ( itDeformedMesh != itBaseMesh->second.end () )
                        pDeformedSubMesh = itDeformedMesh->second;
                }

                if ( !pDeformedSubMesh )
                {
                    SceneCombiner::Copy ( &pDeformedSubMesh, pBaseSubMesh );
                    pDeformedSubMesh->mName = morphTarget.Name;
                    deformedSubMeshes[pBaseSubMesh][&morphTarget] = pDeformedSubMesh;
                }

                if ( pDeformedSubMesh->mVertices )
                    pDeformedSubMesh->mVertices[vertexIdx] += deformation.PositionOffsets[i];
                
                if ( pDeformedSubMesh->mNormals )
                    pDeformedSubMesh->mNormals[vertexIdx] += deformation.NormalOffsets[i];

                if ( pDeformedSubMesh->mTangents )
                    pDeformedSubMesh->mTangents[vertexIdx] += deformation.TangentOffsets[i];
            }
        }
    }

    for ( auto itBaseMesh = deformedSubMeshes.begin (); itBaseMesh != deformedSubMeshes.end (); ++itBaseMesh )
    {
        aiMesh* pBaseMesh = itBaseMesh->first;
        pBaseMesh->mMorphTargets = new aiMorphTarget[itBaseMesh->second.size ()];
        for ( auto itDeformedMesh = itBaseMesh->second.begin (); itDeformedMesh != itBaseMesh->second.end (); ++itDeformedMesh )
        {
            int deformedMeshId = context.Meshes.size ();
            context.Meshes.push_back ( itDeformedMesh->second );
            pBaseMesh->mMorphTargets[pBaseMesh->mNumMorphTargets++].mMesh = deformedMeshId;
        }
    }
}

void XacImporter::ConvertBoneInfluences ( ActorSubMesh* pSubMesh, aiMesh* pAiMesh, aiNode* pParentAiNode, ConversionContext& context )
{
    if ( pSubMesh->VertexInfluences.empty () )
    {
        if ( !pParentAiNode->mIsBone )
            return;

        aiBone* pImplicitBone = new aiBone ();
        pImplicitBone->mName = pParentAiNode->mName;
        pImplicitBone->mNumWeights = pAiMesh->mNumVertices;
        pImplicitBone->mWeights = new aiVertexWeight[pAiMesh->mNumVertices];
        for ( int i = 0; i < pAiMesh->mNumVertices; ++i )
        {
            pImplicitBone->mWeights[i] = aiVertexWeight ( i, 1.0f );
        }

        pAiMesh->mBones = new aiBone*[1];
        pAiMesh->mBones[pAiMesh->mNumBones++] = pImplicitBone;
        return;
    }

    std::map < ActorNode*, std::vector < aiVertexWeight > > weights;
    for ( int i = 0; i < pSubMesh->VertexInfluences.size (); ++i )
    {
        std::vector < BoneInfluence >& influences = pSubMesh->VertexInfluences [ i ];
        foreach ( BoneInfluence& influence, influences )
        {
            weights[influence.Node].push_back ( aiVertexWeight ( i, influence.Weight ) );
        }
    }

    pAiMesh->mBones = new aiBone*[weights.size ()];
    for ( auto it = weights.begin (); it != weights.end (); ++it )
    {
        ActorNode* pActorNode = it->first;
        aiBone* pBone = new aiBone ();
        pBone->mName = pActorNode->Name;

        pBone->mWeights = new aiVertexWeight [ it->second.size () ];
        foreach ( aiVertexWeight& weight, it->second )
        {
            pBone->mWeights[pBone->mNumWeights++] = weight;
        }

        pAiMesh->mBones[pAiMesh->mNumBones++] = pBone;
    }
}

aiMaterial* XacImporter::ConvertMaterial ( ActorMaterial* pActorMaterial, const std::string& modelFolderPath )
{
    Material* pMaterial = _materialLibrary.GetMaterial ( pActorMaterial->Name );
    if ( pMaterial )
        return pMaterial->ConvertToAiMaterial ( modelFolderPath );

    aiMaterial* pAiMaterial = new aiMaterial ();

    pAiMaterial->AddProperty ( new aiString(pActorMaterial->Name), AI_MATKEY_NAME );

    aiColor4D ambientColor = pActorMaterial->AmbientColor;
    pAiMaterial->AddProperty ( &ambientColor, 1, AI_MATKEY_COLOR_AMBIENT );

    aiColor4D diffuseColor = pActorMaterial->DiffuseColor;
    pAiMaterial->AddProperty ( &diffuseColor, 1, AI_MATKEY_COLOR_DIFFUSE );

    aiColor4D specularColor = pActorMaterial->SpecularColor;
    pAiMaterial->AddProperty ( &specularColor, 1, AI_MATKEY_COLOR_SPECULAR );

    aiColor4D emissiveColor = pActorMaterial->EmissiveColor;
    pAiMaterial->AddProperty ( &emissiveColor, 1, AI_MATKEY_COLOR_EMISSIVE );

    pAiMaterial->AddProperty ( &pActorMaterial->Shine, 1, AI_MATKEY_SHININESS );
    pAiMaterial->AddProperty ( &pActorMaterial->ShineStrength, 1, AI_MATKEY_SHININESS_STRENGTH );
    pAiMaterial->AddProperty ( &pActorMaterial->Opacity, 1, AI_MATKEY_OPACITY );
    pAiMaterial->AddProperty ( &pActorMaterial->DoubleSided, 1, AI_MATKEY_TWOSIDED );
    pAiMaterial->AddProperty ( &pActorMaterial->Wireframe, 1, AI_MATKEY_ENABLE_WIREFRAME );

    for ( int i = 0; i < pActorMaterial->Layers.size (); ++i )
    {
        ActorMaterialLayer* pLayer = &pActorMaterial->Layers[i];
        pAiMaterial->AddProperty ( new aiString(pLayer->Texture), 1, AI_MATKEY_TEXTURE_DIFFUSE(i) );
    }

    return pAiMaterial;
}
