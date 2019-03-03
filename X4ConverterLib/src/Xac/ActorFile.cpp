#include "StdInc.h"

using namespace boost;
using namespace boost::algorithm;
using namespace boost::filesystem;
using namespace Assimp;

ActorFile::ChunkReader ActorFile::_chunkReader;

void ActorFile::Read ( IOStream* pStream )
{
    BinaryReader reader ( pStream );
    ReadHeader ( reader );

    ReadContext context;
    while ( _chunkReader.ReadChunk ( reader, this, context ) );
}

void ActorFile::Write ( Assimp::IOStream* pStream )
{
    WriteContext context;
    foreach ( std::shared_ptr < ActorNode > pNode, RootNodes )
    {
        CollectNodes ( pNode.get (), context.Nodes );
    }
    for ( int i = 0; i < context.Nodes.size (); ++i )
    {
        context.NodeIndices[context.Nodes[i]] = i;
    }

    BinaryWriter writer ( pStream );
    WriteHeader ( writer );
    WriteMetaData ( writer );
    WriteNodeHierarchy ( writer, context );
    WriteMaterialTotals ( writer );
    for ( int i = 0; i < Materials.size (); ++i )
    {
        WriteMaterialDefinition ( writer, i );
    }
    for ( int i = 0; i < context.Nodes.size (); ++i )
    {
        if ( context.Nodes[i]->VisualMesh )
            WriteMesh ( writer, i, context.Nodes[i]->VisualMesh.get (), false, context );

        if ( context.Nodes[i]->CollisionMesh )
            WriteMesh ( writer, i, context.Nodes[i]->CollisionMesh.get (), true, context );
    }
    for ( int i = 0; i < context.Nodes.size (); ++i )
    {
        if ( context.Nodes[i]->VisualMesh )
            WriteBoneInfluences ( writer, i, context.Nodes[i]->VisualMesh.get (), false, context );

        if ( context.Nodes[i]->CollisionMesh )
            WriteBoneInfluences ( writer, i, context.Nodes[i]->CollisionMesh.get (), true, context );
    }
    WriteMorphTargets ( writer, context );
}

ActorNode* ActorFile::GetNode ( const std::string& name )
{
    foreach ( std::shared_ptr<ActorNode> pNode, RootNodes )
    {
        ActorNode* pFoundNode = pNode->GetNode ( name );
        if ( pFoundNode )
            return pFoundNode;
    }
    return nullptr;
}

ActorMorphTarget* ActorFile::GetMorphTarget ( const std::string& name )
{
    foreach ( ActorMorphTarget& morphTarget, MorphTargets )
    {
        if ( morphTarget.Name == name )
            return &morphTarget;
    }
    return nullptr;
}

XacHeader ActorFile::ReadHeader ( BinaryReader reader )
{
    XacHeader header = reader.Read < XacHeader > ();
    if ( memcmp ( header.Magic, "XAC ", 4 ) )
        throw std::string ( "Not an XAC file: invalid header magic" );

    if ( header.MajorVersion != 1 || header.MinorVersion != 0 )
        throw (format("Unsupported .xac version: expected v1.0, file is v%d.%d") % header.MajorVersion % header.MinorVersion).str ();

    if ( header.BigEndian )
        throw std::string ( "XAC file is encoded in big endian which is not supported by this importer" );

    return header;
}

void ActorFile::ReadMetaDataV2 ( BinaryReader reader, ReadContext& context )
{
    reader.Read < XacMetaDataChunkv2Header > ();
    auto sourceApp = reader.ReadString ();
    OriginalFileName = reader.ReadString ();
    auto exportDate = reader.ReadString ();
    ActorName = reader.ReadString ();
}

void ActorFile::ReadNodeHierarchyV1 ( BinaryReader reader, ReadContext& context )
{
    int numNodes = reader.ReadInt32 ();
    int numRootNodes = reader.ReadInt32 ();

    if ( numNodes <= 0 )
        throw std::string ( "Invalid number of nodes" );

    context.Nodes.reserve ( numNodes );
    for ( int i = 0; i < numNodes; ++i )
    {
        auto nodeInfo = reader.Read < XacNodeHierarchyChunkv1NodeHeader > ();
        std::string nodeName = reader.ReadString ();

        std::shared_ptr < ActorNode > pNode = std::make_shared < ActorNode > ();
        pNode->Name = nodeName;
        pNode->Position = nodeInfo.Position;
        pNode->Rotation = nodeInfo.Rotation;
        pNode->Scale = nodeInfo.Scale;
        pNode->ScaleRotation = nodeInfo.ScaleRotation;
        pNode->Transform = nodeInfo.Transform;

        if ( nodeInfo.ParentNodeId == -1 )
        {
            pNode->Parent = nullptr;
            RootNodes.push_back ( pNode );
        }
        else
        {
            ActorNode* pParentNode = context.Nodes[nodeInfo.ParentNodeId];
            if ( !pParentNode )
                throw std::string ( "Child node specified before its parent" );

            pNode->Parent = pParentNode;
            pParentNode->Children.push_back ( pNode );
        }

        context.Nodes.push_back ( pNode.get () );
    }

    if ( RootNodes.size () != numRootNodes )
        throw std::string ( "numRootNodes does not match number of nodes with parent ID -1" );
}

