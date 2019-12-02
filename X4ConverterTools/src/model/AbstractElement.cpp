#include <utility>

#include <X4ConverterTools/model/AbstractElement.h>
#include <vector>
#include <iostream>
#include <boost/numeric/conversion/cast.hpp>

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

std::vector<aiNode *> AbstractElement::attrToAiNode() {
  std::vector<aiNode *> children;
  for (const auto &attr : attrs) {
    GenerateAttrNode(children, attr.first, attr.second);
  }
  return children;
}

void AbstractElement::populateAiNodeChildren(aiNode *target, std::vector<aiNode *> children) {
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

void AbstractElement::readMultiObjectAttr(aiNode *parent, const string &namePart, const string &tagPart,
                                          const string &valPart) {
  //source.
  std::cout << "Re-assembling multi-object attribute: " << namePart << "|" << tagPart << std::endl;
  auto count = std::atoi(valPart.substr(1).c_str());
  string val;
  for (auto i = 1; i <= count + 1; i++) {
    std::stringstream ss;
    ss << namePart << "|" << tagPart << "#" << std::to_string(i) << "|";
    auto soughtName = ss.str();
    for (auto j = 0; j < parent->mNumChildren; j++) {
      auto child = parent->mChildren[j];
      std::string childName = child->mName.C_Str();
      if (childName.find(soughtName) != string::npos) {
        if (i != count + 1) {
          // Error because we don't know how many extra parts there are
          std::stringstream err;
          err << "Found extra part #" << std::to_string(i);
          err << "For name: " << namePart;
          err << " and key: " << tagPart;
          throw std::runtime_error(err.str());
        }
        val += childName.substr(soughtName.size());
        break;
      }
    }

    if (i != count + 1) {
      // Warning because they may have accidentally deleted something
      std::cerr << "Could not find part #" << i << "For name: " << namePart << " and key: " << tagPart
                << std::endl;
    }
  }
}

void AbstractElement::readAiNodeChild(aiNode *parent, aiNode *source) {
  string raw = source->mName.C_Str();
  auto firstSplit = raw.find('|');
  auto secondSplit = raw.rfind('|');
  if (firstSplit == secondSplit || firstSplit == string::npos || secondSplit == string::npos) {
    std::cerr << "warning, could not read node: " << raw << std::endl;
    return;
  }
  auto namePart = raw.substr(0, firstSplit);
  auto tagPart = raw.substr(firstSplit + 1, secondSplit - firstSplit - 1);
  auto valPart = raw.substr(secondSplit + 1);
  if (namePart != name) {
    std::cerr << "Warning, name of element was " + name + " but tag was for name: " + namePart << std::endl;
  }

  bool tagContainsPound = tagPart.find('#') != string::npos;
  bool valContainsPound = valPart.find('#') != string::npos;
  if (!tagContainsPound && !valContainsPound) {
    attrs[tagPart] = valPart;
  }
  if (tagContainsPound && !valContainsPound) {
    readMultiObjectAttr(parent, namePart, tagPart, valPart);
  }
  if (!tagContainsPound && valContainsPound) {
    std::cout << "Part of multi-object: " << raw << std::endl;
  }

  if (tagContainsPound && valContainsPound) {
    std::cerr << "Likely Malformed tag: " << raw << std::endl;
  }
}

void AbstractElement::GenerateAttrNode(std::vector<aiNode *> &children, const string &key, const string &value) {
  auto rep = name + "|" + key + "|" + value;
  // TODO make magic numbers constants
  if (rep.size() > 63) {
    if (name.size() + key.size() > 55) {
      throw std::runtime_error("Cannot handle name: " + name + "key: " + key + ", too long to split up!");
    }
    std::cout << "Forming multi-object attribute, Name: " << name << " Key: " << key << std::endl;
    auto base = name + "|" + key;
    auto piece_len = 60 - base.size();
    int piece_count = ceil(boost::numeric_cast<double>(value.size()) / piece_len);
    auto flag_attr = base + "|#" + std::to_string(piece_count);
    if (flag_attr.size() > 63) {
      throw std::runtime_error("Flag attr string too long to import into blender!");
    }
    children.emplace_back(new aiNode(flag_attr));
    for (int i = 0; i < piece_count; i++) {
      auto piece = value.substr(i * piece_len, piece_len);
      std::stringstream ss;
      ss << base << "#" << i << "|" << piece;
      auto result = ss.str();
      if (result.size() > 63) {
        throw std::runtime_error("Piece string too long to import into blender!");
      }
      children.emplace_back(new aiNode(result));
    }
  } else {
    children.emplace_back(new aiNode(rep));
  }
}

void AbstractElement::WriteOffset(pugi::xml_node target) {
  auto offsetNode = xml::AddChild(target, "offset");
  xml::WriteChildXYZ("position", offsetNode, offsetPos);
  xml::WriteRotation(offsetNode, offsetRot);
  xml::RemoveIfChildless(offsetNode);

}

void AbstractElement::ApplyOffsetToAiNode(aiNode *target) {
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