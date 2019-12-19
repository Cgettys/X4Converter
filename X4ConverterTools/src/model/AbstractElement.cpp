#include <utility>

#include <X4ConverterTools/model/AbstractElement.h>
#include <vector>
#include <iostream>

namespace model {
using std::string;
namespace xml = util::xml;
AbstractElement::AbstractElement(ConversionContext::Ptr ctx) : ctx(std::move(ctx)) {

}

string AbstractElement::getName() {
  return name;
}

void AbstractElement::setName(string n) {
  // TODO Verify exact limit
  // E.g.
  // "ship_arg_xl_carrier_01|source|assets\\units\\size_xl\\ship_arg_xl_carrier_01_data"
  // cuts off to "ship_arg_xl_carrier_01|source|assets\\units\\size_xl\\ship_arg_xl_"
  // Hard limit is 63? but we need some characters to do attribute stuff
  if (n.length() > 45) {
    throw std::runtime_error("Name too long, blender will cut it off!");
  }
  name = std::move(n);
}


void AiNodeElement::populateAiNodeChildren(aiNode *target, std::vector<aiNode *> children) {
  unsigned long numChildren = children.size();
  if (children.empty()) {
    return;
  }

  auto oldCount = target->mNumChildren;
  auto newCount = oldCount + numChildren;
  auto arr = new aiNode *[newCount];
  auto oldLen = sizeof(aiNode *[oldCount]);

  if (target->mChildren != nullptr) {
    memcpy(arr, target->mChildren, oldLen);
  }
  memcpy(arr + oldCount, children.data(), sizeof(aiNode *[numChildren]));
  target->mNumChildren = newCount;
  auto old = target->mChildren;
  target->mChildren = arr;
  for (auto i = 0UL; i < newCount; i++) {
    auto child = arr[i];
    child->mParent = target;

  }
  delete[] old;
}


void AbstractElement::WriteOffset(pugi::xml_node target) {
  auto offsetNode = xml::AddChild(target, "offset");
  xml::WriteChildXYZ("position", offsetNode, offsetPos);
  xml::WriteRotation(offsetNode, offsetRot);
  xml::RemoveIfChildless(offsetNode);

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

void AbstractElement::ReadOffset(pugi::xml_node target) {
  offsetPos = aiVector3D();
  offsetRot = aiQuaternion();
  // A word to the wise: the XML tends to be listed qx, qy, qz, qw. Why, I do not know.
  // However, most sensible software expects qw, qx, qy, qz
  auto offsetNode = target.child("offset");
  if (offsetNode) {
    auto positionNode = offsetNode.child("position");
    offsetPos = xml::ReadAttrXYZ(positionNode);

    auto quaternionNode = offsetNode.child("quaternion");
    offsetRot = xml::ReadAttrQuat(quaternionNode);
    // TODO check for weird other cases
  }

}
}