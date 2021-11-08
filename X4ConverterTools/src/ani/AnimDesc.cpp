#include <X4ConverterTools/ani/AnimDesc.h>

namespace algo = boost::algorithm;
using namespace boost;
using namespace Assimp;

namespace ani {
AnimDesc::AnimDesc(StreamReaderLE &reader) {
  for (char &c : Name) {
    reader >> c;

  }

  for (char &c : SubName) {
    reader >> c;
  }

  SafeName = std::string(Name);
  SafeSubName = std::string(SubName);
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

// Export Conversion
AnimDesc::AnimDesc(const std::string &partName, pugi::xml_node node, pugi::xml_node animMeta) {
  // TODO validate against what's actually written
  SafeName = partName;
  SafeSubName = node.attribute("subname").as_string();
  // TODO validate better
  ReadAniXmlKeyframesForKeytype(node, "position", posKeys);
  ReadAniXmlKeyframesForKeytype(node, "rotation_euler", rotKeys);
  ReadAniXmlKeyframesForKeytype(node, "scale", scaleKeys);
  ReadAniXmlKeyframesForKeytype(node, "prescale", preScaleKeys);
  ReadAniXmlKeyframesForKeytype(node, "postscale", postScaleKeys);
  PopulateNumFields();
//  // TODO null checks
  auto start = animMeta.child("frames").attribute("start").as_int();
  auto end = animMeta.child("frames").attribute("end").as_int();
  if (NumPosKeys + NumRotKeys + NumScaleKeys + NumPreScaleKeys + NumPostScaleKeys == 0) {
    Duration = (end - start) / 30.0;
    if (start == 1) {
      Duration = (1 + end - start) / 30.0;
    }
  } else {
    Duration = MaxSeenTime - MinSeenTime;
  }
  if (Duration == 0.0) {
    Duration = 1 / 30.0;
  }
}

void AnimDesc::ReadAniXmlKeyframesForKeytype(const pugi::xml_node &node,
                                             const char *keytype,
                                             std::vector<Keyframe> &frameDest) {
  auto keyNode = node.child(keytype);
  // TODO we should probably check that keyframe times increase monotonically on the source side
  if (keyNode) {
    auto xNode = keyNode.child("X");
    auto yNode = keyNode.child("Y");
    auto zNode = keyNode.child("Y");
    if ((xNode || yNode || zNode) && (!xNode || !yNode || !zNode)) {
      throw std::runtime_error("Either all or no axes should be specified");
    }
    auto xChild = xNode.first_child();
    auto yChild = yNode.first_child();
    auto zChild = zNode.first_child();
    if (!xChild && !yChild && !zChild) {
      return; // No frames, return early
    }
    // Loop through all keyframes
    while (xChild || yChild || zChild) {
      frameDest.emplace_back(xChild, yChild, zChild);

      xChild = xChild.next_sibling();
      yChild = yChild.next_sibling();
      zChild = zChild.next_sibling();
    }
  }
  for (auto &frame : frameDest) {
    auto time = frame.GetTime();
    MaxSeenTime = std::max(MaxSeenTime, time);
    MinSeenTime = std::min(MinSeenTime, time);
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
// Export
void AnimDesc::WriteToGameFiles(StreamWriterLE &writer) {
  // Copy names into their character buffers
  // Not sure if 63 or 64 is the limit, not pushing it
  if (SafeName.size() > 63) {
    throw runtime_error("Name cannot be longer than 63 bytes");
  }
  if (SafeSubName.size() > 63) {
    throw runtime_error("SubName cannot be longer than 63 bytes");
  }
  memcpy(Name, SafeName.c_str(), SafeName.size());
  memcpy(SubName, SafeSubName.c_str(), SafeSubName.size());

  PopulateNumFields();

  for (char &c : Name) {
    writer << c;

  }

  for (char &c : SubName) {
    writer << c;
  }

  writer << NumPosKeys;
  writer << NumRotKeys;
  writer << NumScaleKeys;
  writer << NumPreScaleKeys;
  writer << NumPostScaleKeys;
  writer << Duration;
  for (int &i : Padding) {
    writer << i;
  }
}

void AnimDesc::PopulateNumFields() {
  NumPosKeys = numeric_cast<int>(posKeys.size());
  NumRotKeys = numeric_cast<int>(rotKeys.size());
  NumScaleKeys = numeric_cast<int>(scaleKeys.size());
  NumPreScaleKeys = numeric_cast<int>(preScaleKeys.size());
  NumPostScaleKeys = numeric_cast<int>(postScaleKeys.size());
}

void AnimDesc::write_frames(StreamWriterLE &writer) {
  for (int i = 0; i < NumPosKeys; i++) {
    posKeys[i].WriteToGameFiles(writer);
  }
  for (int i = 0; i < NumRotKeys; i++) {
    rotKeys[i].WriteToGameFiles(writer);
  }
  for (int i = 0; i < NumScaleKeys; i++) {
    scaleKeys[i].WriteToGameFiles(writer);
  }
  for (int i = 0; i < NumPreScaleKeys; i++) {
    preScaleKeys[i].WriteToGameFiles(writer);
  }
  for (int i = 0; i < NumPostScaleKeys; i++) {
    postScaleKeys[i].WriteToGameFiles(writer);
  }
}

std::string AnimDesc::validate() {
  bool valid = true;
  std::string ret = "\nAnimationDesc: \n";

  ret.append(str(format("\tName: %1%\n\tSubName: %2%\n") % SafeName % SafeSubName));
  bool seenSubName = false;
  // TODO clean this up
  std::set<std::string> known{
      // Used for station/teleporter doors???
      "doors_open", "doors_closing", "doors_closed", "doors_opening",
      // Used for ship cockpit open/close
      // On medium ships, sometimes used to trigger ramps
      "cockpit_open", "cockpit_closing", "cockpit_closed", "cockpit_opening",

      // Used for flaps??? not really sure, used for interior props too
      "flaps_minmax", "flaps_up_minmax", "flaps_right_minmax",


      // Weapons
      "gun_firing", "turret_inactive", "turret_activating", "turret_active", "turret_deactivating",

      // Explosions etc
      "playonce", "scaling_range", // Also on some props

      // Landing gear, 'nuf said
      "landinggears_activating", "landinggears_active", "landinggears_deactivating", "landinggears_inactive",


      // docking bay
      // Includes airlock as xs hatch?
      "dockingbay_closed", "dockingbay_opening", "dockingbay_open", "dockingbay_closing",

      // medium ship docking bay?
      "dockingbay_fence_closed", "dockingbay_fence_opening", "dockingbay_fence_open",
      "dockingbay_fence_closing", "dockingbay_unassigned", "dockingbay_assigning", "dockingbay_assigned",
      "dockingbay_unassigning", "dockingbay_warning_inactive", "dockingbay_warning_activating",
      "dockingbay_warning_active", "dockingbay_warning_deactivating", "dockingbay_idle",


      // Used for loops
      "loop",
      // No clue how these work
      "loop_random", "repeat",

      // Mystery states #1
      "deactivated", "activating", "active", "deactivating",

      // Mystery states #2 - some debris/environmental stuff
      "mode_inactive", "mode_activating", "mode_active", "mode_deactivating", "mode_abort",

      // Mystery states #3
      "unlocked", "locking", "locked", "unlocking",

      // Mystery states #4 - probably related to signal leaks???
      "signal_normalmode", "signal_scanmode",

      // Chairs... wow that's a lot.
      "chair_default_inactive", "chair_default_activating", "chair_default_active",
      "chair_default_deactivating", "chair_paranid_inactive", "chair_paranid_activating",
      "chair_paranid_active", "chair_paranid_deactivating", "chair_default_to_paranid",
      "chair_paranid_to_default", "chair_default_inactive", "chair_default_activating",
      "chair_default_active",

      // The weirdos
      // Appears to be for weapon hitting something (appears on some cargo/gate effects???)
      "weapon_firing",
      // Seen on drones
      "cockpit_entered", "cockpit_leaving", "cockpit_left", "cockpit_entering",

      // LOD-like presets (fog, etc)
      "dockingbay_retrievingship_fastest", "dockingbay_storingship_faster",
      "dockingbay_retrievingship_faster", "dockingbay_storingship", "dockingbay_retrievingship",
      "dockingbay_idle",

      // Found on a wreck??
      "switchable01_activating", "switchable01_active", "switchable01_deactivating", "switchable01_inactive",

      // Cutscene-related. No clue how they work
      // And they appear on some ships bizarrely
      // may be how it knows what to do with them during a cutscene??
      "cutsceneloop", "gamestart1", "gamestart2", "gamestart3", "gamestart4",};
  for (const auto &s : known) {
    if (s == SafeSubName) {
      seenSubName = true;
      break;
    }
  }
  if (!seenSubName) {
    ret.append("New Animation Trigger! See SubName (above)");
    std::cerr << "New Animation Trigger: ";
    std::cerr << "\"" << SafeSubName << "\"," << std::endl;
  }


  // TODO better indentation
  //TODO check expected # of frames = actual length of array
  ret.append(str(format(
      "\tNumPosKeys: %1%\n\tNumRotKeys: %2%\n\tNumScaleKeys: %3%\n\tNumPreScaleKeys: %4%\n\tNumPostScaleKeys: %5%\n\tDuration: %6%\n")
                     %
                         NumPosKeys % NumRotKeys % NumScaleKeys % NumPreScaleKeys % NumPostScaleKeys % Duration));
  for (int &i : Padding) {
    if (i != 0) {
      valid = false;
      ret.append("PADDING MUST BE 0");
    }
  }

  valid &= validateFrameType(ret, "Position", NumPosKeys, posKeys);
  valid &= validateFrameType(ret, "Rotation", NumRotKeys, rotKeys);
  valid &= validateFrameType(ret, "Scale", NumScaleKeys, scaleKeys);
  valid &= validateFrameType(ret, "PreScale", NumPreScaleKeys, preScaleKeys);
  valid &= validateFrameType(ret, "PostScale", NumPostScaleKeys, postScaleKeys);

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

bool AnimDesc::validateFrameType(std::string &ret,
                                 std::string name,
                                 size_t expectedFrameCount,
                                 std::vector<Keyframe> frames) {
  auto valid = true;
  auto actualFrameCount = frames.size();
  ret.append(name + " Keyframes:\n");
  if (expectedFrameCount != actualFrameCount) {
    ret.append(str(
        format("Number of Keys specified in description for %1% != Number of Keys provided: %2% vs %3%") % name
            % expectedFrameCount % actualFrameCount));
    valid = false;
  }
  for (int i = 0; i < actualFrameCount; i++) {
    try {
      ret.append(frames[i].validate());
    } catch (std::exception &e) {
      ret.append(e.what());
      valid = false;
    }
  }
  return valid;
}
void AnimDesc::WriteIntermediateRepr(pugi::xml_node tgtNode) const {
  std::string keys[] = {"location", "rotation_euler", "scale", "prescale", "postscale"};
  Axis axes[] = {Axis::X, Axis::Y, Axis::Z};
  for (std::string &key : keys) {
    for (auto axis: axes) {
      WriteIntermediateReprOfChannel(tgtNode, key, axis);
    }
  }
}

void AnimDesc::WriteIntermediateReprOfChannel(pugi::xml_node tgtNode, const std::string &keyType,
                                              Axis axis) const {

  std::vector<Keyframe> frames;
  if (keyType == "location") {
    frames = posKeys;
  } else if (keyType == "rotation_euler") {
    frames = rotKeys;
  } else if (keyType == "scale") {
    frames = scaleKeys;
  } else if (keyType == "prescale") {
    frames = preScaleKeys;
  } else if (keyType == "postscale") {
    frames = postScaleKeys;
  } else {
    throw runtime_error("Invalid keyType");
  }
//NB: it's valid and probably expected for parents not to have actual keyframes
//  if (frames.empty()) {
//    return;
//  }
  std::vector<std::string> namePortions;
  algo::split(namePortions, SafeName, is_any_of(" "));
  std::string namePortion = namePortions[0];
  const std::string &id = namePortion;

  if (!tgtNode.find_child_by_attribute("part", "name", id.c_str())) {
    tgtNode.append_child("part").append_attribute("name").set_value(id.c_str());
  }
  pugi::xml_node partRoot = tgtNode.find_child_by_attribute("part", "name", id.c_str());

  std::string subNameCategory;
//            std::string subNameRemaining;
  auto idx = SafeSubName.find('_');
  if (idx == std::string::npos) {
    subNameCategory = "misc";
//                subNameRemaining=SafeSubName;
  } else {
    subNameCategory = SafeSubName.substr(0, idx);
//                subNameRemaining=SafeSubName.substr(idx+1);
  }

  if (!partRoot.find_child_by_attribute("category", "name", subNameCategory.c_str())) {
    partRoot.append_child("category").append_attribute("name").set_value(subNameCategory.c_str());
  }

  pugi::xml_node catRoot = partRoot.find_child_by_attribute("category", "name", subNameCategory.c_str());
  if (!catRoot.find_child_by_attribute("animation", "subname", SafeSubName.c_str())) {
    catRoot.append_child("animation").append_attribute("subname").set_value(SafeSubName.c_str());
  }

  pugi::xml_node animRoot = catRoot.find_child_by_attribute("animation", "subname", SafeSubName.c_str());
  if (!animRoot.child(keyType.c_str())) {
    animRoot.append_child(keyType.c_str());
  }

  if (frames.empty()) {
    return;
  }
  pugi::xml_node channelRoot = animRoot.child(keyType.c_str());
  if (!channelRoot.child(GetAxisString(axis))) {
    channelRoot.append_child(GetAxisString(axis));
  }
  pugi::xml_node axisRoot = channelRoot.child(GetAxisString(axis));
  for (auto f : frames) {
    f.WriteChannel(axisRoot, axis);
  }

}

}