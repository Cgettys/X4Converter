#pragma once

#include "AbstractElement.h"

namespace model {
    class Lod : public AbstractElement {
    public:
        explicit Lod(std::shared_ptr<ConversionContext> ctx) : AbstractElement(ctx) {}

        virtual ~Lod() override = default;
        int getIndex() {
            return index;
        }

    protected:
        int index = -2;
    };

}