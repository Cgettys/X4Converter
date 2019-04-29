#pragma once

#include "AbstractElement.h"

namespace model {
    class Lod : public AbstractElement {
    public:
        virtual ~Lod() = default;
        int getIndex() {
            return index;
        }

    protected:
        int index = -2;
    };

}