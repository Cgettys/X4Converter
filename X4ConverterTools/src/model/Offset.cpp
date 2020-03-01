#include <X4ConverterTools/model/Offset.h>
#include <X4ConverterTools/util/XmlUtil.h>
namespace model {
using util::XmlUtil;
Offset::Offset(pugi::xml_node &source) {
  ReadXml(source);
}
void Offset::ReadXml(pugi::xml_node &source) {
  // A word to the wise: the XML tends to be listed qx, qy, qz, qw. Why, I do not know.
  // However, most sensible software expects qw, qx, qy, qz
  auto offsetNode = source.child("offset");
  if (offsetNode) {
    auto positionNode = offsetNode.child("position");
    offsetPos = XmlUtil::ReadAttrXYZ(positionNode);

    auto quaternionNode = offsetNode.child("quaternion");
    offsetRot = XmlUtil::ReadAttrQuat(quaternionNode);
    // TODO check for weird other cases
  }

}

void Offset::WriteXml(pugi::xml_node &target) {
  auto offsetNode = XmlUtil::AddChild(target, "offset");
  XmlUtil::WriteChildXYZ("position", offsetNode, offsetPos);
  XmlUtil::WriteRotation(offsetNode, offsetRot);
  XmlUtil::RemoveIfChildless(offsetNode);

}
void Offset::ReadAiNode(aiNode *source) {
  // TODO check for scaling and error if found
  source->mTransformation.DecomposeNoScaling(offsetRot, offsetPos);
}

void Offset::WriteAiNode(aiNode *target) {
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
void Offset::ReadAiLight(aiLight *light) {
  offsetPos = light->mPosition;
//  offsetRot = light->mDirection.
//  light->mDirection = offsetRot.Rotate(light->mUp); // TODO checkme
}
void Offset::WriteAiLight(aiLight *light) {
  light->mPosition = offsetPos;
//  light->mDirection =
//      light->mUp =offsetRot.Rotate(light->mUp); // TODO checkme
}
}