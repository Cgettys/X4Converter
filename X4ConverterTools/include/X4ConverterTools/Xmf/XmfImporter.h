#pragma once
// TODO can we cut this down a bit?
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include <X4ConverterTools/Util/AssimpUtil.h>
#include <X4ConverterTools/Component.h>
#include <X4ConverterTools/ComponentPart.h>
#include <X4ConverterTools/Material.h>
#include <X4ConverterTools/MaterialLibrary.h>
#include <X4ConverterTools/Xmf/XuMeshFile.h>
#include <X4ConverterTools/Ani/AniFile.h>

#include <assimp/IOSystem.hpp>
#include <assimp/ProgressHandler.hpp>
#include <assimp/importerdesc.h>
#include <assimp/Importer.hpp>
#include <assimp/Exceptional.h>
#include <assimp/BaseImporter.h>
#include <assimp/IOSystem.hpp>
#include <assimp/scene.h>
#include <assimp/SceneCombiner.h>

class XmfImporter : public Assimp::BaseImporter {
public:
    XmfImporter(const std::string &gameBaseFolderPath);

    virtual const aiImporterDesc *GetInfo() const;

    virtual bool CanRead(const std::string &filePath, Assimp::IOSystem *pIOHandler, bool checkSig) const;

    virtual void InternReadFile(const std::string &filePath, aiScene *pScene, Assimp::IOSystem *pIOHandler);

private:
    struct ConversionContext {
        std::map<std::string, uint> Materials;
        std::vector<aiMesh *> Meshes;
    };

    aiNode *ConvertComponentToAiNode(Component &component, ConversionContext &context);

    aiNode *ConvertComponentPartToAiNode(ComponentPart &part, ConversionContext &context);

    aiNode *ConvertXuMeshToAiNode(XuMeshFile &mesh, const std::string &name, ConversionContext &context);

    aiMesh *ConvertXuMeshToAiMesh(XuMeshFile &mesh, int firstIndex, int numIndices, const std::string &name,
                                  ConversionContext &context);

    //void                            AllocSceneMaterials             ( aiScene* pScene, XuMeshFile& meshFile );
    void AllocMeshVertices(aiMesh *pMesh, XuMeshFile &meshFile, int numVertices);

    void AllocMeshFaces(aiMesh *pMesh, XuMeshFile &meshFile, int numIndices);

    void PopulateMeshVertices(aiMesh *pMesh, XuMeshFile &meshFile, int firstIndex, unsigned int numIndices);

    void PopulateMeshFaces(aiMesh *pMesh, int numIndices);

    std::string _gameBaseFolderPath;
    MaterialLibrary _materialLibrary;
};
