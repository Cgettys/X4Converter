#include <X4ConverterTools/AssetExporter.h>

using namespace boost;
using boost::numeric_cast;
using util::DXUtil;

using namespace xmf;
Assimp::Exporter::ExportFormatEntry AssetExporter::Format("xmf", "EgoSoft XuMeshFile exporter", ".xml",
                                                          AssetExporter::Export);

std::string AssetExporter::gameBaseFolderPath = "";
void AssetExporter::Export(const char *pFilePath, Assimp::IOSystem *pIOHandler, const aiScene *pScene,
                           const Assimp::ExportProperties *props) {
    try {
//        aiString tmp;
//        pScene->mMetaData->Get("GameBaseFolderPath", tmp);
//        std::string gameBaseFolderPath(tmp.C_Str());
        if (gameBaseFolderPath.empty()) {
            throw std::runtime_error("GameBaseFolderPath not set");
        }
        if (!pScene->mRootNode || pScene->mRootNode->mNumChildren != 1) {
            throw std::runtime_error(
                    "Scene must have exactly one root node (make sure to remove any lights and cameras)");
        }
        for (int i = 0; i < pScene->mNumMeshes; ++i) {
            AssimpUtil::MergeVertices(pScene->mMeshes[i]);
        }

        aiNode *pComponentNode = pScene->mRootNode->mChildren[0];

        Component component;
        component.Name = pComponentNode->mName.C_Str();
        for (int i = 0; i < pComponentNode->mNumChildren; ++i) {
            ConvertPartNode(component, "", pScene, pComponentNode->mChildren[i]);
        }

        component.WriteToFile(pFilePath, gameBaseFolderPath, pIOHandler);
    } catch (std::exception &e) {
        throw DeadlyExportError(e.what());
    }
}

void AssetExporter::ConvertPartNode(Component &component, const std::string &parentPartName, const aiScene *pScene,
                                    aiNode *pPartNode) {
    std::string partName = pPartNode->mName.C_Str();
    if (!std::regex_match(partName, std::regex("\\w+"))) {
        throw std::runtime_error(
                str(format("Invalid part name %s: must contain only letters, numbers and underscores") % partName));
    }
    if (component.Parts.find(partName) != component.Parts.end())
        throw std::runtime_error(str(format("Duplicate part name %s") % partName));

    Part &part = component.Parts[partName];
    part.Name = partName;
    part.ParentName = parentPartName;
    part.Position = aiVector3D(-pPartNode->mTransformation.a4, pPartNode->mTransformation.b4,
                               pPartNode->mTransformation.c4);

    for (int i = 0; i < pPartNode->mNumChildren; ++i) {
        aiNode *pChildNode = pPartNode->mChildren[i];
        std::string childName = std::string(pChildNode->mName.C_Str());
        std::smatch childNameMatch;

        if (std::regex_match(childName, childNameMatch, std::regex(partName + "Xlod(\\d+)"))) {
            int lodIndex = stoi(childNameMatch[1].str());
            part.PrepareLodNodeForExport(lodIndex, pScene, pChildNode);
        } else if (childName == partName + "Xcollision") {
            part.PrepareCollisionNodeForExport(pScene, pChildNode);
        } else if (std::regex_match(childName, std::regex("\\w+"))) {
            ConvertPartNode(component, partName, pScene, pChildNode);
        } else {
            throw std::runtime_error((format("Cannot determine node type for node %s") % childName).str());
        }
    }
}


