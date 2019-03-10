#pragma once
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include <X4ConverterTools/Util/AssimpUtil.h>
#include <X4ConverterTools/Component.h>
#include <X4ConverterTools/ComponentPart.h>
#include <X4ConverterTools/Material.h>
#include <X4ConverterTools/MaterialLibrary.h>
#include <X4ConverterTools/Xmf/XuMeshFile.h>
#include <assimp/scene.h>
#include <assimp/Exceptional.h>
#include <assimp/Exporter.hpp>
class XmfExporter
{
public:
    static Assimp::Exporter::ExportFormatEntry Format;

    static std::string                  GameBaseFolderPath;

private:
    static void                         Export                  ( const char* pFilePath, Assimp::IOSystem* pIOHandler, const aiScene* pScene, const Assimp::ExportProperties* props );
    static void                         ConvertPartNode         ( Component& component, const std::string& parentPartName, const aiScene* pScene, aiNode* pPartNode );
    static void                         ConvertLodNode          ( ComponentPart& part, int lodIndex, const aiScene* pScene, aiNode* pLodNode );
    static void                         ConvertCollisionNode    ( ComponentPart& part, const aiScene* pScene, aiNode* pCollisionNode );
    static std::shared_ptr<XuMeshFile>  ConvertMeshNode         ( const aiScene* pScene, aiNode* pNode, bool isCollisionMesh );
    static void                         CalculatePartSize       ( ComponentPart& part, const aiScene* pScene, aiNode* pCollisionNode );

    static void                         ExtendVertexDeclaration ( aiMesh* pMesh, std::vector<XmfVertexElement>& declaration );
    static void                         ApplyVertexDeclaration  ( std::vector<XmfVertexElement>& declaration, XmfDataBuffer& buffer );
    static int                          WriteVertexElement      ( aiMesh* pMesh, int vertexIdx, XmfVertexElement& elem, byte* pElemData );
};