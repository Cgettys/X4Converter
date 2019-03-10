#pragma once
#include "pugixml.hpp"
#include <map>
#include <stdexcept>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH
#include <assimp/IOSystem.hpp>
#include <assimp/ProgressHandler.hpp>
#include <assimp/scene.h>
#include <assimp/SceneCombiner.h>
#include <X4ConverterTools/Util/PathUtil.h>
#include <X4ConverterTools/ComponentPart.h>
#include <X4ConverterTools/ComponentPartLod.h>
class Component
{
public:
    static std::shared_ptr<Component>       ReadFromFile            ( const std::string& filePath, const std::string& gameBaseFolderPath, Assimp::IOSystem* pIOHandler );
    void                                    WriteToFile             ( const std::string& filePath, const std::string& gameBaseFolderPath, Assimp::IOSystem* pIOHandler );

    std::string                             Name;
    std::map < std::string, ComponentPart > Parts;

private:
    static pugi::xml_node                   GetComponentNode        ( pugi::xml_document& doc );
    static boost::filesystem::path          GetGeometryFolderPath   ( pugi::xml_node componentNode, const std::string& gameBaseFolderPath, bool createIfMissing );
    
    void                                    ReadPart                ( pugi::xml_node partNode, const boost::filesystem::path& geometryFolderPath, Assimp::IOSystem* pIOHandler );
    
    void                                    CreateDummyFile         ( const std::string& filePath, const std::string& gameBaseFolderPath );
    void                                    WritePart               ( ComponentPart& part, pugi::xml_node connectionsNode, const boost::filesystem::path& geometryFolderPath, Assimp::IOSystem* pIOHandler );
    void                                    WritePartParent         ( ComponentPart& part, pugi::xml_node partNode );
    void                                    WritePartPosition       ( ComponentPart& part, pugi::xml_node partNode );
    void                                    WritePartSize           ( ComponentPart& part, pugi::xml_node partNode );
    void                                    WritePartCenter         ( ComponentPart& part, pugi::xml_node partNode );
    void                                    WritePartLods           ( ComponentPart& part, pugi::xml_node partNode, const boost::filesystem::path& geometryFolderPath, Assimp::IOSystem* pIOHandler );
};