void ActorFile::ReadMaterialTotalsV1 ( BinaryReader reader, ReadContext& context )
{
    auto totals = reader.Read < XacMaterialTotalsChunkv1 > ();
    if ( totals.NumStandardMaterials <= 0 )
        throw std::string ( "Invalid number of standard materials" );

    if ( totals.NumFxMaterials != 0 )
        throw std::string ( "Actor uses FX materials which are not supported by this importer" );

    if ( totals.NumTotalMaterials != totals.NumStandardMaterials + totals.NumFxMaterials )
        throw std::string ( "Incorrect numTotalMaterials (must be sum of standard materials and fx materials" );

    Materials.reserve ( totals.NumStandardMaterials );
}

void ActorFile::ReadMaterialDefinitionV2 ( BinaryReader reader, ReadContext& context )
{
    if ( Materials.size () == Materials.capacity () )
        throw std::string ( "Too many materials defined" );

    auto materialInfo = reader.Read < XacMaterialDefinitionChunkv2Header > ();
    std::string materialName = reader.ReadString ();

    Materials.resize ( Materials.size () + 1 );
    ActorMaterial& material = Materials.back ();
    material.Name = materialName;
    material.AmbientColor = materialInfo.AmbientColor;
    material.DiffuseColor = materialInfo.DiffuseColor;
    material.SpecularColor = materialInfo.SpecularColor;
    material.EmissiveColor = materialInfo.EmissiveColor;
    material.Shine = materialInfo.Shine;
    material.ShineStrength = materialInfo.ShineStrength;
    material.Opacity = materialInfo.Opacity;
    material.IOR = materialInfo.IOR;
    material.DoubleSided = materialInfo.DoubleSided;
    material.Wireframe = materialInfo.Wireframe;

    material.Layers.resize ( materialInfo.NumLayers );
    for ( int i = 0; i < materialInfo.NumLayers; ++i )
    {
        auto layerInfo = reader.Read < XacMaterialDefinitionChunkv2Layer > ();
        std::string texture = reader.ReadString ();

        if ( layerInfo.MaterialId != Materials.size () - 1 )
            throw std::string ( "Invalid material ID in material layer" );

        ActorMaterialLayer& layer = material.Layers[i];
        layer.Texture = texture;
        layer.Amount = layerInfo.Amount;
        layer.UOffset = layerInfo.UOffset;
        layer.VOffset = layerInfo.VOffset;
        layer.UTiling = layerInfo.UTiling;
        layer.VTiling = layerInfo.VTiling;
        layer.Rotation = layerInfo.Rotation;
        layer.MapType = layerInfo.MapType;
    }
}

