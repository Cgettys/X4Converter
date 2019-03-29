#include <X4ConverterTools/Component.h>

using namespace boost;
using namespace xmf;
ComponentPartLod *ComponentPart::GetLod(int lodIndex) {
    for (ComponentPartLod &lod: Lods) {
        if (lod.LodIndex == lodIndex) {
            return &lod;
        }
    }
    return nullptr;
}

ComponentPart::ComponentPart(pugi::xml_node partNode,
                             const boost::filesystem::path &geometryFolderPath,
                             Assimp::IOSystem *pIOHandler) {

    Name = partNode.attribute("name").value();

    pugi::xml_attribute parentAttr = partNode.parent().parent().attribute(
            "parent");
    if (parentAttr)
        ParentName = parentAttr.value();

    pugi::xml_node posNode =
            partNode.select_node("../../offset/position").node();
    if (posNode) {
        Position = aiVector3D(posNode.attribute("x").as_float(),
                              posNode.attribute("y").as_float(),
                              posNode.attribute("z").as_float());
    }

    pugi::xml_node sizeNode = partNode.select_node("size/max").node();
    if (sizeNode) {
        Size = aiVector3D(sizeNode.attribute("x").as_float(),
                          sizeNode.attribute("y").as_float(),
                          sizeNode.attribute("z").as_float());
    }

    int lodIndex = 0;
    while (true) {
        // TODO what about the mysterious assets .xmfs?
        //TODO better solution for path generation & debugging
        boost::filesystem::path lodFilePath = geometryFolderPath
                                              / (format("%s-lod%d.xmf") % Name % lodIndex).str();
        std::cout << "reading lod .xmf: " << lodFilePath << std::endl;
        if (!is_regular_file(lodFilePath)) {
            break;
        }

        std::shared_ptr<XuMeshFile> pMeshFile = XuMeshFile::ReadFromFile(
                lodFilePath.string(), pIOHandler);
        Lods.push_back(ComponentPartLod(lodIndex, pMeshFile));
        lodIndex++;
    }

    //TODO better solution
    boost::filesystem::path collisionFilePath = geometryFolderPath / (Name + "-collision.xmf");
    std::cout << "reading collison .xmf: " << collisionFilePath << std::endl;
    if (is_regular_file(collisionFilePath)) {
        CollisionMesh = XuMeshFile::ReadFromFile(
                collisionFilePath.string(), pIOHandler);
    }
}

void ComponentPart::WritePart(pugi::xml_node connectionsNode,
                              const boost::filesystem::path &geometryFolderPath, Assimp::IOSystem *pIOHandler) {
    pugi::xml_node connectionNode;
    pugi::xml_node partNode =
            connectionsNode.select_node(
                    (format("connection/parts/part[@name='%s']") % Name).str().c_str()).node();
    if (!partNode) {
        connectionNode = connectionsNode.append_child("connection");
        int numConnections = connectionsNode.select_nodes("connection").size();
        connectionNode.append_attribute("name").set_value(
                (format("Connection%02d") % numConnections).str().c_str());
        connectionNode.append_attribute("tags").set_value("part");

        pugi::xml_node partsNode = connectionNode.append_child("parts");
        partNode = partsNode.append_child("part");
        partNode.append_attribute("name").set_value(Name.c_str());
    }
    WritePartParent(partNode);
    WritePartPosition(partNode);

    // special case - ref parts don't have a size or center or lods
    if (partNode.attribute("ref")) {
        return;
    }
    pugi::xml_node sizeNode = partNode.select_node("size").node();
    if (!sizeNode) {
        sizeNode = partNode.append_child("size");
    }
    WritePartSize(sizeNode);
    WritePartCenter(sizeNode);
    WritePartLods(partNode, geometryFolderPath, pIOHandler);
    if (CollisionMesh) {
        //TODO better solution
        std::string xmfFileName =
                (format("%s-collision.out.xmf") % Name).str();
        std::string xmfFilePath = (geometryFolderPath / xmfFileName).string();
        CollisionMesh->WriteToFile(xmfFilePath, pIOHandler);
    }
}

void ComponentPart::WritePartParent(pugi::xml_node partNode) {
    pugi::xml_node connectionNode = partNode.parent().parent();
    pugi::xml_attribute parentAttr = connectionNode.attribute("parent");
    if (ParentName.empty()) {
        if (parentAttr) {
            connectionNode.remove_attribute(parentAttr);
        }
    } else {
        if (!parentAttr) {
            parentAttr = connectionNode.append_attribute("parent");
        }
        parentAttr.set_value(ParentName.c_str());
    }
}

