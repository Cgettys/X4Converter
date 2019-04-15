#pragma once

#include <pugixml.hpp>
#include "AbstractElement.h"

namespace model {
    class Connection : AbstractElement{

        Connection(pugi::xml_node node);
    };
}