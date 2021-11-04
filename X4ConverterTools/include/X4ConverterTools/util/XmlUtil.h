#ifndef X4CONVERTER_X4CONVERTERTOOLS_SRC_UTIL_XMLUTIL_H_
#define X4CONVERTER_X4CONVERTERTOOLS_SRC_UTIL_XMLUTIL_H_
#include <pugixml.hpp>
#include <boost/format.hpp>
#include "FormatUtil.h"
#include "AssimpUtil.h"
#include <X4ConverterTools/util/XmlUtil.h>
#include <X4ConverterTools/types.h>

namespace util {
using std::string;
using pugi::xml_node;
class XmlUtil {
 public:
  static xml_node AddChild(xml_node &parent, const string &elementName);

  static xml_node AddChildByAttr(xml_node &parent,
                                 const string &elemName,
                                 const string &attrName,
                                 const string &attrVal);
  static void WriteAttr(xml_node &target, const string &name, const string &val);
  static void WriteAttr(xml_node &target, const string &name, float val);
  static aiColor3D ReadAttrRGB(xml_node &target);
  static void WriteAttrRGB(xml_node &target, const aiColor3D &val);
  static aiVector3D ReadAttrXYZ(xml_node &target);
  static void WriteAttrXYZ(xml_node &target, aiVector3D &val);

  static void WriteChildXYZ(const string &name, xml_node &parent, aiVector3D &val, bool write_if_zero = false);

  static aiQuaternion ReadAttrQuat(xml_node &target);
  static void WriteAttrQuat(xml_node &target, aiQuaternion &val);

  static void WriteRotation(xml_node &offsetNode, aiQuaternion &val);

  static bool HasChildren(xml_node &node);

  static void RemoveIfChildless(xml_node &node);

};
}
#endif //X4CONVERTER_X4CONVERTERTOOLS_SRC_UTIL_XMLUTIL_H_
