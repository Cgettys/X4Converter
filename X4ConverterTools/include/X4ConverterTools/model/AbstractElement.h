//
// Created by cg on 4/14/19.
//

#pragma once

#include <string>
#include <assimp/scene.h>

namespace model {
    class AbstractElement {
    public:
        virtual void setName(std::string n) {
            name = n;
        }

        virtual std::string getName() {
            return name;
        }

        virtual aiNode *ConvertToAiNode() = 0;

        virtual void ConvertFromAiNode(aiNode *node) = 0;

    protected:
        std::string name;
    };


}