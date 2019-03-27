#include <X4ConverterTools/Ani/AniAnimDesc.h>

namespace algo = boost::algorithm;
using namespace boost;
using namespace Assimp;

// TODO copy constructors?
AniAnimDesc::AniAnimDesc(StreamReaderLE &reader) {
    for (char &c : Name) {
        reader >> c;

    }

    for (char &c : SubName) {
        reader >> c;
    }

    for (int i = 0; i < 64; i++) {
        char c = Name[i];
        if (c == ' ') {
            throw std::runtime_error("Did not expect spaces in names");
        } else if (c == '\0') {
            SafeName.insert(i, 1, ' ');
        } else {
            SafeName.insert(i, 1, c);
        }
    }
    algorithm::trim_right(SafeName);

    for (int i = 0; i < 64; i++) {
        char c = SubName[i];
        if (c == ' ') {
            throw std::runtime_error("Did not expect spaces in names");
        } else if (c == '\0') {
            SafeSubName.insert(i, 1, ' ');
        } else {
            SafeSubName.insert(i, 1, c);
        }
    }
    algorithm::trim_right(SafeSubName);

    reader >> NumPosKeys;
    reader >> NumRotKeys;
    reader >> NumScaleKeys;
    reader >> NumPreScaleKeys;
    reader >> NumPostScaleKeys;
    reader >> Duration;
    for (int &i : Padding) {
        reader >> i;
    }


}

void AniAnimDesc::read_frames(StreamReaderLE &reader) {
    for (int i = 0; i < NumPosKeys; i++) {
        posKeys.emplace_back(reader);
    }
    for (int i = 0; i < NumRotKeys; i++) {
        rotKeys.emplace_back(reader);
    }
    for (int i = 0; i < NumScaleKeys; i++) {
        scaleKeys.emplace_back(reader);
    }
    for (int i = 0; i < NumPreScaleKeys; i++) {
        preScaleKeys.emplace_back(reader);
    }
    for (int i = 0; i < NumPostScaleKeys; i++) {
        postScaleKeys.emplace_back(reader);
    }
}

std::string AniAnimDesc::validate() {
    bool valid = true;
    std::string ret = "\nAnimationDesc: \n";


    ret.append(str(format("\tName: %1%\n\tSubName: %2%\n") % SafeName % SafeSubName));


    // TODO better indentation
    //TODO check expected # of frames = actual length of array
    ret.append(str(format(
            "\tNumPosKeys: %1%\n\tNumRotKeys: %2%\n\tNumScaleKeys: %3%\n\tNumPreScaleKeys: %4%\n\tNumPostScaleKeys: %5%\n\tDuration: %6%\n") %
                   NumPosKeys % NumRotKeys % NumScaleKeys % NumPreScaleKeys % NumPostScaleKeys % Duration));
    for (int &i : Padding) {
        if (i != 0) {
            valid = false;
            ret.append("PADDING MUST BE 0");
        }
    }
    ret.append("Position Keyframes:\n");
    for (int i = 0; i < NumPosKeys; i++) {
        try {
            ret.append(posKeys[i].validate());
        }
        catch (std::exception &e) {
            ret.append(e.what());
            valid = false;
        }
    }

    ret.append("Rotation Keyframes:\n");
    for (int i = 0; i < NumRotKeys; i++) {
        try {
            ret.append(rotKeys[i].validate());
        }
        catch (std::exception &e) {
            ret.append(e.what());
            valid = false;
        }
    }

    ret.append("Scale Keyframes:\n");
    for (int i = 0; i < NumScaleKeys; i++) {
        try {
            ret.append(scaleKeys[i].validate());
        }
        catch (std::exception &e) {
            ret.append(e.what());
            valid = false;
        }
    }

    ret.append("Prescale Keyframes:\n");
    for (int i = 0; i < NumPreScaleKeys; i++) {
        try {
            ret.append(preScaleKeys[i].validate());
        }
        catch (std::exception &e) {
            ret.append(e.what());
            valid = false;
        }
    }

    ret.append("PostScale Keyframes:\n");
    for (int i = 0; i < NumPostScaleKeys; i++) {
        try {
            ret.append(postScaleKeys[i].validate());
        }
        catch (std::exception &e) {
            ret.append(e.what());
            valid = false;
        }
    }
    if (NumPreScaleKeys || NumPostScaleKeys) {
        ret.append("WARNING: Pre/Post scale keyframes are not yet supported by this converter");
    }
    if (NumPreScaleKeys != NumPostScaleKeys) {
        ret.append("NumPreScaleKeys Must equal NumPostScaleKeys (Don't ask me why - the game says so)");
        valid = false;
    }
    if (!valid) {
        throw std::runtime_error(ret);
    }
    return ret;
}

