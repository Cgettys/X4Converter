#include <X4ConverterTools/Component.h>

using namespace Assimp;

using namespace boost;
using namespace boost::filesystem;

std::shared_ptr<Component>
Component::ReadFromFile(const std::string &filePath, const ConversionContext &context, IOSystem *pIOHandler) {

    if (!is_regular_file(filePath)) {
        throw std::runtime_error(".xml file doesn't exist");
    }

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filePath.c_str());
    if (result.status != pugi::status_ok) {
        throw std::runtime_error(str(format("Failed to parse .xml file: %s") % result.description()));
    }

    pugi::xml_node componentNode = GetComponentNode(doc);
    path geometryFolderPath = GetGeometryFolderPath(componentNode, context.gameBaseFolderPath, false);
    pugi::xpath_node_set partNodes = componentNode.select_nodes("connections/connection/parts/part");

    if (partNodes.empty()) {
        throw std::runtime_error("File does not contain any <part> elements");
    }

    std::shared_ptr<Component> pComponent = std::make_shared<Component>();
    pComponent->Name = componentNode.attribute("name").value();
    for (auto partNode : partNodes) {
        ComponentPart part(partNode.node(), geometryFolderPath, pIOHandler);
        pComponent->Parts[part.Name] = part;
    }
    return pComponent;
}
// Import
aiNode *Component::GetEquivalentAiNode( ConversionContext &context) {
    std::map<std::string, aiNode *> partNodes;

    // Create nodes and meshes
    for (auto &Part : Parts) {
        partNodes[Part.first] = Part.second.GetEquivalentAiNode(context);
    }

    // Link parent nodes
    std::vector<aiNode *> rootNodes;
    std::map<aiNode *, std::vector<aiNode *> > nodeChildren;
    for (auto &Part : Parts) {
        aiNode *pPartNode = partNodes[Part.first];
        if (Part.second.ParentName.empty()) {
            rootNodes.push_back(pPartNode);
            continue;
        }

        auto parentIt = partNodes.find(Part.second.ParentName);
        if (parentIt == partNodes.end()) {
            throw std::runtime_error(str(format("Node %1% has invalid parent %2%") % (Part.first).c_str() %
                                         (Part.second.ParentName).c_str()));
        }
        nodeChildren[parentIt->second].push_back(pPartNode);
    }

    for (auto &it : nodeChildren) {
        aiNode *pParentNode = it.first;
        auto **ppNewChildren = new aiNode *[pParentNode->mNumChildren + it.second.size()];
        if (pParentNode->mChildren) {
            memcpy(ppNewChildren, pParentNode->mChildren, sizeof(aiNode *) * pParentNode->mNumChildren);
            delete[] pParentNode->mChildren;
        }
        pParentNode->mChildren = ppNewChildren;

        for (aiNode *pChildNode : it.second) {
            pParentNode->mChildren[pParentNode->mNumChildren++] = pChildNode;
            pChildNode->mParent = pParentNode;
        }
    }

    // Create component node that contains all the part nodes
    if (rootNodes.empty()) {
        throw std::runtime_error("No root parts found");
    }
    auto *pComponentNode = new aiNode(Name);
    pComponentNode->mChildren = new aiNode *[rootNodes.size()];

    aiNode *pRealRootNode = new aiNode("Scene");// To make blender happy name root node Scene?
    pRealRootNode->mChildren = new aiNode *[1];
    pRealRootNode->mChildren[0] = pComponentNode;
    pRealRootNode->mNumChildren = 1;
    pComponentNode->mParent = pRealRootNode;
    for (int i = 0; i < rootNodes.size(); i++) {
        aiNode *pRootNode = rootNodes[i];
        pComponentNode->mChildren[i] = pRootNode;
        pRootNode->mParent = pComponentNode;
        pComponentNode->mNumChildren++;
    }
    return pRealRootNode;
}

void Component::WriteToFile(const std::string &filePath, const std::string &gameBaseFolderPath,
                            Assimp::IOSystem *pIOHandler) {
    if (!is_regular_file(filePath)) {
        CreateDummyFile(filePath, gameBaseFolderPath);
    }
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filePath.c_str());
    if (result.status != pugi::status_ok) {
        throw std::runtime_error(str(format("Failed to parse %s: %s") % filePath % result.description()));
    }
    pugi::xml_node componentNode = GetComponentNode(doc);
    path geometryFolderPath = GetGeometryFolderPath(componentNode, gameBaseFolderPath, true);

    // Remove parts that no longer exist
    pugi::xpath_node_set partNodes = componentNode.select_nodes("connections/connection/parts/part");
    for (auto partNode : partNodes) {
        std::string partName = partNode.node().attribute("name").value();
        if (Parts.find(partName) != Parts.end()) {
            continue;
        }
        pugi::xml_node connectionNode = partNode.node().parent().parent();
        connectionNode.parent().remove_child(connectionNode);
    }

    // Add/update component parts
    pugi::xml_node connectionsNode = componentNode.select_node("connections").node();
    if (!connectionsNode) {
        connectionsNode = componentNode.append_child("connections");
    }

    for (auto &Part : Parts) {
        Part.second.WritePart(connectionsNode, geometryFolderPath, pIOHandler);
    }

    doc.save_file(filePath.c_str());
}

pugi::xml_node Component::GetComponentNode(pugi::xml_document &doc) {
    pugi::xml_node componentNode = doc.select_node("/components/component").node();
    if (!componentNode) {
        throw std::runtime_error("File has no <component> element");
    }
    return componentNode;
}

path Component::GetGeometryFolderPath(pugi::xml_node componentNode, const std::string &gameBaseFolderPath,
                                      bool createIfMissing) {
    pugi::xml_attribute geometryAttr = componentNode.select_node("source/@geometry").attribute();
    if (!geometryAttr) {
        throw std::runtime_error("File has no geometry source");
    }

    std::string pathStr = geometryAttr.value();
    pathStr = PathUtil::MakePlatformSafe(pathStr);
    path geometryFolderPath = path(gameBaseFolderPath) / pathStr;
    if (!is_directory(geometryFolderPath)) {
        if (!createIfMissing) {
            throw std::runtime_error(str(format("Directory %s doesn't exist") % geometryFolderPath));
        }
        create_directory(geometryFolderPath);
    }

    return geometryFolderPath;
}

void Component::CreateDummyFile(const std::string &filePath, const std::string &gameBaseFolderPath) {
    pugi::xml_document doc;
    pugi::xml_node componentsNode = doc.append_child("components");

    pugi::xml_node componentNode = componentsNode.append_child("component");
    componentNode.append_attribute("name").set_value(Name.c_str());

    pugi::xml_node sourceNode = componentNode.append_child("source");
    path dataPath = path(filePath).parent_path() / (Name + "_data");
    dataPath = PathUtil::GetRelativePath(dataPath, gameBaseFolderPath);
    sourceNode.append_attribute("geometry").set_value(dataPath.string().c_str());

    pugi::xml_node connectionsNode = componentNode.append_child("connections");

    doc.save_file(filePath.c_str());
}

