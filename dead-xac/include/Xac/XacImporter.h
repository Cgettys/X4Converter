#pragma once

class XacImporter : public Assimp::BaseImporter
{
public:
                                    XacImporter                 ( const std::string& gameBaseFolderPath );

    virtual const aiImporterDesc*   GetInfo                     () const;

    virtual bool                    CanRead                     ( const std::string& filePath, Assimp::IOSystem* pIOHandler, bool checkSig ) const;

protected:
    virtual void                    InternReadFile              ( const std::string& filePath, aiScene* pScene, Assimp::IOSystem* pIOHandler );

private:
    struct ConversionContext
    {
        ActorFile*                      Actor;
        std::vector < aiMesh* >         Meshes;
        std::vector < aiNode* >         MeshNodes;
        std::map < ActorNode*, std::map < int, aiMesh* > >  SubMeshesByActorNode;
    };

    aiNode*                         ConvertNode                 ( ActorNode* pActorNode, aiNode* pParentAiNode, ConversionContext& context );
    std::vector < aiNode* >         ConvertMesh                 ( ActorNode* pActorNode, ActorMesh* pActorMesh, aiNode* pParentAiNode, const std::string& meshName, ConversionContext& context );
    int                             ConvertSubMesh              ( const std::string& subMeshName, ActorSubMesh* pSubMesh, aiNode* pParentAiNode, ConversionContext& context );
    void                            ConvertMorphTargets         ( ConversionContext& context );    
    void                            ConvertBoneInfluences       ( ActorSubMesh* pSubMesh, aiMesh* pAiMesh, aiNode* pParentAiNode, ConversionContext& context );
    aiMaterial*                     ConvertMaterial             ( ActorMaterial* pActorMaterial, const std::string& modelFolderPath );

    MaterialLibrary                 _materialLibrary;
};
