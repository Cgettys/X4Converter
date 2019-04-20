#pragma once

#include "AbstractElement.h"

namespace model {
    class Lod : public AbstractElement {
    public:
        int getIndex() {
            return index;
        }

        aiNode *ConvertToAiNode() override {
            return new aiNode(name);
        }
    protected:
        int index = -2;
    };

}