#ifndef X4CONVERTER_X4CONVERTERTOOLS_SRC_UTIL_XMLUTIL_H_
#define X4CONVERTER_X4CONVERTERTOOLS_SRC_UTIL_XMLUTIL_H_
#include <pugixml.hpp>
#include <boost/format.hpp>
#include "FormatUtil.h"
#include "AssimpUtil.h"
namespace util::xml {
using std::string;
using pugi::xml_node;
static xml_node AddChild(xml_node &parent, const string &elementName) {
  auto result = parent.child(elementName.c_str());
  if (!result.empty()) {
    return result;
  }
  return parent.append_child(elementName.c_str());
}

static xml_node
AddChildByAttr(xml_node &parent, const string &elemName, const string &attrName, const string &attrVal) {
  auto result = parent.find_child_by_attribute(elemName.c_str(), attrName.c_str(), attrVal.c_str());
  if (!result.empty()) {
    return result;
  }
  result = parent.append_child(elemName.c_str());
  result.append_attribute(attrName.c_str()).set_value(attrVal.c_str());
  return result;
}
static void WriteAttr(xml_node &target, const string &name, const string &val) {
  auto attr = target.attribute(name.c_str());
  if (attr.empty()) {
    attr = target.append_attribute(name.c_str());
  }
  attr.set_value(val.c_str());
}

static void WriteAttr(xml_node &target, const string &name, float val) {
  string strVal = FormatUtil::formatFloat(val);
  WriteAttr(target, name, strVal);
}

static aiColor3D ReadAttrRGB(xml_node &target) {
  return aiColor3D(target.attribute("r").as_float(),
                   target.attribute("g").as_float(),
                   target.attribute("b").as_float());
}

static void WriteAttrRGB(xml_node &target, const aiColor3D &val) {
  WriteAttr(target, "r", val.r);
  WriteAttr(target, "g", val.g);
  WriteAttr(target, "b", val.b);
}
static aiVector3D ReadAttrXYZ(xml_node &target) {
  return aiVector3D(target.attribute("x").as_float(),
                    target.attribute("y").as_float(),
                    target.attribute("z").as_float());
}
static void WriteAttrXYZ(xml_node &target, aiVector3D &val) {
  WriteAttr(target, "x", val.x);
  WriteAttr(target, "y", val.y);
  WriteAttr(target, "z", val.z);
}

static void WriteChildXYZ(const string &name, xml_node &parent, aiVector3D &val, bool write_if_zero = false) {
  if (!AssimpUtil::IsZero(val) || write_if_zero) {
    auto node = xml::AddChild(parent, name);
    xml::WriteAttrXYZ(node, val);
  } else {
    parent.remove_child(name.c_str());
  }
}

static aiQuaternion ReadAttrQuat(xml_node &target) {
  if (target.empty()) {
    return aiQuaternion();
  } else {
    return aiQuaternion{target.attribute("qw").as_float(),
                        target.attribute("qx").as_float(),
                        target.attribute("qy").as_float(),
                        target.attribute("qz").as_float()};
  }
}
static void WriteAttrQuat(xml_node &target, aiQuaternion &val) {
  // NB: weird XML ordering for consistency with game files
  WriteAttr(target, "qx", val.x);
  WriteAttr(target, "qy", val.y);
  WriteAttr(target, "qz", val.z);
  WriteAttr(target, "qw", val.w);
}

static bool WriteRotation(xml_node &offsetNode, aiQuaternion &val) {
  if (!AssimpUtil::IsZero(val)) {
    auto quatNode = AddChild(offsetNode, "quaternion");
    WriteAttrQuat(quatNode, val);
  } else {
    offsetNode.remove_child("quaternion");
  }
}

static bool HasChildren(xml_node &node) {
  return !node.first_child().empty();
}

static void RemoveIfChildless(xml_node &node) {
  if (!HasChildren(node)) {
    node.parent().remove_child(node);
  }
}

}

#endif //X4CONVERTER_X4CONVERTERTOOLS_SRC_UTIL_XMLUTIL_H_
