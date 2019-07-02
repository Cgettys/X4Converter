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

        virtual aiNode *ConvertToAiNode() = 0;

        virtual void ConvertFromAiNode(aiNode *node) = 0;

        virtual void ConvertToGameFormat(pugi::xml_node out) = 0;

    protected:

        // TODO template pattern?
        // TODO refactor me please....
        virtual std::vector<aiNode *> attrToAiNode();

        virtual void populateAiNodeChildren(aiNode *target, std::vector<aiNode *> children);


        virtual void readAiNodeChild(aiNode *source);

        void ReadOffset(pugi::xml_node target);

        void ApplyOffsetToAiNode(aiNode *target);

        void WriteOffset(pugi::xml_node target);

        static pugi::xml_node Child(pugi::xml_node parent, const std::string &elementName);

        static pugi::xml_node
        ChildByAttr(pugi::xml_node parent, const std::string &elemName, const std::string &attrName,
                    const std::string &attrVal);


        static aiColor3D ReadAttrRGB(pugi::xml_node target);

        static void WriteAttrRGB(pugi::xml_node target, aiColor3D val);

        static aiVector3D ReadAttrXYZ(pugi::xml_node target);

        static void WriteAttrXYZ(pugi::xml_node target, aiVector3D val);

        static aiQuaternion ReadAttrQuat(pugi::xml_node target);

        static void WriteAttrQuat(pugi::xml_node target, aiQuaternion val);

        static void WriteAttr(pugi::xml_node target, std::string name, float val);

        static void WriteAttr(pugi::xml_node target, std::string name, std::string val);

        std::map<std::string, std::string> attrs;

        std::shared_ptr<ConversionContext> ctx;
    protected:

        aiVector3D offsetPos;
        aiQuaternion offsetRot;
    private:

        std::string name;

        aiNode *GenerateAttrNode(const std::string &key, const std::string &value);

    };


}