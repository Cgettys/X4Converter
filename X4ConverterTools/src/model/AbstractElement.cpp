#include <utility>

#include <X4ConverterTools/model/AbstractElement.h>
#include <iostream>
#include <X4ConverterTools/util/XmlUtil.h>

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

bool AbstractElement::hasAttr(const std::string &n) {
  if (name.empty()) {
    throw std::runtime_error("Name is not yet set!");
  }
  return ctx->metadata->HasAttribute(name, n);
}
string AbstractElement::getAttr(const string &n) {
  if (name.empty()) {
    throw std::runtime_error("Name is not yet set!");
  }
  return ctx->metadata->GetAttribute(name, n);
}

void AbstractElement::setAttr(const string &n, const string &value, bool overwrite) {
  if (name.empty()) {
    throw std::runtime_error("Name is not yet set!");
  }
  if (!overwrite && hasAttr(n)) {
    throw std::runtime_error(
        "Attribute " + n + " already set for " + getName() + " and this attribute should not be overwritten!");
  }

  ctx->metadata->SetAttribute(name, n, value);
}

void AbstractElement::CheckXmlElement(pugi::xml_node &src, const std::string &expectedName, bool nameRequired) {
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
  if (nameRequired) {
    auto nameAttr = src.attribute("name");
    if (!nameAttr) {
      throw std::runtime_error("XML element <" + expectedName + "> must have a name attribute!");
    } else {
      setName(nameAttr.value());
    }
  }
}

void AbstractElement::ProcessAttributes(pugi::xml_node &node,
                                        const std::function<void(AbstractElement *, std::string, std::string)> &func) {
  for (auto attr: node.attributes()) {
    std::string attrName = attr.name();
    if (attrName == "name") {
      if (getName().empty()) {
        throw std::runtime_error("Name was not set before processing attributes");
      }
    } else {
      std::invoke(func, this, attr.name(), attr.value());
    }
  }
}

void AbstractElement::DefaultProcessAttribute(const std::string &n, const std::string &value) {
  setAttr(n, value);
}

void AbstractElement::WriteAttrs(pugi::xml_node &node) {
  WriteAttrs(node, [](auto) -> bool { return true; });
}
void AbstractElement::WriteAttrs(pugi::xml_node &node, const std::function<bool(std::string)> &predicate) {
  for (auto &entry : ctx->metadata->GetMetadata(name)) {
    std::string attrName = entry.first;
    std::string attrValue = entry.second;
    if (attrName == "name") {
      // Should be used to construct the node or not at all, so skip it
      continue;
    }
    if (predicate(attrName)) {
      XmlUtil::WriteAttr(node, attrName, attrValue);
    }
  }
}
std::vector<aiNode *> AbstractElement::getChildren(aiNode *node) {
  std::vector<aiNode *> children;
  children.reserve(node->mNumChildren);
  for (int i = 0; i < node->mNumChildren; ++i) {
    children.push_back(node->mChildren[i]);
  }
  return children;
}

}