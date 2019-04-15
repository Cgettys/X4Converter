
#pragma once

#include <pugixml.hpp>
#include "AbstractElement.h"

namespace model {

    class Component : AbstractElement{

        Component(pugi::xml_node node);
    };

}
