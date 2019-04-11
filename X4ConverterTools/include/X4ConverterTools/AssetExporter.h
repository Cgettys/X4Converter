#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <X4ConverterTools/util/AssimpUtil.h>
#include <X4ConverterTools/Component.h>
#include <X4ConverterTools/ComponentPart.h>
#include <X4ConverterTools/Material.h>
#include <X4ConverterTools/MaterialLibrary.h>
#include <X4ConverterTools/xmf/XuMeshFile.h>
#include <assimp/scene.h>
#include <assimp/Exceptional.h>
#include <assimp/Exporter.hpp>

class AssetExporter {
public:
    static Assimp::Exporter::ExportFormatEntry Format;

    static std::string gameBaseFolderPath;


    static void Export(const char *pFilePath, Assimp::IOSystem *pIOHandler, const aiScene *pScene,
                       const Assimp::ExportProperties *props);

protected:
    static void
    ConvertPartNode(Component &component, const std::string &parentPartName, const aiScene *pScene, aiNode *pPartNode);


};
