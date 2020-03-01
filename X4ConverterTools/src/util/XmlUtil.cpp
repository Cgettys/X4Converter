#include <X4ConverterTools/util/XmlUtil.h>

#include <utility>

namespace util {
using std::string;
using pugi::xml_node;

xml_node XmlUtil::AddChild(xml_node &parent, const string &elementName) {
  auto result = parent.child(elementName.c_str());
  if (!result.empty()) {
    return result;
  }
  return parent.append_child(elementName.c_str());
}

xml_node XmlUtil::AddChildByAttr(xml_node &parent,
                                 const string &elemName,
                                 const string &attrName,
                                 const string &attrVal) {
  auto result = parent.find_child_by_attribute(elemName.c_str(), attrName.c_str(), attrVal.c_str());
  if (!result.empty()) {
    return result;
  }
  result = parent.append_child(elemName.c_str());
  result.append_attribute(attrName.c_str()).set_value(attrVal.c_str());
  return result;
}
void XmlUtil::WriteAttr(xml_node &target, const string &name, const string &val) {
  auto attr = target.attribute(name.c_str());
  if (attr.empty()) {
    attr = target.append_attribute(name.c_str());
  }
  attr.set_value(val.c_str());
}

void XmlUtil::WriteAttr(xml_node &target, const string &name, float val) {
  string strVal = FormatUtil::formatFloat(val);
  WriteAttr(target, name, strVal);
}
aiColor3D XmlUtil::ReadAttrRGB(xml_node &target) {
  return aiColor3D(target.attribute("r").as_float() / 255.0f,
                   target.attribute("g").as_float() / 255.0f,
                   target.attribute("b").as_float() / 255.0f);
}

void XmlUtil::WriteAttrRGB(xml_node &target, const aiColor3D &val) {
  WriteAttr(target, "r", val.r * 255.0f);
  WriteAttr(target, "g", val.g * 255.0f);
  WriteAttr(target, "b", val.b * 255.0f);
}
aiVector3D XmlUtil::ReadAttrXYZ(xml_node &target) {
  return aiVector3D(target.attribute("x").as_float(),
                    target.attribute("y").as_float(),
                    target.attribute("z").as_float());
}
void XmlUtil::WriteAttrXYZ(xml_node &target, aiVector3D &val) {
  WriteAttr(target, "x", val.x);
  WriteAttr(target, "y", val.y);
  WriteAttr(target, "z", val.z);
}

void XmlUtil::WriteChildXYZ(const string &name, xml_node &parent, aiVector3D &val, bool write_if_zero) {
  if (!AssimpUtil::IsZero(val) || write_if_zero) {
    auto node = AddChild(parent, name);
    WriteAttrXYZ(node, val);
  } else {
    parent.remove_child(name.c_str());
  }
}

aiQuaternion XmlUtil::ReadAttrQuat(xml_node &target) {
  if (target.empty()) {
    return aiQuaternion();
  } else {
    return aiQuaternion{target.attribute("qw").as_float(),
                        target.attribute("qx").as_float(),
                        target.attribute("qy").as_float(),
                        target.attribute("qz").as_float()};
  }
}

void XmlUtil::WriteAttrQuat(xml_node &target, aiQuaternion &val) {

  // NB: weird XML ordering for consistency with game files
  // TODO is negating all components the right call? leads to consistency with game files
  WriteAttr(target, "qx", val.x);
  WriteAttr(target, "qy", val.y);
  WriteAttr(target, "qz", val.z);
  WriteAttr(target, "qw", val.w);
}

bool XmlUtil::WriteRotation(xml_node &offsetNode, aiQuaternion &val) {
  if (!AssimpUtil::IsZero(val)) {
    auto quatNode = AddChild(offsetNode, "quaternion");
    WriteAttrQuat(quatNode, val);
  } else {
    offsetNode.remove_child("quaternion");
  }
}

bool XmlUtil::HasChildren(xml_node &node) {
  return !node.first_child().empty();
}

void XmlUtil::RemoveIfChildless(xml_node &node) {
  if (!HasChildren(node)) {
    node.parent().remove_child(node);
  }
}

}