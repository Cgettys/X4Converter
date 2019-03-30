#include <X4ConverterTools/ani/AnimDesc.h>

namespace algo = boost::algorithm;
using namespace boost;
using namespace Assimp;

namespace ani {
// TODO copy constructors?
    AnimDesc::AnimDesc(StreamReaderLE &reader) {
        for (char &c : Name) {
            reader >> c;

        }

        for (char &c : SubName) {
            reader >> c;
        }

        for (unsigned long i = 0; i < 64; i++) {
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

        for (unsigned long i = 0; i < 64; i++) {
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

    void AnimDesc::read_frames(StreamReaderLE &reader) {
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

    std::string AnimDesc::validate() {
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
            } catch (std::exception &e) {
                ret.append(e.what());
                valid = false;
            }
        }

        ret.append("Rotation Keyframes:\n");
        for (int i = 0; i < NumRotKeys; i++) {
            try {
                ret.append(rotKeys[i].validate());
            } catch (std::exception &e) {
                ret.append(e.what());
                valid = false;
            }
        }

        ret.append("Scale Keyframes:\n");
        for (int i = 0; i < NumScaleKeys; i++) {
            try {
                ret.append(scaleKeys[i].validate());
            } catch (std::exception &e) {
                ret.append(e.what());
                valid = false;
            }
        }

        ret.append("Prescale Keyframes:\n");
        for (int i = 0; i < NumPreScaleKeys; i++) {
            try {
                ret.append(preScaleKeys[i].validate());
            } catch (std::exception &e) {
                ret.append(e.what());
                valid = false;
            }
        }

        ret.append("PostScale Keyframes:\n");
        for (int i = 0; i < NumPostScaleKeys; i++) {
            try {
                ret.append(postScaleKeys[i].validate());
            } catch (std::exception &e) {
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

    void AnimDesc::WriteAnim(pugi::xml_node tgtNode) const {
        std::string keys[] = {"location", "rotation", "scale"};
        std::string axes[] = {"X", "Y", "Z"};
        for (std::string &key : keys) {
            for (std::string &axis: axes) {
                WriteChannel(tgtNode, key, axis);
            }
        }
    }

    void AnimDesc::WriteChannel(pugi::xml_node tgtNode, std::string keyType, std::string axis) const {

        std::vector<Keyframe> frames;
        if (keyType == "location") {
            frames = posKeys;
        } else if (keyType == "rotation") {
            frames = rotKeys;
        } else if (keyType == "scale") {
            frames = scaleKeys;
        } else {
            // TODO error
            std::cerr << "YOU FOOL!" << std::endl;
        }

        if (frames.empty()) {
            return;
        }
        std::vector<std::string> namePortions;
        algo::split(namePortions, SafeName, is_any_of(" "));
        std::string namePortion = namePortions[0];
        std::string id = namePortion;

        if (tgtNode.find_child_by_attribute("part","name",id.c_str()).empty()){
            tgtNode.append_child("part").append_attribute("name").set_value(id.c_str());
        }
        pugi::xml_node partRoot = tgtNode.find_child_by_attribute("part","name",id.c_str());

        if(!partRoot.find_child_by_attribute("animation","subname",SafeSubName.c_str())){
            partRoot.append_child("animation").append_attribute("subname").set_value(SafeSubName.c_str());
        }
        pugi::xml_node animRoot = partRoot.find_child_by_attribute("animation","subname",SafeSubName.c_str());

        if (!animRoot.child(keyType.c_str())){
            animRoot.append_child(keyType.c_str());
        }
        pugi::xml_node channelRoot = animRoot.child(keyType.c_str());

        if (!channelRoot.child(axis.c_str())){
            channelRoot.append_child(axis.c_str());
        }
        pugi::xml_node axisRoot = channelRoot.child(axis.c_str());
        for (auto f : frames){
            f.WriteChannel(axisRoot, axis);
        }


    }





}