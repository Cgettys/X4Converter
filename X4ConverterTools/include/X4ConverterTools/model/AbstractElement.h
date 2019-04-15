//
// Created by cg on 4/14/19.
//

#pragma once

#include <string>
#include <assimp/scene.h>

namespace model {
    class AbstractElement {
    public:
        virtual void setName(std::string name);

        virtual std::string getName();

        virtual aiNode *ConvertToAiNode() = 0;

        virtual void ConvertFromAiNode(aiNode *) = 0;

    protected:
        std::string name;
    };


}