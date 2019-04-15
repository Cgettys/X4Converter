//
// Created by cg on 4/14/19.
//

#include <iostream>
#include "X4ConverterTools/model/Part.h"

model::Part::Part(pugi::xml_node node) {
    if (std::string(node.name()) != "part"){
        throw std::runtime_error("XML element must be a part element!");
    }
    if (node.attribute("name").empty()){
        throw std::runtime_error("Part must have a name attribute!");
    }
    setName(node.attribute("name").as_string());
}

aiNode *model::Part::ConvertToAiNode() {
    return new aiNode(name);
}

void model::Part::ConvertFromAiNode(aiNode *) {

}
