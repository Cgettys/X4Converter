#pragma once

class XacExporter
{
public:
    static Assimp::Exporter::ExportFormatEntry Format;

    static std::string                      GameBaseFolderPath;

private:
    static void                             Export                  ( const char* pFilePath, Assimp::IOSystem* pIOHandler, const aiScene* pScene );

    struct SubMeshEntry
    {
                                            SubMeshEntry            () {}
                                            SubMeshEntry            ( ActorMesh* pMesh, int subMeshIndex ) : pActorMesh ( pMesh ), SubMeshIndex ( subMeshIndex ) {}
        
        ActorMesh*                          pActorMesh;
        int                                 SubMeshIndex;
    };

    struct ConversionContext
    {
        const aiScene*                                          Scene;
        ActorFile*                                              Actor;
        std::map < aiMesh*, SubMeshEntry >                      ConvertedSubMeshes;
        std::map < std::string, std::shared_ptr<ActorMesh> >    VisualMeshes;
        std::map < std::string, std::shared_ptr<ActorMesh> >    CollisionMeshes;
        std::map < std::string, int >                           Materials;
    };

    static std::shared_ptr < ActorNode >    ConvertNode             ( aiNode* pAiNode, ConversionContext& context );
    static ActorSubMesh                     ConvertSubMesh          ( aiMesh* pAiMesh, ConversionContext& context );
    static void                             AssignMeshesToNodes     ( ConversionContext& context );
    static void                             ConvertBoneInfluences   ( ConversionContext& context );
    static void                             ConvertMorphTargets     ( ConversionContext& context );
    static void                             ConvertMorphTargets     ( ActorNode* pActorNode, ConversionContext& context );
    static void                             ConvertMorphTarget      ( ActorNode* pActorNode, aiMesh* pBaseAiMesh, aiMesh* pMorphTargetAiMesh, int vertexOffset, ConversionContext& context );
};
