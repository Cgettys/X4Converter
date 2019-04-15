//
// Created by cg on 4/14/19.
//

#include "X4ConverterTools/model/AbstractElement.h"

void model::AbstractElement::setName(std::string n) {
    name = n;
}

std::string model::AbstractElement::getName() {
    return name;
}