void ActorFile::ReadMeshV1 ( BinaryReader reader, ReadContext& context )
{
    auto meshInfo = reader.Read < XacMeshChunkv1Header > ();
    if ( meshInfo.NodeId < 0 || meshInfo.NodeId >= context.Nodes.size () )
        throw std::string ( "Invalid node ID in mesh" );

    if ( meshInfo.NumInfluenceRanges < 0 )
        throw std::string ( "Invalid number of influence ranges in mesh" );

    if ( meshInfo.NumVertices <= 0 )
        throw std::string ( "Invalid number of vertices in mesh" );

    if ( meshInfo.NumIndices <= 0 )
        throw std::string ( "Invalid number of indices in mesh" );

    if ( meshInfo.NumSubMeshes <= 0 )
        throw std::string ( "Invalid number of sub meshes in mesh" );

    if ( meshInfo.NumVertexElements <= 0 )
        throw std::string ( "Invalid number of vertex elements in mesh" );

    ActorNode* pNode = context.Nodes[meshInfo.NodeId];
    std::shared_ptr < ActorMesh > pMesh = std::make_shared < ActorMesh > ();
    if ( !meshInfo.IsCollisionMesh )
    {
        if ( pNode->VisualMesh )
            throw std::string ( "Node already has a visual mesh" );

        pNode->VisualMesh = pMesh;
    }
    else
    {
        if ( pNode->CollisionMesh )
            throw std::string ( "Node already has a collision mesh" );

        pNode->CollisionMesh = pMesh;
    }

    context.MeshNumInfluenceRanges [ pMesh.get () ] = meshInfo.NumInfluenceRanges;

    std::vector < Vec3D > positions;
    std::vector < Vec3D > normals;
    std::vector < Vec4D > tangents;
    std::vector < Vec4D > bitangents;
    std::vector < std::vector < Vec2D > > uvs;
    std::vector < int > influenceRangeIndices;
    std::vector < std::vector < Color8 > > colors32;
    std::vector < std::vector < Color > > colors128;
    for ( int i = 0; i < meshInfo.NumVertexElements; ++i )
    {
        auto elemInfo = reader.Read < XacMeshChunkv1VertexElement > ();
        switch ( elemInfo.Usage )
        {
            case EMOFX_DECLUSAGE_POSITION:
                if ( elemInfo.ElementSize != sizeof(positions[0]) )
                    throw std::string ( "Invalid position element size" );

                if ( !positions.empty () )
                    throw std::string ( "Duplicate position element list" );

                reader.Read ( positions, meshInfo.NumVertices );
                break;

            case EMOFX_DECLUSAGE_NORMAL:
                if ( elemInfo.ElementSize != sizeof(normals[0]) )
                    throw std::string ( "Invalid normal element size" );

                if ( !normals.empty () )
                    throw std::string ( "Duplicate normal element list" );

                reader.Read ( normals, meshInfo.NumVertices );
                break;

            case EMOFX_DECLUSAGE_TANGENT:
                if ( elemInfo.ElementSize != sizeof(tangents[0]) )
                    throw std::string ( "Invalid tangent element size" );

                if ( tangents.empty () )
                    reader.Read ( tangents, meshInfo.NumVertices );
                else if ( bitangents.empty () )
                    reader.Read ( bitangents, meshInfo.NumVertices );
                else
                    throw std::string ( "Duplicate tangent element list" );
                
                break;

            case EMOFX_DECLUSAGE_TEXCOORD:
                if ( elemInfo.ElementSize != sizeof(uvs[0][0]) )
                    throw std::string ( "Invalid UV element size" );

                uvs.resize ( uvs.size () + 1 );
                reader.Read ( uvs.back (), meshInfo.NumVertices );
                break;

            case EMOFX_DECLUSAGE_INFLUENCERANGEIDX:
            {
                if ( elemInfo.ElementSize != sizeof(influenceRangeIndices[0]) )
                    throw std::string ( "Invalid influence range index element size" );

                if ( !influenceRangeIndices.empty () )
                    throw std::string ( "Duplicate influence range element list" );

                reader.Read ( influenceRangeIndices, meshInfo.NumVertices );
                break;
            }

            case EMOFX_DECLUSAGE_COLOR32:
                if ( elemInfo.ElementSize != sizeof(colors32[0][0]) )
                    throw std::string ( "Invalid 32-bit color element size" );

                colors32.resize ( colors32.size () + 1 );
                reader.Read ( colors32.back (), meshInfo.NumVertices );
                break;

            case EMOFX_DECLUSAGE_COLOR128:
                if ( elemInfo.ElementSize != sizeof(colors128[0][0]) )
                    throw std::string ( "Invalid 128-bit color element list" );

                colors128.resize ( colors128.size () + 1 );
                reader.Read ( colors128.back (), meshInfo.NumVertices );
                break;

            default:
                throw (format("Encountered unknown vertex element usage %d") % elemInfo.Usage).str ();
        }
    }

    pMesh->SubMeshes.resize ( meshInfo.NumSubMeshes );
    int vertexOffset = 0;
    int indexOffset = 0;
    for ( int i = 0; i < meshInfo.NumSubMeshes; ++i )
    {
        auto subMeshInfo = reader.Read < XacMeshChunkv1SubMesh > ();
        if ( subMeshInfo.NumVertices <= 0 || vertexOffset + subMeshInfo.NumVertices > meshInfo.NumVertices )
            throw std::string ( "Invalid number of vertices in submesh" );
        
        if ( subMeshInfo.NumIndices <= 0 || indexOffset + subMeshInfo.NumIndices > meshInfo.NumIndices )
            throw std::string ( "Invalid number of indices in submesh" );

        if ( subMeshInfo.NumIndices % 3 )
            throw std::string ( "Number of indices in submesh is not divisible by 3" );

        if ( subMeshInfo.NumBones < 0 )
            throw std::string ( "Invalid number of bones in submesh" );

        if ( subMeshInfo.MaterialId < 0 || subMeshInfo.MaterialId >= Materials.size () )
            throw std::string ( "Invalid material ID in submesh" );

        ActorSubMesh& subMesh = pMesh->SubMeshes[i];
        subMesh.MaterialId = subMeshInfo.MaterialId;

        if ( !positions.empty () )
            subMesh.VertexPositions.assign ( positions.begin () + vertexOffset, positions.begin () + vertexOffset + subMeshInfo.NumVertices );

        if ( !normals.empty () )
            subMesh.VertexNormals.assign ( normals.begin () + vertexOffset, normals.begin () + vertexOffset + subMeshInfo.NumVertices );

        if ( !tangents.empty () )
            subMesh.VertexTangents.assign ( tangents.begin () + vertexOffset, tangents.begin () + vertexOffset + subMeshInfo.NumVertices );

        if ( !bitangents.empty () )
            subMesh.VertexBitangents.assign ( bitangents.begin () + vertexOffset, bitangents.begin () + vertexOffset + subMeshInfo.NumVertices );

        if ( !uvs.empty () )
        {
            foreach ( std::vector < Vec2D >& uvset, uvs )
            {
                subMesh.VertexUVs.resize ( subMesh.VertexUVs.size () + 1 );
                subMesh.VertexUVs.back ().assign ( uvset.begin () + vertexOffset, uvset.begin () + vertexOffset + subMeshInfo.NumVertices );
            }
        }

        if ( !influenceRangeIndices.empty () )
            context.SubMeshInfluenceRangeIndices[&subMesh].assign ( influenceRangeIndices.begin () + vertexOffset, influenceRangeIndices.begin () + vertexOffset + subMeshInfo.NumVertices );

        if ( !colors32.empty () && !colors128.empty () )
            throw std::string ( "Mesh is using both 32-bit and 128-bit colors" );

        if ( !colors32.empty () )
        {
            foreach ( std::vector < Color8 >& colorset, colors32 )
            {
                subMesh.VertexColors32.resize ( subMesh.VertexColors32.size () + 1 );
                subMesh.VertexColors32.back ().assign ( colorset.begin () + vertexOffset, colorset.begin () + vertexOffset + subMeshInfo.NumVertices );
            }
        }

        if ( !colors128.empty () )
        {
            foreach ( std::vector < Color >& colorset, colors128 )
            {
                subMesh.VertexColors128.resize ( subMesh.VertexColors128.size () + 1 );
                subMesh.VertexColors128.back ().assign ( colorset.begin () + vertexOffset, colorset.begin () + vertexOffset + subMeshInfo.NumVertices );
            }
        }

        reader.Read ( subMesh.Indices, subMeshInfo.NumIndices );
        reader.Skip < int > ( subMeshInfo.NumBones );
        vertexOffset += subMeshInfo.NumVertices;
        indexOffset += subMeshInfo.NumIndices;
    }

    if ( vertexOffset != meshInfo.NumVertices )
        throw std::string ( "Number of vertices in mesh does not equal total number of vertices in submeshes" );

    if ( indexOffset != meshInfo.NumIndices )
        throw std::string ( "Number of indices in mesh does not equal total number of indices in submeshes" );
}

