#pragma once

class XmfImporter : public Assimp::BaseImporter
{
public:
                                    XmfImporter                 ( const std::string& gameBaseFolderPath );

    virtual const aiImporterDesc*   GetInfo                     () const;

    virtual bool                    CanRead                     ( const std::string& filePath, Assimp::IOSystem* pIOHandler, bool checkSig ) const;

protected:
    virtual void                    InternReadFile              ( const std::string& filePath, aiScene* pScene, Assimp::IOSystem* pIOHandler );

private:
    struct ConversionContext
    {
        std::map < std::string, uint >  Materials;
        std::vector < aiMesh* >         Meshes;
    };

    aiNode*                         ConvertComponentToAiNode        ( Component& component, ConversionContext& context );
    aiNode*                         ConvertComponentPartToAiNode    ( ComponentPart& part, ConversionContext& context );
    aiNode*                         ConvertXuMeshToAiNode           ( XuMeshFile& mesh, const std::string& name, ConversionContext& context );
    aiMesh*                         ConvertXuMeshToAiMesh           ( XuMeshFile& mesh, int firstIndex, int numIndices, const std::string& name, ConversionContext& context );    

    void                            AllocSceneMaterials             ( aiScene* pScene, XuMeshFile& meshFile );
    void                            AllocMeshVertices               ( aiMesh* pMesh, XuMeshFile& meshFile, int numVertices );
    void                            AllocMeshFaces                  ( aiMesh* pMesh, XuMeshFile& meshFile, int numIndices );

    void                            PopulateMeshVertices            ( aiMesh* pMesh, XuMeshFile& meshFile, int firstIndex, int numIndices );
    void                            PopulateMeshFaces               ( aiMesh* pMesh, int numIndices );
    
    std::string                     _gameBaseFolderPath;
    MaterialLibrary                 _materialLibrary;
};