void AniAnimDesc::WriteAnim(pugi::xml_node tgtNode) const {
    std::string keys[] = {"location", "rotation_euler", "scale"};
    std::string axes[] = {"X", "Y", "Z"};
    for (std::string &key : keys) {
        for (std::string &axis: axes) {
            WriteChannel(tgtNode, key, axis);
        }
    }
}

void AniAnimDesc::WriteChannel(pugi::xml_node tgtNode, std::string keyType, std::string axis) const {

    std::vector<AniKeyframe> frames;
    if (keyType == "location") {
        frames = posKeys;
    } else if (keyType == "rotation_euler") {
        frames = rotKeys;
    } else if (keyType == "scale") {
        frames = scaleKeys;
    } else {
        // TODO error
        std::cerr << "YOU FOOL!" << std::endl;
    }
    //TODO refactor plz
    std::vector<std::string> namePortions;
    algo::split(namePortions, SafeName, is_any_of(" "));
    std::string namePortion = namePortions[0];
    std::string animKey = str(format("%1%_%2%_%3%") % namePortion % keyType % axis);
    pugi::xml_node animRoot = tgtNode.append_child("animation");
    animRoot.append_attribute("id").set_value(animKey.c_str());

    WriteInputElement(animKey, animRoot, frames, axis);
    WriteOutputElement(animKey, animRoot, frames, axis);
    WriteInterpolationElement(animKey, animRoot, frames, axis);
    WriteInTangentElement(animKey, animRoot, frames);
    WriteOutTangentElement(animKey, animRoot, frames);

    WriteSamplerElement(animKey, animRoot);
    animRoot.find_child_by_attribute("channel", "source", ("#" + animKey + "-sampler").c_str()).append_attribute(
            "target").set_value((namePortion + "/" + keyType + "." + axis).c_str());

}


void AniAnimDesc::WriteInputElement(std::string &animKey, pugi::xml_node &animRoot, std::vector<AniKeyframe> &frames,
                                    std::string &axis) const {
    pugi::xml_node inputNode = animRoot.append_child("source");
    inputNode.append_attribute("id").set_value((animKey + "-input").c_str());
    auto inputArrayKey = (animKey + "-input-array");
    pugi::xml_node inputArrayNode = inputNode.append_child("float_array");
    inputArrayNode.append_attribute("id").set_value(inputArrayKey.c_str());
    inputArrayNode.append_attribute("count").set_value(frames.size());

    std::string value;
    for (auto f : frames) {
        value += f.getValueByAxis(axis);
        value += " ";
    }
    trim_right(value);
    inputArrayNode.set_value(value.c_str());

    pugi::xml_node inputAccessor = inputNode.child("technique_common").child("accessor");
    inputAccessor.append_attribute("source").set_value(("#" + inputArrayKey).c_str());
    inputAccessor.append_attribute("count").set_value(2);
    inputAccessor.append_attribute("stride").set_value(1);
    pugi::xml_node inputParam = inputAccessor.child("param");
    inputParam.append_attribute("name").set_value("TIME");
    inputParam.append_attribute("type").set_value("float");
}


void AniAnimDesc::WriteOutputElement(std::string &animKey, pugi::xml_node &animRoot, std::vector<AniKeyframe> &frames,
                                     std::string &axis) const {
    pugi::xml_node outputNode = animRoot.append_child("source");
    outputNode.append_attribute("id").set_value((animKey + "-output").c_str());
    auto outputArrayKey = (animKey + "-output-array");
    pugi::xml_node outputArrayNode = outputNode.append_child("float_array");
    outputArrayNode.append_attribute("id").set_value(outputArrayKey.c_str());
    outputArrayNode.append_attribute("count").set_value(frames.size());

    //TODO outputArrayNode.

    pugi::xml_node outputAccessor = outputNode.child("technique_common").child("accessor");
    outputAccessor.append_attribute("source").set_value(("#" + outputArrayKey).c_str());
    outputAccessor.append_attribute("count").set_value(2);
    outputAccessor.append_attribute("stride").set_value(1);
    pugi::xml_node outputParam = outputAccessor.child("param");
    outputParam.append_attribute("name").set_value(axis.c_str());
    outputParam.append_attribute("type").set_value("float");
}

