#pragma once
// TODO can we cut this down a bit?
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include <X4ConverterTools/util/AssimpUtil.h>
#include <X4ConverterTools/Component.h>
#include <X4ConverterTools/ComponentPart.h>
#include <X4ConverterTools/Material.h>
#include <X4ConverterTools/MaterialLibrary.h>
#include <X4ConverterTools/xmf/XuMeshFile.h>
#include <X4ConverterTools/ani/AniFile.h>
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
        XmfImporter(const std::string &gameBaseFolderPath);

        virtual const aiImporterDesc *GetInfo() const;

        virtual bool CanRead(const std::string &filePath, Assimp::IOSystem *pIOHandler, bool checkSig) const;

        virtual void InternReadFile(const std::string &filePath, aiScene *pScene, Assimp::IOSystem *pIOHandler);

    private:


        aiNode *ConvertComponentToAiNode(Component &component, ConversionContext &context);

        aiNode *ConvertComponentPartToAiNode(ComponentPart &part, ConversionContext &context);


        std::string _gameBaseFolderPath;
        MaterialLibrary _materialLibrary;

        void AddMaterials(const std::string &filePath, aiScene *pScene, const ConversionContext &context);
    };
}