#pragma once

#include "pugixml.hpp"
#include <map>
#include <stdexcept>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#include <assimp/IOSystem.hpp>
#include <X4ConverterTools/util/PathUtil.h>
#include <X4ConverterTools/ComponentPartLod.h>

class ComponentPart {
public:
    ComponentPart() = default;
    explicit ComponentPart(pugi::xml_node partNode,
            const boost::filesystem::path &geometryFolderPath,
            Assimp::IOSystem *pIOHandler);

    ComponentPartLod *GetLod(int lodIndex);

    std::string Name;
    std::string ParentName;
    std::vector<ComponentPartLod> Lods;
    std::shared_ptr<xmf::XuMeshFile> CollisionMesh;
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