void
AniAnimDesc::WriteInterpolationElement(std::string &animKey, pugi::xml_node &animRoot, std::vector<AniKeyframe> &frames,
                                       std::string &axis) const {
    pugi::xml_node interpolationNode = animRoot.append_child("source");
    interpolationNode.append_attribute("id").set_value(
            (animKey + "-interpolation").c_str());
    auto interpolationArrayKey = (animKey + "-interpolation-array");
    pugi::xml_node interpolationArrayNode = interpolationNode.append_child("Name_array");
    interpolationArrayNode.append_attribute("id").set_value(
            interpolationArrayKey.c_str());
    interpolationArrayNode.append_attribute("count").set_value(frames.size());

    std::string value;
    for (auto f : frames) {
        InterpolationType interp = f.getInterpByAxis(axis);
        if (AniKeyframe::checkInterpolationType(interp)) {
            value += AniKeyframe::getInterpolationTypeName(interp);
        } else {
            throw std::runtime_error("Unsupported interpolation type, Cannot encode given interpolation type to .dae!");
        }
        value += " ";
    }
    trim_right(value);
    interpolationArrayNode.set_value(value.c_str());

    pugi::xml_node interpolationAccessor = interpolationNode.child("technique_common").child("accessor");
    interpolationAccessor.append_attribute("source").set_value(("#" + interpolationArrayKey).c_str());
    interpolationAccessor.append_attribute("count").set_value(2);
    interpolationAccessor.append_attribute("stride").set_value(1);

    pugi::xml_node interpolationParam = interpolationAccessor.append_child("param");
    interpolationParam.append_attribute("name").set_value("INTERPOLATION");
    interpolationParam.append_attribute("type").set_value("name");
}


void AniAnimDesc::WriteInTangentElement(std::string &animKey, pugi::xml_node &animRoot,
                                        std::vector<AniKeyframe> &frames) const {
    pugi::xml_node inTangentNode = animRoot.append_child("source");
    inTangentNode.append_attribute("id").set_value((animKey + "-intangent").c_str());
    auto inTangentArrayKey = (animKey + "-intangent-array");
    pugi::xml_node inTangentArrayNode = inTangentNode.append_child("float_array");
    inTangentArrayNode.append_attribute("id").set_value(
            inTangentArrayKey.c_str());
    inTangentArrayNode.append_attribute("count").set_value(2 * frames.size());
    //TODO inTangentArrayNode.

    pugi::xml_node inTangentAccessor = inTangentNode.child("technique_common").child("accessor");
    inTangentAccessor.append_attribute("source").set_value(("#" + inTangentArrayKey).c_str());
    inTangentAccessor.append_attribute("count").set_value(2);
    inTangentAccessor.append_attribute("stride").set_value(2);

    pugi::xml_node inTangentParamA = inTangentAccessor.append_child("param");
    inTangentParamA.append_attribute("name").set_value("X");
    inTangentParamA.append_attribute("type").set_value("float");
    pugi::xml_node inTangentParamB = inTangentAccessor.append_child("param");
    inTangentParamB.append_attribute("name").set_value("Y");
    inTangentParamB.append_attribute("type").set_value("float");
}


void AniAnimDesc::WriteOutTangentElement(std::string &animKey, pugi::xml_node &animRoot,
                                         std::vector<AniKeyframe> &frames) const {
    pugi::xml_node outTangentNode = animRoot.append_child("source");
    outTangentNode.append_attribute("id").set_value((animKey + "-outtangent").c_str());
    auto outTangentArrayKey = (animKey + "-outtangent-array");
    pugi::xml_node outTangentArrayNode = outTangentNode.append_child("float_array");
    outTangentArrayNode.append_attribute("id").set_value(
            outTangentArrayKey.c_str());
    outTangentArrayNode.append_attribute("count").set_value(2 * frames.size());
    //TODO outTangentArrayNode.

    pugi::xml_node outTangentAccessor = outTangentNode.child("technique_common").child("accessor");
    outTangentAccessor.append_attribute("source").set_value(("#" + outTangentArrayKey).c_str());
    outTangentAccessor.append_attribute("count").set_value(2);
    outTangentAccessor.append_attribute("stride").set_value(2);

    pugi::xml_node outTangentParamA = outTangentAccessor.append_child("param");
    outTangentParamA.append_attribute("name").set_value("X");
    outTangentParamA.append_attribute("type").set_value("float");
    pugi::xml_node outTangentParamB = outTangentAccessor.append_child("param");
    outTangentParamB.append_attribute("name").set_value("Y");
    outTangentParamA.append_attribute("type").set_value("float");
}


void AniAnimDesc::WriteSamplerElement(std::string &animKey, pugi::xml_node &animRoot) const {
    pugi::xml_node samplerNode = animRoot.append_child("sampler");
    samplerNode.append_attribute("id").set_value((animKey + "-sampler").c_str());
    samplerNode.find_child_by_attribute("input", "semantic", "INPUT").append_attribute("source").set_value(
            ("#" + animKey + "-input").c_str());
    samplerNode.find_child_by_attribute("output", "semantic", "OUTPUT").append_attribute("source").set_value(
            ("#" + animKey + "-output").c_str());
    samplerNode.find_child_by_attribute("input", "semantic", "INTERPOLATION").append_attribute("source").set_value(
            ("#" + animKey + "-interpolation").c_str());
    samplerNode.find_child_by_attribute("input", "semantic", "IN_TANGENT").append_attribute("source").set_value(
            ("#" + animKey + "-intangent").c_str());
    samplerNode.find_child_by_attribute("input", "semantic", "OUT_TANGENT").append_attribute("source").set_value(
            ("#" + animKey + "-outtangent").c_str());
}