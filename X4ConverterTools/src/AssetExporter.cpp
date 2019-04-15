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

    } catch (std::exception &e) {
        throw DeadlyExportError(e.what());
    }
}
