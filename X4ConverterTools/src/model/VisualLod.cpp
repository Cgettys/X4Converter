#include "X4ConverterTools/model/VisualLod.h"

#include <utility>

namespace model {
using util::XmlUtil;
VisualLod::VisualLod(ConversionContext::Ptr ctx) : Lod(std::move(ctx)) {}

VisualLod::VisualLod(int index, std::string partName, const ConversionContext::Ptr &ctx) : Lod(ctx) {
  this->index = index;
  std::string tmp = str(boost::format("%1%-lod%2%") % partName % index);
  setName(tmp);

  if (ctx->ShouldConvertGeometry()) {
    xmfFile = xmf::XmfFile::ReadFromFile(tmp + ".xmf", ctx);
  }
}

void VisualLod::ConvertFromAiNode(aiNode *node) {
  AbstractElement::ConvertFromAiNode(node);
  // Parse out the index
  size_t pos = getName().rfind("-lod");
  if (pos == std::string::npos) {
    throw std::runtime_error("lod lacks index");
  }
  index = std::stoi(getName().substr(pos + 4));
  if (ctx->ShouldConvertGeometry()) {
    xmfFile = xmf::XmfFile::GenerateMeshFile(ctx, node, false);
  }
}

void VisualLod::ConvertToGameFormat(pugi::xml_node &out) {
  if (std::string(out.name()) != "lods") {
    throw std::runtime_error("Lods must be added to a lods node");
  }
  auto lodNode = out.find_child_by_attribute("lod", "index", std::to_string(index).c_str());
  if (lodNode.empty()) {
    auto previousLod = out.find_child_by_attribute("lod", "index", std::to_string(index - 1).c_str());
    auto nextLod = out.find_child_by_attribute("lod", "index", std::to_string(index + 1).c_str());
    // Ensure correct ordering
    if (!previousLod.empty()) {
      lodNode = out.insert_child_after("lod", previousLod);
    } else if (!nextLod.empty()) {
      lodNode = out.insert_child_before("lod", nextLod);
    } else {
      lodNode = out.append_child("lod");
    }
    XmlUtil::WriteAttr(lodNode, "index", std::to_string(index));
    auto matsNode = XmlUtil::AddChild(lodNode, "materials");
    // TODO write out xmfFile
    int matIdx = 1;
    for (auto mat : xmfFile->GetMaterials()) {
      // TODO ordering
      // TODO debug source of uninit.
      auto matNode = XmlUtil::AddChildByAttr(matsNode, "material", "id", std::to_string(matIdx++));
      std::string matName{mat.Name.data()};
      std::cout << matName << std::endl;
      XmlUtil::WriteAttr(matNode, "ref", matName);
    }
  }

  if (ctx->ShouldConvertGeometry()) {
    xmfFile->WriteToFile(ctx->fsUtil->GetOutputPath(getName() + ".xmf"));
  }
}

bool VisualLod::MatchesPattern(std::string name) {
  if (!matchesQualifier(name, Lod::Qualifier)) {
    return false;
  }
  std::regex lodRegex("[[]Lo[]][^-]+\\-lod\\d");
  return regex_match(name, lodRegex);
}
}