void ComponentPart::WritePartPosition(pugi::xml_node partNode) {
    pugi::xml_node connectionNode = partNode.parent().parent();
    pugi::xml_node offsetNode = connectionNode.select_node("offset").node();
    if (!offsetNode) {
        offsetNode = connectionNode.append_child("offset");
    }
    pugi::xml_node positionNode = offsetNode.select_node("position").node();
    if (!positionNode) {
        positionNode = offsetNode.append_child("position");
        positionNode.append_attribute("x");
        positionNode.append_attribute("y");
        positionNode.append_attribute("z");
    }
    positionNode.attribute("x").set_value((format("%f") % Position.x).str().c_str());
    positionNode.attribute("y").set_value((format("%f") % Position.y).str().c_str());
    positionNode.attribute("z").set_value((format("%f") % Position.z).str().c_str());
}

void ComponentPart::WritePartSize(pugi::xml_node sizeNode) {
    pugi::xml_node maxNode = sizeNode.select_node("max").node();
    if (!maxNode) {
        maxNode = sizeNode.append_child("max");
        maxNode.append_attribute("x");
        maxNode.append_attribute("y");
        maxNode.append_attribute("z");
    }
    maxNode.attribute("x").set_value(Size.x);
    maxNode.attribute("y").set_value(Size.y);
    maxNode.attribute("z").set_value(Size.z);
}

void ComponentPart::WritePartCenter(pugi::xml_node sizeNode) {
    pugi::xml_node centerNode = sizeNode.select_node("center").node();
    if (!centerNode) {
        centerNode = sizeNode.append_child("center");
        centerNode.append_attribute("x");
        centerNode.append_attribute("y");
        centerNode.append_attribute("z");
    }
    centerNode.attribute("x").set_value(Center.x);
    centerNode.attribute("y").set_value(Center.y);
    centerNode.attribute("z").set_value(Center.z);
}

void ComponentPart::WritePartLods(pugi::xml_node partNode,
                                  const boost::filesystem::path &geometryFolderPath, Assimp::IOSystem *pIOHandler) {
    pugi::xml_node lodsNode = partNode.select_node("lods").node();
    if (!lodsNode)
        lodsNode = partNode.append_child("lods");

    // Remove LOD's that are no longer in the part
    pugi::xpath_node_set lodNodes = lodsNode.select_nodes("lod");
    for (auto it = lodNodes.begin(); it != lodNodes.end(); ++it) {
        int lodIndex = it->node().attribute("index").as_int();
        if (!GetLod(lodIndex))
            lodsNode.remove_child(it->node());
    }

    // Add/update remaining LOD's
    for (ComponentPartLod &lod: Lods) {
        pugi::xml_node lodNode = lodsNode.select_node(
                (format("lod[@index='%d']") % lod.LodIndex).str().c_str()).node();
        if (!lodNode) {
            lodNode = lodsNode.append_child("lod");
            lodNode.append_attribute("index").set_value(lod.LodIndex);
        }

        // Ensure material node
        pugi::xml_node materialsNode = lodNode.select_node("materials").node();
        if (!materialsNode)
            materialsNode = lodNode.append_child("materials");

        // Remove materials that are no longer in the lod
        pugi::xpath_node_set materialNodes = materialsNode.select_nodes("material");
        for (auto it = materialNodes.begin(); it != materialNodes.end(); ++it) {
            if (it->node().attribute("id").as_int() > lod.Mesh->NumMaterials())
                materialsNode.remove_child(it->node());
        }

        // Add/update remaining materials
        int materialId = 1;
        for (XmfMaterial &material: lod.Mesh->GetMaterials()) {
            pugi::xml_node materialNode = materialsNode.select_node(
                    (format("material[@id='%d']") % materialId).str().c_str()).node();
            if (!materialNode) {
                materialNode = materialsNode.append_child("material");
                materialNode.append_attribute("id").set_value(materialId);
            }
            pugi::xml_attribute refAttr = materialNode.attribute("ref");
            if (!refAttr) {
                refAttr = materialNode.append_attribute("ref");
            }
            refAttr.set_value(material.Name);
            materialId++;

        }

        // Write mesh file
        //TODO better solution
        std::string xmfFileName = (format("%s-lod%d.out.xmf") % Name % lod.LodIndex).str();
        std::string xmfFilePath = (geometryFolderPath / xmfFileName).string();
        lod.Mesh->WriteToFile(xmfFilePath, pIOHandler);
    }
}