void ActorFile::ReadBoneInfluencesV3 ( BinaryReader reader, ReadContext& context )
{
    auto influencesHeader = reader.Read < XacSkinningChunkv3Header > ();
    if ( influencesHeader.NodeId < 0 || influencesHeader.NodeId >= context.Nodes.size () )
        throw std::string ( "Invalid node ID in skinning chunk" );

    if ( influencesHeader.NumInfluences <= 0 )
        throw std::string ( "Invalid number of influences in skinning chunk" );

    ActorNode* pNode = context.Nodes[influencesHeader.NodeId];
    std::shared_ptr<ActorMesh> pMesh = (!influencesHeader.IsForCollisionMesh ? pNode->VisualMesh : pNode->CollisionMesh);
    if ( !pMesh )
        throw std::string ( "File contains skinning data for nonexistant mesh" );

    std::vector < XacSkinningChunkv3Influence > influences;
    reader.Read ( influences, influencesHeader.NumInfluences );

    int numInfluenceRanges = context.MeshNumInfluenceRanges [ pMesh.get () ];
    std::vector < XacSkinningChunkv3InfluenceRange > influenceRanges;
    reader.Read ( influenceRanges, numInfluenceRanges );

    foreach ( ActorSubMesh& subMesh, pMesh->SubMeshes )
    {
        if ( !subMesh.VertexInfluences.empty () )
            throw std::string ( "Duplicate vertex influences for submesh" );

        std::vector < int >& influenceRangeIndices = context.SubMeshInfluenceRangeIndices[&subMesh];
        if ( influenceRangeIndices.empty () )
            continue;

        subMesh.VertexInfluences.resize ( influenceRangeIndices.size () );
        for ( int vertexIdx = 0; vertexIdx < influenceRangeIndices.size (); ++vertexIdx )
        {
            XacSkinningChunkv3InfluenceRange& range = influenceRanges[influenceRangeIndices[vertexIdx]];
            subMesh.VertexInfluences[vertexIdx].reserve ( range.NumInfluences );
            for ( int i = range.FirstInfluenceIndex; i < range.FirstInfluenceIndex + range.NumInfluences; ++i )
            {
                XacSkinningChunkv3Influence& influence = influences[i];
                subMesh.VertexInfluences[vertexIdx].push_back ( BoneInfluence ( context.Nodes[influence.NodeId], influence.Weight ) );
            }
        }
    }
}

void ActorFile::ReadMorphTargetsV1 ( BinaryReader reader, ReadContext& context )
{
    int numMorphTargets = reader.ReadInt32 ();
    if ( numMorphTargets <= 0 )
        throw std::string ( "Invalid number of morph targets" );

    int morphTargetLodIdx = reader.ReadInt32 ();
    if ( morphTargetLodIdx != 0 )
        throw std::string ( "Unexpected morph target LOD index" );

    if ( !MorphTargets.empty () )
        throw std::string ( "Duplicate morph target list" );

    MorphTargets.resize ( numMorphTargets );
    foreach ( ActorMorphTarget& morphTarget, MorphTargets )
    {
        auto morphTargetInfo = reader.Read < XacMorphTargetsChunkv1MorphTarget > ();
        if ( morphTargetInfo.LodLevel != 0 )
            throw std::string ( "Unexpected morph target LOD level" );

        if ( morphTargetInfo.NumDeformations < 0 )
            throw std::string ( "Invalid number of deformations in morph target" );

        if ( morphTargetInfo.NumTransformations != 0 )
            throw std::string ( "Morph target uses transformations which are not supported by this importer" );

        morphTarget.Name = reader.ReadString ();
        morphTarget.RangeMin = morphTargetInfo.RangeMin;
        morphTarget.RangeMax = morphTargetInfo.RangeMax;
        morphTarget.PhonemeSetBitmask = morphTargetInfo.PhonemeSetBitmask;

        morphTarget.Deformations.resize ( morphTargetInfo.NumDeformations );
        foreach ( ActorNodeDeformation& deformation, morphTarget.Deformations )
        {
            auto deformationInfo = reader.Read < XacMorphTargetsChunkv1Deformation > ();
            if ( deformationInfo.NodeId < 0 || deformationInfo.NodeId >= context.Nodes.size () )
                throw std::string ( "Deformation has invalid node ID" );

            deformation.Node = context.Nodes[deformationInfo.NodeId];

            std::vector < XacDeformVec3D16 > positionOffsets;
            reader.Read ( positionOffsets, deformationInfo.NumVertices );
            deformation.PositionOffsets.reserve ( deformationInfo.NumVertices );
            foreach ( XacDeformVec3D16& posOffset, positionOffsets )
            {
                deformation.PositionOffsets.push_back (
                    Vec3D (
                        deformationInfo.MinValue + (deformationInfo.MaxValue - deformationInfo.MinValue)*((float)posOffset.X / 65535.0f),
                        deformationInfo.MinValue + (deformationInfo.MaxValue - deformationInfo.MinValue)*((float)posOffset.Y / 65535.0f),
                        deformationInfo.MinValue + (deformationInfo.MaxValue - deformationInfo.MinValue)*((float)posOffset.Z / 65535.0f)
                    )
                );
            }

            std::vector < XacDeformVec3D8 > normalOffsets;
            reader.Read ( normalOffsets, deformationInfo.NumVertices );
            deformation.NormalOffsets.reserve ( deformationInfo.NumVertices );
            foreach ( XacDeformVec3D8& normalOffset, normalOffsets )
            {
                deformation.NormalOffsets.push_back (
                    Vec3D (
                        (float)normalOffset.X / 127.5f - 1.0f,
                        (float)normalOffset.Y / 127.5f - 1.0f,
                        (float)normalOffset.Z / 127.5f - 1.0f
                    )
                );
            }

            std::vector < XacDeformVec3D8 > tangentOffsets;
            reader.Read ( tangentOffsets, deformationInfo.NumVertices );
            deformation.TangentOffsets.reserve ( deformationInfo.NumVertices );
            foreach ( XacDeformVec3D8& tangentOffset, tangentOffsets )
            {
                deformation.TangentOffsets.push_back (
                    Vec3D (
                        (float)tangentOffset.X / 127.5f - 1.0f,
                        (float)tangentOffset.Y / 127.5f - 1.0f,
                        (float)tangentOffset.Z / 127.5f - 1.0f
                    )
                );
            }

            reader.Read ( deformation.VertexIndices, deformationInfo.NumVertices );
        }
    }
}

