#include "X4ConverterTools/model/Layer.h"

model::Layer::Layer(std::shared_ptr<ConversionContext> ctx) : AbstractElement(ctx) {

}

model::Layer::Layer(pugi::xml_node node, std::shared_ptr<ConversionContext> ctx) {

}

aiNode *model::Layer::ConvertToAiNode() {
    return nullptr;
}

void model::Layer::ConvertFromAiNode(aiNode *node) {

}

void model::Layer::ConvertToGameFormat(pugi::xml_node out) {

}
