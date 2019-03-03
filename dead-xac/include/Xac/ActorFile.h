#pragma once

class ActorFile
{
public:
    void                    Read                        ( Assimp::IOStream* pStream );
    void                    Write                       ( Assimp::IOStream* pStream );

    ActorNode*              GetNode                     ( const std::string& name );
    ActorMorphTarget*       GetMorphTarget              ( const std::string& name );

    std::vector < std::shared_ptr < ActorNode > >   RootNodes;
    std::vector < ActorMaterial >                   Materials;
    std::vector < ActorMorphTarget >                MorphTargets;
    std::string                                     ActorName;
    std::string                                     OriginalFileName;

private:
    struct ReadContext
    {
        std::vector < ActorNode* >                      Nodes;
        std::map < ActorMesh*, int >                    MeshNumInfluenceRanges;
        std::map < ActorSubMesh*, std::vector<int> >    SubMeshInfluenceRangeIndices;
    };

    struct WriteContext
    {
        std::vector < ActorNode* >                      Nodes;
        std::map < ActorNode*, int >                    NodeIndices;
    };

    XacHeader               ReadHeader                  ( BinaryReader reader );
    void                    ReadMetaDataV2              ( BinaryReader reader, ReadContext& context );
    void                    ReadNodeHierarchyV1         ( BinaryReader reader, ReadContext& context );
    void                    ReadMaterialTotalsV1        ( BinaryReader reader, ReadContext& context );
    void                    ReadMaterialDefinitionV2    ( BinaryReader reader, ReadContext& context );
    void                    ReadMeshV1                  ( BinaryReader reader, ReadContext& context );
    void                    ReadBoneInfluencesV3        ( BinaryReader reader, ReadContext& context );
    void                    ReadMorphTargetsV1          ( BinaryReader reader, ReadContext& context );

    void                    WriteHeader                 ( BinaryWriter writer );
    void                    WriteMetaData               ( BinaryWriter writer );
    void                    WriteNodeHierarchy          ( BinaryWriter writer, WriteContext& context );
    void                    WriteMaterialTotals         ( BinaryWriter writer );
    void                    WriteMaterialDefinition     ( BinaryWriter writer, int materialId );
    void                    WriteMesh                   ( BinaryWriter writer, int nodeId, ActorMesh* pMesh, bool isCollisionMesh, WriteContext& context );
    void                    WriteBoneInfluences         ( BinaryWriter writer, int nodeId, ActorMesh* pMesh, bool isCollisionMesh, WriteContext& context );
    void                    WriteMorphTargets           ( BinaryWriter writer, WriteContext& context );

    void                    CollectNodes                ( ActorNode* pNode, std::vector < ActorNode* >& nodes );

    class ChunkReader
    {
    public:
                                                        ChunkReader         ();

        bool                                            ReadChunk           ( BinaryReader reader, ActorFile* pActor, ReadContext& context );

    private:
        typedef void (ActorFile::*ReadChunk_t)( BinaryReader reader, ReadContext& context );
        std::vector < std::vector < ReadChunk_t > >    _handlers;
    };
    static ChunkReader      _chunkReader;
};