void ActorFile::WriteHeader ( BinaryWriter writer )
{
    XacHeader header;
    memcpy ( header.Magic, "XAC ", 4 );
    header.MajorVersion = 1;
    header.MinorVersion = 0;
    header.BigEndian = false;
    header.MultiplyOrder = 1;
    writer.Write ( header );
}

void ActorFile::WriteMetaData ( BinaryWriter writer )
{
    writer.Write ( XacChunkHeader ( EMOFX_CHUNK_XAC_METADATA, 2 ) );
    
    XacMetaDataChunkv2Header chunkHeader;
    chunkHeader.RepositionMask = 1;
    chunkHeader.RepositioningNode = -1;
    chunkHeader.ExporterMajorVersion = 1;
    chunkHeader.ExporterMinorVersion = 0;
    chunkHeader.RetargetRootOffset = 0.0f;
    writer.Write ( chunkHeader );
    writer.Write ( std::string("XRConverters") );
    writer.Write ( OriginalFileName );
    writer.Write ( std::string("") );
    writer.Write ( ActorName );
}

void ActorFile::WriteNodeHierarchy ( BinaryWriter writer, WriteContext& context )
{
    writer.Write ( XacChunkHeader ( EMOFX_CHUNK_XAC_NODE_HIERARCHY, 1 ) );
    writer.Write ( context.Nodes.size () );
    writer.Write ( RootNodes.size () );
    foreach ( ActorNode* pNode, context.Nodes )
    {
        XacNodeHierarchyChunkv1NodeHeader nodeData;
        nodeData.Position = pNode->Position;
        nodeData.Rotation = pNode->Rotation;
        nodeData.Scale = pNode->Scale;
        nodeData.ScaleRotation = pNode->ScaleRotation;
        nodeData.UnknownIndex1 = -1;
        nodeData.UnknownIndex2 = -1;
        nodeData.ParentNodeId = (pNode->Parent ? context.NodeIndices[pNode->Parent] : -1);
        nodeData.NumChildren = pNode->Children.size ();
        nodeData.ImportanceFactor = 1.0f;
        nodeData.IncludeInBoundsCalc = 1;
        nodeData.Transform = pNode->Transform;
        
        writer.Write ( nodeData );
        writer.Write ( pNode->Name );
    }
}

void ActorFile::WriteMaterialTotals ( BinaryWriter writer )
{
    writer.Write ( XacChunkHeader ( EMOFX_CHUNK_XAC_MATERIAL_TOTALS, 1 ) );
    writer.Write ( Materials.size () );
    writer.Write ( Materials.size () );
    writer.Write ( 0 );
}

void ActorFile::WriteMaterialDefinition ( BinaryWriter writer, int materialId )
{
    writer.Write ( XacChunkHeader ( EMOFX_CHUNK_XAC_MATERIAL_DEFINITION, 2 ) );
 
    ActorMaterial& material = Materials[materialId];
    XacMaterialDefinitionChunkv2Header materialData;
    materialData.AmbientColor = material.AmbientColor;
    materialData.DiffuseColor = material.DiffuseColor;
    materialData.SpecularColor = material.SpecularColor;
    materialData.EmissiveColor = material.EmissiveColor;
    materialData.Shine = material.Shine;
    materialData.ShineStrength = material.ShineStrength;
    materialData.Opacity = material.Opacity;
    materialData.IOR = material.IOR;
    materialData.DoubleSided = material.DoubleSided;
    materialData.Wireframe = material.Wireframe;
    materialData.NumLayers = material.Layers.size ();
    writer.Write ( materialData );
    writer.Write ( material.Name );

    foreach ( ActorMaterialLayer& layer, material.Layers )
    {
        XacMaterialDefinitionChunkv2Layer layerData;
        layerData.Amount = layer.Amount;
        layerData.UOffset = layer.UOffset;
        layerData.VOffset = layer.VOffset;
        layerData.UTiling = layer.UTiling;
        layerData.VTiling = layer.VTiling;
        layerData.Rotation = layer.Rotation;
        layerData.MaterialId = materialId;
        layerData.MapType = layer.MapType;
        writer.Write ( layerData );
        writer.Write ( layer.Texture );
    }
}

