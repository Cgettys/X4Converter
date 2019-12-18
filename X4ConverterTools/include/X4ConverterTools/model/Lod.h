#pragma once

#include <X4ConverterTools/xmf/XmfFile.h>
#include "AbstractElement.h"

namespace model {
class Lod : public AbstractElement {
 public:
  explicit Lod(ConversionContext::Ptr ctx) : AbstractElement(ctx) {}

  aiNode *ConvertToAiNode(pugi::xml_node &intermediateXml) override {
    if (!ctx->ShouldConvertGeometry()) {
      throw std::runtime_error("Should not be converting in this mode.");
    }
    return xmfFile->ConvertToAiNode(getName());
  }

  ~Lod() override = default;

  int getIndex() {
    return index;
  }

 protected:
  std::shared_ptr<xmf::XmfFile> xmfFile;
  int index = -2;
};

}