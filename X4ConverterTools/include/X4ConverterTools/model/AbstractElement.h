//
// Created by cg on 4/14/19.
//

#pragma once

#include <string>
#include <map>
#include <assimp/scene.h>
#include <X4ConverterTools/ConversionContext.h>

namespace model {
    class AbstractElement {
    public:
        explicit AbstractElement(std::shared_ptr<ConversionContext> ctx);


        virtual ~AbstractElement() = default;

        std::string getName();

        void setName(std::string n);

        virtual aiNode *ConvertToAiNode(std::shared_ptr<ConversionContext> ctx) = 0;

        virtual void ConvertFromAiNode(aiNode *node, std::shared_ptr<ConversionContext> ctx) = 0;

        virtual void ConvertToXml(pugi::xml_node out, std::shared_ptr<ConversionContext> ctx) = 0;

    protected:

        // TODO template pattern?
        // TODO refactor me please....
        virtual std::vector<aiNode *> attrToAiNode();

        virtual void populateAiNodeChildren(aiNode *target, std::vector<aiNode *> children);


        virtual void readAiNodeChild(aiNode *source);

        virtual pugi::xml_node Child(pugi::xml_node parent, std::string elementName);

        virtual pugi::xml_node
        ChildByAttr(pugi::xml_node parent, std::string elemName, std::string attrName, std::string attrVal);

        virtual void WriteAttr(pugi::xml_node target, std::string name, std::string val);

        std::map<std::string, std::string> attrs;

        std::shared_ptr<ConversionContext> ctx;
    private:

        std::string name;

        aiNode *GenerateAttrNode(const std::string &key, const std::string &value);

    };


}