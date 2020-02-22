#include <utility>

#include <X4ConverterTools/model/AbstractElement.h>
#include <vector>
#include <iostream>

namespace model {
using std::string;
using util::XmlUtil;

AbstractElement::AbstractElement(ConversionContext::Ptr ctx) : ctx(std::move(ctx)) {

}

string AbstractElement::getName() {
  return name;
}

void AbstractElement::setName(string n) {
  // E.g.
  // "ship_arg_xl_carrier_01|source|assets\\units\\size_xl\\ship_arg_xl_carrier_01_data"
  // cuts off to "ship_arg_xl_carrier_01|source|assets\\units\\size_xl\\ship_arg_xl_"
  // Hard limit is 63? but we need some characters to do attribute stuff
  if (n.length() > 45) {
    throw std::runtime_error("Name too long, blender will cut it off!");
  }
  name = std::move(n);
}


void AiNodeElement::ApplyOffsetToAiNode(aiNode *target) {
  aiMatrix4x4 tmp(aiVector3D(1, 1, 1), offsetRot, offsetPos);
  // TODO fixme upstream... this sucks
  target->mTransformation.a1 = tmp.a1;
  target->mTransformation.a2 = tmp.a2;
  target->mTransformation.a3 = tmp.a3;
  target->mTransformation.a4 = tmp.a4;
  target->mTransformation.b1 = tmp.b1;
  target->mTransformation.b2 = tmp.b2;
  target->mTransformation.b3 = tmp.b3;
  target->mTransformation.b4 = tmp.b4;
  target->mTransformation.c1 = tmp.c1;
  target->mTransformation.c2 = tmp.c2;
  target->mTransformation.c3 = tmp.c3;
  target->mTransformation.c4 = tmp.c4;
  target->mTransformation.d1 = tmp.d1;
  target->mTransformation.d2 = tmp.d2;
  target->mTransformation.d3 = tmp.d3;
  target->mTransformation.d4 = tmp.d4;

}
AiNodeElement::AiNodeElement(ConversionContext::Ptr ctx) : AbstractElement(std::move(ctx)) {

}

void AbstractElement::CheckXmlNode(pugi::xml_node &src, const std::string expectedName) {
  // Check that the xml_node is not empty
  if (src.empty()) {
    throw std::runtime_error("Could not find expected xml element of type: <" + expectedName + ">");
  }
  // Check we have the correct tag
  std::string actualName = std::string(src.name());
  if (actualName != expectedName) {
    throw std::runtime_error(
        "Received wrong xml element. Expected: <" + expectedName + "> Actual: <" + actualName + ">");
  }
  // Check it is named
  if (!src.attribute("name")) {
    throw std::runtime_error("XML element <" + expectedName + "> must have a name attribute!");
  }
}
void AbstractElement::ReadOffset(pugi::xml_node target) {
  offsetPos = aiVector3D();
  offsetRot = aiQuaternion();
  // A word to the wise: the XML tends to be listed qx, qy, qz, qw. Why, I do not know.
  // However, most sensible software expects qw, qx, qy, qz
  auto offsetNode = target.child("offset");
  if (offsetNode) {
    auto positionNode = offsetNode.child("position");
    offsetPos = XmlUtil::ReadAttrXYZ(positionNode);

    auto quaternionNode = offsetNode.child("quaternion");
    offsetRot = XmlUtil::ReadAttrQuat(quaternionNode);
    // TODO check for weird other cases
  }

}

void AbstractElement::WriteOffset(pugi::xml_node target) {
  auto offsetNode = XmlUtil::AddChild(target, "offset");
  XmlUtil::WriteChildXYZ("position", offsetNode, offsetPos);
  XmlUtil::WriteRotation(offsetNode, offsetRot);
  XmlUtil::RemoveIfChildless(offsetNode);

}
}