void ActorFile::WriteMesh ( BinaryWriter writer, int nodeId, ActorMesh* pMesh, bool isCollisionMesh, WriteContext& context )
{
    std::string& nodeName = context.Nodes[nodeId]->Name;
    if ( pMesh->SubMeshes.empty () )
        throw (format("Mesh of node %s must have at least one submesh") % nodeName).str ();

    int numTangents = -1;
    int numUvChannels = -1;
    int numColorChannels = -1;
    bool colors32;
    foreach ( ActorSubMesh& subMesh, pMesh->SubMeshes )
    {
        if ( numUvChannels == -1 )
        {
            if ( subMesh.VertexTangents.empty () )
                numTangents = 0;
            else if ( subMesh.VertexBitangents.empty () )
                numTangents = 1;
            else
                numTangents = 2;

            numUvChannels = subMesh.VertexUVs.size ();
            if ( !subMesh.VertexColors32.empty () )
            {
                colors32 = true;
                numColorChannels = subMesh.VertexColors32.size ();
            }
            else
            {
                colors32 = false;
                numColorChannels = subMesh.VertexColors128.size ();
            }
        }
        else
        {
            if ( numTangents == 0 && (!subMesh.VertexTangents.empty() || !subMesh.VertexBitangents.empty()) ||
                 numTangents == 1 && ( subMesh.VertexTangents.empty() || !subMesh.VertexBitangents.empty()) ||
                 numTangents == 2 && ( subMesh.VertexTangents.empty() ||  subMesh.VertexBitangents.empty()) )
            {
                throw (format("Submeshes of node %s have inconsistent tangents") % nodeName).str ();
            }

            if ( subMesh.VertexUVs.size () != numUvChannels )
                throw (format("Submeshes of node %s have inconsistent numbers of UV channels") % nodeName).str ();

            if ( colors32 )
            {
                if ( subMesh.VertexColors32.size () != numColorChannels )
                    throw (format("Submeshes of node %s have inconsistent numbers of color channels") % nodeName).str ();

                if ( !subMesh.VertexColors128.empty () )
                    throw (format("Submeshes of node %s mix 32-bit and 128-bit colors") % nodeName).str ();
            }
            else
            {
                if ( subMesh.VertexColors128.size () != numColorChannels )
                    throw (format("Submeshes of node %s have inconsistent numbers of color channels") % nodeName).str ();

                if ( !subMesh.VertexColors32.empty () )
                    throw (format("Submeshes of node %s mix 32-bit and 128-bit colors") % nodeName).str ();
            }
        }
    }

    writer.Write ( XacChunkHeader ( EMOFX_CHUNK_XAC_MESH, 1 ) );

    XacMeshChunkv1Header meshData;
    meshData.NodeId = nodeId;
    meshData.NumSubMeshes = pMesh->SubMeshes.size ();
    meshData.IsCollisionMesh = isCollisionMesh;
    meshData.NumVertexElements = 3 + numTangents + numUvChannels + numColorChannels;

    meshData.NumVertices = 0;
    meshData.NumIndices = 0;
    meshData.NumInfluenceRanges = 0;
    foreach ( ActorSubMesh& subMesh, pMesh->SubMeshes )
    {
        meshData.NumVertices += subMesh.VertexPositions.size ();
        meshData.NumIndices += subMesh.Indices.size ();
        meshData.NumInfluenceRanges += subMesh.VertexInfluences.size ();
    }

    writer.Write ( meshData );

    // First vertex element: positions
    XacMeshChunkv1VertexElement vertexElem;
    vertexElem.Usage = EMOFX_DECLUSAGE_POSITION;
    vertexElem.ElementSize = sizeof(Vec3D);
    vertexElem.KeepOriginals = true;
    vertexElem.IsScaleFactor = false;
    writer.Write ( vertexElem );
    foreach ( ActorSubMesh& subMesh, pMesh->SubMeshes )
    {
        writer.Write ( subMesh.VertexPositions );
    }

    // Second vertex element: normals
    vertexElem.Usage = EMOFX_DECLUSAGE_NORMAL;
    vertexElem.ElementSize = sizeof(Vec3D);
    vertexElem.KeepOriginals = true;
    vertexElem.IsScaleFactor = false;
    writer.Write ( vertexElem );
    foreach ( ActorSubMesh& subMesh, pMesh->SubMeshes )
    {
        writer.Write ( subMesh.VertexNormals );
    }

    // Third and fourth vertex elements: tangents and bitangents
    if ( numTangents > 0 )
    {
        vertexElem.Usage = EMOFX_DECLUSAGE_TANGENT;
        vertexElem.ElementSize = sizeof(Vec4D);
        vertexElem.KeepOriginals = true;
        vertexElem.IsScaleFactor = false;
        writer.Write ( vertexElem );
        foreach ( ActorSubMesh& subMesh, pMesh->SubMeshes )
        {
            writer.Write ( subMesh.VertexTangents );
        }

        if ( numTangents == 2 )
        {
            writer.Write ( vertexElem );
            foreach ( ActorSubMesh& subMesh, pMesh->SubMeshes )
            {
                writer.Write ( subMesh.VertexBitangents );
            }
        }
    }

    // Fifth vertex element: influence range indices
    vertexElem.Usage = EMOFX_DECLUSAGE_INFLUENCERANGEIDX;
    vertexElem.ElementSize = sizeof(int);
    vertexElem.KeepOriginals = false;
    vertexElem.IsScaleFactor = false;
    writer.Write ( vertexElem );
    for ( int i = 0; i < meshData.NumVertices; ++i )
    {
        writer.Write ( i );
    }

    // Further vertex elements: UV channels
    vertexElem.Usage = EMOFX_DECLUSAGE_TEXCOORD;
    vertexElem.ElementSize = sizeof(Vec2D);
    vertexElem.KeepOriginals = false;
    vertexElem.IsScaleFactor = false;
    for ( int i = 0; i < numUvChannels; ++i )
    {
        writer.Write ( vertexElem );
        foreach ( ActorSubMesh& subMesh, pMesh->SubMeshes )
        {
            writer.Write ( subMesh.VertexUVs[i] );
        }
    }

    // Further vertex elements: color channels
    if ( colors32 )
    {
        vertexElem.Usage = EMOFX_DECLUSAGE_COLOR32;
        vertexElem.ElementSize = sizeof(Color8);
    }
    else
    {
        vertexElem.Usage = EMOFX_DECLUSAGE_COLOR128;
        vertexElem.ElementSize = sizeof(Color);
    }
    vertexElem.KeepOriginals = false;
    vertexElem.IsScaleFactor = false;

    for ( int i = 0; i < numColorChannels; ++i )
    {
        writer.Write ( vertexElem );
        foreach ( ActorSubMesh& subMesh, pMesh->SubMeshes )
        {
            if ( colors32 )
                writer.Write ( subMesh.VertexColors32[i] );
            else
                writer.Write ( subMesh.VertexColors128[i] );
        }
    }

    foreach ( ActorSubMesh& subMesh, pMesh->SubMeshes )
    {
        XacMeshChunkv1SubMesh subMeshData;
        subMeshData.NumVertices = subMesh.VertexPositions.size ();
        subMeshData.NumIndices = subMesh.Indices.size ();
        subMeshData.MaterialId = subMesh.MaterialId;
        subMeshData.NumBones = 0;
        writer.Write ( subMeshData );
        writer.Write ( subMesh.Indices );
    }
}

