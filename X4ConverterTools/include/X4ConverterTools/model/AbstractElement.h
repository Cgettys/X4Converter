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


        void readMultiObjectAttr(aiNode *parent, const std::string &namePart, const std::string &tagPart,
                                 const std::string &valPart);

        virtual void readAiNodeChild(aiNode *parent, aiNode *source);

        void ReadOffset(pugi::xml_node target);

        void ApplyOffsetToAiNode(aiNode *target);

        void WriteOffset(pugi::xml_node target);

        static pugi::xml_node AddChild(pugi::xml_node parent, const std::string &elementName);

        static pugi::xml_node
        AddChildByAttr(pugi::xml_node parent, const std::string &elemName, const std::string &attrName,
                       const std::string &attrVal);


        static aiColor3D ReadAttrRGB(pugi::xml_node target);

        static void WriteAttrRGB(pugi::xml_node target, const aiColor3D &val);

        static aiVector3D ReadAttrXYZ(pugi::xml_node target);

        static void WriteAttrXYZ(pugi::xml_node target, aiVector3D val);

        static aiQuaternion ReadAttrQuat(pugi::xml_node target);

        static void WriteAttrQuat(pugi::xml_node target, aiQuaternion val);

        static void WriteAttr(pugi::xml_node target, const std::string &name, float val);

        static void WriteAttr(pugi::xml_node target, const std::string &name, const std::string &val);

        std::map<std::string, std::string> attrs;

        std::shared_ptr<ConversionContext> ctx;
    protected:

        aiVector3D offsetPos;
        aiQuaternion offsetRot;
    private:

        std::string name;

        void GenerateAttrNode(std::vector<aiNode *> children, const std::string &key, const std::string &value);

    };


}