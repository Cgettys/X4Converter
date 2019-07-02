#include "X4ConverterTools/model/Light.h"
#include <boost/format.hpp>

using namespace boost;
namespace model {
    // TODO composition or subclass isntead of this mess
    Light::Light(std::shared_ptr<ConversionContext> ctx) : AbstractElement(ctx) {

    }

    Light::Light(pugi::xml_node node, std::shared_ptr<ConversionContext> ctx) : AbstractElement(ctx) {
        setName(node.attribute("name").value());
        ReadOffset(node);
        std::string kind = node.value();
        if (kind.compare("arealight") == 0) {
            lightKind = arealight;
        } else if (kind.compare("ommi") == 0) {
            lightKind = omni;
        } else {
            throw std::runtime_error("Unknown light type:" + kind);
        }
        color = ReadAttrRGB(node);
        area = aiVector2D(node.attribute("areax").as_float(0.0f), node.attribute("areay").as_float(0.0f));
        // TODO how to handle these:
        lightEffect = node.attribute("lighteffect").as_bool(false);
        range = node.attribute("range").as_float(0.0f);
        shadowRange = node.attribute("shadowrange").as_float(0.0f);
        radius = node.attribute("radius").as_float(0.0f);
        spotAttenuation = node.attribute("spotattenuation").as_float(0.0f);
        specularIntensity = node.attribute("specularintensity").as_float(0.0f);
        trigger = node.attribute("trigger").as_bool(false);
        intensity = node.attribute("intensity").as_float(0.0f);
    }

    aiNode *Light::ConvertToAiNode() {
        auto result = new aiLight();
        result->mName = getName();
        // TODO axes?
        result->mPosition = offsetPos;

        // NB: only one value in the XML, but duplicated here to make sure it works
        result->mColorDiffuse = color;
        result->mColorSpecular = color;
        result->mColorAmbient = color;
        switch (lightKind) {
            case arealight:
                result->mType = aiLightSource_AREA;
                result->mSize = area;
                result->mUp = aiVector3D(0, 0, 1); // TODO figure this out
                result->mDirection = offsetRot.Rotate(result->mUp); // TODO checkme
                break;
            case omni:
                result->mType = aiLightSource_POINT;
                break;
        }
        ctx->AddLight(result);
        auto node = new aiNode();
        node->mName = getName();
        // TODO other stuff here
        return node;
    }

    void Light::ConvertFromAiNode(aiNode *node) {
        std::string name = node->mName.C_Str();
        setName(name);// TODO template method out this stuff?
        auto light = ctx->GetLight(name);
        offsetPos = light->mPosition;
        color = light->mColorSpecular;// TODO is this the best choice?
        // TODO reconstruct vector for mDirection
        switch (light->mType) {
            case aiLightSource_AREA:
                lightKind = arealight;
                area = light->mSize;
                break;
            case aiLightSource_POINT:
                lightKind = omni;
                break;
            default:
                auto err = str(format("Unknown light type from Assimp: %d") % light->mType);
                throw std::runtime_error(err);
        }
    }

    void Light::ConvertToGameFormat(pugi::xml_node out) {
        WriteOffset(out);
        WriteAttrRGB(out, color);
    }
}