void ActorFile::WriteBoneInfluences ( BinaryWriter writer, int nodeId, ActorMesh* pMesh, bool isCollisionMesh, WriteContext& context )
{
    std::map < int, bool > boneNodeIds;
    int numInfluences = 0;
    foreach ( ActorSubMesh& subMesh, pMesh->SubMeshes )
    {
        foreach ( std::vector < BoneInfluence >& boneInfluences, subMesh.VertexInfluences )
        {
            numInfluences += boneInfluences.size ();
            foreach ( BoneInfluence& influence, boneInfluences )
            {
                boneNodeIds[context.NodeIndices[influence.Node]] = true;
            }
        }
    }
    if ( numInfluences == 0 )
        return;

    writer.Write ( XacChunkHeader ( EMOFX_CHUNK_XAC_BONE_INFLUENCES, 3 ) );

    XacSkinningChunkv3Header chunkHeader;
    chunkHeader.NodeId = nodeId;
    chunkHeader.NumLocalBones = boneNodeIds.size ();
    chunkHeader.NumInfluences = numInfluences;
    chunkHeader.IsForCollisionMesh = isCollisionMesh;
    writer.Write ( chunkHeader );

    foreach ( ActorSubMesh& subMesh, pMesh->SubMeshes )
    {
        foreach ( std::vector < BoneInfluence >& boneInfluences, subMesh.VertexInfluences )
        {
            foreach ( BoneInfluence& influence, boneInfluences )
            {
                writer.Write ( XacSkinningChunkv3Influence ( context.NodeIndices[influence.Node], influence.Weight ) );
            }
        }
    }

    int influenceOffset = 0;
    foreach ( ActorSubMesh& subMesh, pMesh->SubMeshes )
    {
        foreach ( std::vector < BoneInfluence >& boneInfluences, subMesh.VertexInfluences )
        {
            writer.Write ( XacSkinningChunkv3InfluenceRange ( influenceOffset, boneInfluences.size () ) );
            influenceOffset += boneInfluences.size ();
        }
    }
}

