#pragma once

#include "ComponentPartLod.h"

class ComponentPart {
public:
    ComponentPart() = default;
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
    explicit ComponentPart(pugi::xml_node partNode,
            const boost::filesystem::path &geometryFolderPath,
            Assimp::IOSystem *pIOHandler);
#pragma clang diagnostic pop

    ComponentPartLod *GetLod(int lodIndex);

    std::string Name;
    std::string ParentName;
    std::vector<ComponentPartLod> Lods;
    std::shared_ptr<XuMeshFile> CollisionMesh;
    aiVector3D Position;
    aiVector3D Center;
    aiVector3D Size;

    void
    WritePart(pugi::xml_node connectionsNode, const boost::filesystem::path &geometryFolderPath,
              Assimp::IOSystem *pIOHandler);

protected:
    void WritePartParent(pugi::xml_node partNode);

    void WritePartPosition(pugi::xml_node partNode);

    void WritePartSize(pugi::xml_node partNode);

    void WritePartCenter(pugi::xml_node partNode);

    void WritePartLods(pugi::xml_node partNode, const boost::filesystem::path &geometryFolderPath,
                       Assimp::IOSystem *pIOHandler);
};
