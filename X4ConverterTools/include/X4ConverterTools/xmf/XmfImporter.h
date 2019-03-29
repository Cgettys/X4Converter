#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include <X4ConverterTools/util/AssimpUtil.h>
#include <X4ConverterTools/Component.h>
#include <X4ConverterTools/ComponentPart.h>
#include <X4ConverterTools/Material.h>
#include <X4ConverterTools/MaterialLibrary.h>
#include <X4ConverterTools/xmf/XuMeshFile.h>
#include <X4ConverterTools/ani/AnimFile.h>
#include <X4ConverterTools/types/ConversionContext.h>

#include <assimp/IOSystem.hpp>
#include <assimp/ProgressHandler.hpp>
#include <assimp/importerdesc.h>
#include <assimp/Importer.hpp>
#include <assimp/Exceptional.h>
#include <assimp/BaseImporter.h>
#include <assimp/IOSystem.hpp>
#include <assimp/scene.h>
#include <assimp/SceneCombiner.h>

namespace xmf {
    class XmfImporter : public Assimp::BaseImporter {
    public:
        explicit XmfImporter(const std::string &gameBaseFolderPath);

        const aiImporterDesc *GetInfo() const override;

        bool CanRead(const std::string &filePath, Assimp::IOSystem *pIOHandler, bool checkSig) const override;

        void InternReadFile(const std::string &filePath, aiScene *pScene, Assimp::IOSystem *pIOHandler) override;

    private:


        aiNode *ConvertComponentToAiNode(Component &component, ConversionContext &context);

        aiNode *ConvertComponentPartToAiNode(ComponentPart &part, ConversionContext &context);


        std::string _gameBaseFolderPath;
        MaterialLibrary _materialLibrary;

        void AddMaterials(const std::string &filePath, aiScene *pScene, const ConversionContext &context);
    };
}