void ActorFile::WriteMorphTargets ( BinaryWriter writer, WriteContext& context )
{
    if ( MorphTargets.empty () )
        return;

    writer.Write ( XacChunkHeader ( EMOFX_CHUNK_XAC_MORPH_TARGETS, 1 ) );
    writer.Write ( MorphTargets.size () );
    writer.Write ( 0 );

    foreach ( ActorMorphTarget& morphTarget, MorphTargets )
    {
        XacMorphTargetsChunkv1MorphTarget morphTargetData;
        morphTargetData.RangeMin = morphTarget.RangeMin;
        morphTargetData.RangeMax = morphTarget.RangeMax;
        morphTargetData.LodLevel = 0;
        morphTargetData.NumDeformations = morphTarget.Deformations.size ();
        morphTargetData.NumTransformations = 0;
        morphTargetData.PhonemeSetBitmask = morphTarget.PhonemeSetBitmask;
        writer.Write ( morphTargetData );
        writer.Write ( morphTarget.Name );

        foreach ( ActorNodeDeformation& deformation, morphTarget.Deformations )
        {
            if ( deformation.VertexIndices.empty () )
                throw (format("Deformation for node %s in morph target %s has no vertices") % deformation.Node->Name % morphTarget.Name).str ();

            if ( deformation.PositionOffsets.size () != deformation.VertexIndices.size () )
                throw (format("Deformation for node %s in morph target %s doesn't have the required number of position offsets") % deformation.Node->Name % morphTarget.Name).str ();

            if ( deformation.NormalOffsets.size () != deformation.VertexIndices.size () )
                throw (format("Deformation for node %s in morph target %s doesn't have the required number of normal offsets") % deformation.Node->Name % morphTarget.Name).str ();

            if ( deformation.TangentOffsets.size () != deformation.VertexIndices.size () )
                throw (format("Deformation for node %s in morph target %s doesn't have the required number of tangent offsets") % deformation.Node->Name % morphTarget.Name).str ();

            float minPosOffset = std::min(deformation.PositionOffsets[0].X, std::min(deformation.PositionOffsets[0].Y, deformation.PositionOffsets[0].Z));
            float maxPosOffset = std::max(deformation.PositionOffsets[0].X, std::max(deformation.PositionOffsets[0].Y, deformation.PositionOffsets[0].Z));
            for ( int i = 1; i < deformation.PositionOffsets.size (); ++i )
            {
                minPosOffset = std::min(minPosOffset, std::min(deformation.PositionOffsets[i].X, std::min(deformation.PositionOffsets[i].Y, deformation.PositionOffsets[i].Z)));
                maxPosOffset = std::max(maxPosOffset, std::max(deformation.PositionOffsets[i].X, std::max(deformation.PositionOffsets[i].Y, deformation.PositionOffsets[i].Z)));
            }

            XacMorphTargetsChunkv1Deformation deformationData;
            deformationData.NodeId = context.NodeIndices[deformation.Node];
            deformationData.MinValue = minPosOffset;
            deformationData.MaxValue = maxPosOffset;
            deformationData.NumVertices = deformation.VertexIndices.size ();
            writer.Write ( deformationData );
            foreach ( Vec3D& posOffset, deformation.PositionOffsets )
            {
                writer.Write (
                    XacDeformVec3D16 (
                        (ushort)( (posOffset.X - minPosOffset) / (maxPosOffset - minPosOffset) * 65535.0f ),
                        (ushort)( (posOffset.Y - minPosOffset) / (maxPosOffset - minPosOffset) * 65535.0f ),
                        (ushort)( (posOffset.Z - minPosOffset) / (maxPosOffset - minPosOffset) * 65535.0f )
                    )
                );
            }
            foreach ( Vec3D& normalOffset, deformation.NormalOffsets )
            {
                writer.Write (
                    XacDeformVec3D8 (
                        (byte)( (normalOffset.X + 1.0f) * 127.5f ),
                        (byte)( (normalOffset.Y + 1.0f) * 127.5f ),
                        (byte)( (normalOffset.Z + 1.0f) * 127.5f )
                    )
                );
            }
            foreach ( Vec3D& tangentOffset, deformation.TangentOffsets )
            {
                writer.Write (
                    XacDeformVec3D8 (
                        (byte)( (tangentOffset.X + 1.0f) * 127.5f ),
                        (byte)( (tangentOffset.Y + 1.0f) * 127.5f ),
                        (byte)( (tangentOffset.Z + 1.0f) * 127.5f )
                    )
                );
            }
            writer.Write ( deformation.VertexIndices );
        }
    }
}

void ActorFile::CollectNodes ( ActorNode* pNode, std::vector < ActorNode* >& nodes )
{
    nodes.push_back ( pNode );
    foreach ( std::shared_ptr < ActorNode > pChildNode, pNode->Children )
    {
        CollectNodes ( pChildNode.get (), nodes );
    }
}

ActorFile::ChunkReader::ChunkReader ()
{
    _handlers.resize ( EMOFX_MAX_CHUNK );

    _handlers[EMOFX_CHUNK_XAC_METADATA].push_back ( nullptr );
    _handlers[EMOFX_CHUNK_XAC_METADATA].push_back ( &ActorFile::ReadMetaDataV2 );

    _handlers[EMOFX_CHUNK_XAC_NODE_HIERARCHY].push_back ( &ActorFile::ReadNodeHierarchyV1 );

    _handlers[EMOFX_CHUNK_XAC_MATERIAL_TOTALS].push_back ( &ActorFile::ReadMaterialTotalsV1 );

    _handlers[EMOFX_CHUNK_XAC_MATERIAL_DEFINITION].push_back ( nullptr );
    _handlers[EMOFX_CHUNK_XAC_MATERIAL_DEFINITION].push_back ( &ActorFile::ReadMaterialDefinitionV2 );

    _handlers[EMOFX_CHUNK_XAC_MESH].push_back ( &ActorFile::ReadMeshV1 );

    _handlers[EMOFX_CHUNK_XAC_BONE_INFLUENCES].push_back ( nullptr );
    _handlers[EMOFX_CHUNK_XAC_BONE_INFLUENCES].push_back ( nullptr );
    _handlers[EMOFX_CHUNK_XAC_BONE_INFLUENCES].push_back ( &ActorFile::ReadBoneInfluencesV3 );

    _handlers[EMOFX_CHUNK_XAC_MORPH_TARGETS].push_back ( &ActorFile::ReadMorphTargetsV1 );
}

bool ActorFile::ChunkReader::ReadChunk ( BinaryReader reader, ActorFile* pActor, ReadContext& context )
{
    if ( reader.IsAtEnd () )
        return false;

    XacChunkHeader chunkHeader = reader.Read < XacChunkHeader > ();
    if ( chunkHeader.Type < 0 || chunkHeader.Type >= _handlers.size () || _handlers[chunkHeader.Type].empty () )
        throw (format("Unrecognized chunk type %d in file") % chunkHeader.Type).str ();

    std::vector < ReadChunk_t >& versionReaders = _handlers[chunkHeader.Type];
    if ( chunkHeader.Version <= 0 || chunkHeader.Version > versionReaders.size () || !versionReaders[chunkHeader.Version - 1] )
        throw (format("Unsupported version %d in chunk %d") % chunkHeader.Version % chunkHeader.Type).str ();

    (pActor->*versionReaders[chunkHeader.Version - 1]) ( reader, context );
    return true;
}
