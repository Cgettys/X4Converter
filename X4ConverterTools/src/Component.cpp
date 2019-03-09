#include <X4ConverterTools/Component.h>

using namespace Assimp;

using namespace boost;
using namespace boost::filesystem;

Component *Component::ReadFromFile(const std::string& filePath,
		const std::string& gameBaseFolderPath, IOSystem* pIOHandler) {
	if (!is_regular_file(filePath)) {
		throw std::runtime_error(".xml file doesn't exist");
	}
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(filePath.c_str());
	if (result.status != pugi::status_ok) {
		throw std::runtime_error(
				str(
						format("Failed to parse .xml file: %s")
								% result.description()));
	}
	pugi::xml_node componentNode = GetComponentNode(doc);
	path geometryFolderPath = GetGeometryFolderPath(componentNode,
			gameBaseFolderPath, false);
	pugi::xpath_node_set partNodes = componentNode.select_nodes(
			"connections/connection/parts/part");
	if (partNodes.empty()) {
		throw std::runtime_error("File does not contain any <part> elements");
	}
	Component* pComponent = new Component();
	pComponent->Name = componentNode.attribute("name").value();
	for (auto it = partNodes.begin(); it != partNodes.end(); ++it) {
		pComponent->ReadPart(it->node(), geometryFolderPath, pIOHandler);
	}
	return pComponent;
}

void Component::WriteToFile(const std::string& filePath,
		const std::string& gameBaseFolderPath, Assimp::IOSystem* pIOHandler) {
	if (!is_regular_file(filePath)) {
		CreateDummyFile(filePath, gameBaseFolderPath);
	}
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(filePath.c_str());
	if (result.status != pugi::status_ok)
		throw std::runtime_error(
				str(
						format("Failed to parse %s: %s") % filePath
								% result.description()));

	pugi::xml_node componentNode = GetComponentNode(doc);
	path geometryFolderPath = GetGeometryFolderPath(componentNode,
			gameBaseFolderPath, true);

	// Remove parts that no longer exist
	pugi::xpath_node_set partNodes = componentNode.select_nodes(
			"connections/connection/parts/part");
	for (auto it = partNodes.begin(); it != partNodes.end(); ++it) {
		std::string partName = it->node().attribute("name").value();
		if (Parts.find(partName) != Parts.end())
			continue;

		pugi::xml_node connectionNode = it->node().parent().parent();
		connectionNode.parent().remove_child(connectionNode);
	}

	// Add/update component parts
	pugi::xml_node connectionsNode =
			componentNode.select_node("connections").node();
	if (!connectionsNode)
		connectionsNode = componentNode.append_child("connections");

	for (auto it = Parts.begin(); it != Parts.end(); ++it) {
		WritePart(it->second, connectionsNode, geometryFolderPath, pIOHandler);
	}

	doc.save_file(filePath.c_str());
}

pugi::xml_node Component::GetComponentNode(pugi::xml_document& doc) {
	pugi::xml_node componentNode =
			doc.select_node("/components/component").node();
	if (!componentNode)
		throw std::runtime_error("File has no <component> element");

	return componentNode;
}

path Component::GetGeometryFolderPath(pugi::xml_node componentNode,
		const std::string& gameBaseFolderPath, bool createIfMissing) {
	pugi::xml_attribute geometryAttr = componentNode.select_node(
			"source/@geometry").attribute();
	if (!geometryAttr)
		throw std::runtime_error("File has no geometry source");
	std::string windowsPath = geometryAttr.value();
#ifdef BOOST_OS_LINUX
	std::replace(windowsPath.begin(), windowsPath.end(), '\\', '/');
#endif
	path geometryFolderPath = path(gameBaseFolderPath) / windowsPath;
	if (!is_directory(geometryFolderPath)) {
		if (!createIfMissing)
			throw std::runtime_error(
					str(
							format("Directory %s doesn't exist")
									% geometryFolderPath));

		create_directory(geometryFolderPath);
	}

	return geometryFolderPath;
}

void Component::ReadPart(pugi::xml_node partNode,
		const path& geometryFolderPath, Assimp::IOSystem* pIOHandler) {
	std::string partName = partNode.attribute("name").value();
	ComponentPart& part = Parts[partName];
	part.Name = partName;

	pugi::xml_attribute parentAttr = partNode.parent().parent().attribute(
			"parent");
	if (parentAttr)
		part.ParentName = parentAttr.value();

	pugi::xml_node posNode =
			partNode.select_node("../../offset/position").node();
	if (posNode) {
		part.Position = aiVector3D(posNode.attribute("x").as_float(),
				posNode.attribute("y").as_float(),
				posNode.attribute("z").as_float());
	}

	pugi::xml_node sizeNode = partNode.select_node("size/max").node();
	if (sizeNode) {
		part.Size = aiVector3D(sizeNode.attribute("x").as_float(),
				sizeNode.attribute("y").as_float(),
				sizeNode.attribute("z").as_float());
	}

	int lodIndex = 0;
	while (true) {
		// TODO what about the mysterious assets .xmfs?
		//TODO better solution for path generation & debugging
		path lodFilePath = geometryFolderPath
				/ (format("%s-lod%d.xmf") % partName % lodIndex).str();
//		std::cerr << "reading normal .xmf: " << lodFilePath << std::endl;
		if (!is_regular_file(lodFilePath)) {
			break;
		}

		std::shared_ptr<XuMeshFile> pMeshFile = XuMeshFile::ReadFromFile(
				lodFilePath.string(), pIOHandler);
		part.Lods.push_back(ComponentPartLod(lodIndex, pMeshFile));
		lodIndex++;
	}

	//TODO better solution
	path collisionFilePath = geometryFolderPath / (partName + "-collision.xmf");
//	std::cerr << "reading collison .xmf: " << collisionFilePath << std::endl;
	if (is_regular_file(collisionFilePath)) {
		part.CollisionMesh = XuMeshFile::ReadFromFile(
				collisionFilePath.string(), pIOHandler);
	}
}

void Component::CreateDummyFile(const std::string& filePath,
		const std::string& gameBaseFolderPath) {
	pugi::xml_document doc;
	pugi::xml_node componentsNode = doc.append_child("components");

	pugi::xml_node componentNode = componentsNode.append_child("component");
	componentNode.append_attribute("name").set_value(Name.c_str());

	pugi::xml_node sourceNode = componentNode.append_child("source");
	path dataPath = path(filePath).parent_path() / (Name + "_data");
	dataPath = PathUtil::GetRelativePath(dataPath, gameBaseFolderPath);
	sourceNode.append_attribute("geometry").set_value(
			dataPath.string().c_str());

	pugi::xml_node connectionsNode = componentNode.append_child("connections");

	doc.save_file(filePath.c_str());
}

void Component::WritePart(ComponentPart& part, pugi::xml_node connectionsNode,
		const path& geometryFolderPath, Assimp::IOSystem* pIOHandler) {
	pugi::xml_node connectionNode;
	pugi::xml_node partNode =
			connectionsNode.select_node(
					(format("connection/parts/part[@name='%s']") % part.Name).str().c_str()).node();
	if (!partNode) {
		connectionNode = connectionsNode.append_child("connection");
		int numConnections = connectionsNode.select_nodes("connection").size();
		connectionNode.append_attribute("name").set_value(
				(format("Connection%02d") % numConnections).str().c_str());
		connectionNode.append_attribute("tags").set_value("part");

		pugi::xml_node partsNode = connectionNode.append_child("parts");
		partNode = partsNode.append_child("part");
		partNode.append_attribute("name").set_value(part.Name.c_str());
	} else {
		connectionNode = partNode.parent().parent();
	}

	WritePartParent(part, partNode);
	WritePartPosition(part, partNode);
	WritePartSize(part, partNode);
	WritePartCenter(part, partNode);
	WritePartLods(part, partNode, geometryFolderPath, pIOHandler);
	if (part.CollisionMesh) {
		//TODO better solution
		std::string xmfFileName =
				(format("%s-collision.out.xmf") % part.Name).str();
		std::string xmfFilePath = (geometryFolderPath / xmfFileName).string();
		part.CollisionMesh->WriteToFile(xmfFilePath, pIOHandler);
	}
}

void Component::WritePartParent(ComponentPart& part, pugi::xml_node partNode) {
	pugi::xml_node connectionNode = partNode.parent().parent();
	pugi::xml_attribute parentAttr = connectionNode.attribute("parent");
	if (part.ParentName.empty()) {
		if (parentAttr)
			connectionNode.remove_attribute(parentAttr);
	} else {
		if (!parentAttr)
			parentAttr = connectionNode.append_attribute("parent");

		parentAttr.set_value(part.ParentName.c_str());
	}
}

void Component::WritePartPosition(ComponentPart& part,
		pugi::xml_node partNode) {
	pugi::xml_node connectionNode = partNode.parent().parent();
	pugi::xml_node offsetNode = connectionNode.select_node("offset").node();
	if (!offsetNode)
		offsetNode = connectionNode.append_child("offset");

	pugi::xml_node positionNode = offsetNode.select_node("position").node();
	if (!positionNode) {
		positionNode = offsetNode.append_child("position");
		positionNode.append_attribute("x");
		positionNode.append_attribute("y");
		positionNode.append_attribute("z");
	}
	positionNode.attribute("x").set_value(
			(format("%f") % part.Position.x).str().c_str());
	positionNode.attribute("y").set_value(
			(format("%f") % part.Position.y).str().c_str());
	positionNode.attribute("z").set_value(
			(format("%f") % part.Position.z).str().c_str());
}

void Component::WritePartSize(ComponentPart& part, pugi::xml_node partNode) {
	pugi::xml_node sizeNode = partNode.select_node("size").node();
	if (!sizeNode)
		sizeNode = partNode.append_child("size");

	pugi::xml_node maxNode = sizeNode.select_node("max").node();
	if (!maxNode) {
		maxNode = sizeNode.append_child("max");
		maxNode.append_attribute("x");
		maxNode.append_attribute("y");
		maxNode.append_attribute("z");
	}
	maxNode.attribute("x").set_value(part.Size.x);
	maxNode.attribute("y").set_value(part.Size.y);
	maxNode.attribute("z").set_value(part.Size.z);
}

void Component::WritePartCenter(ComponentPart& part, pugi::xml_node partNode) {
	pugi::xml_node sizeNode = partNode.select_node("size").node();
	if (!sizeNode)
		sizeNode = partNode.append_child("size");

	pugi::xml_node centerNode = sizeNode.select_node("center").node();
	if (!centerNode) {
		centerNode = sizeNode.append_child("center");
		centerNode.append_attribute("x");
		centerNode.append_attribute("y");
		centerNode.append_attribute("z");
	}
	centerNode.attribute("x").set_value(part.Center.x);
	centerNode.attribute("y").set_value(part.Center.y);
	centerNode.attribute("z").set_value(part.Center.z);
}

void Component::WritePartLods(ComponentPart& part, pugi::xml_node partNode,
		const path& geometryFolderPath, Assimp::IOSystem* pIOHandler) {
	pugi::xml_node lodsNode = partNode.select_node("lods").node();
	if (!lodsNode)
		lodsNode = partNode.append_child("lods");

	// Remove LOD's that are no longer in the part
	pugi::xpath_node_set lodNodes = lodsNode.select_nodes("lod");
	for (auto it = lodNodes.begin(); it != lodNodes.end(); ++it) {
		int lodIndex = it->node().attribute("index").as_int();
		if (!part.GetLod(lodIndex))
			lodsNode.remove_child(it->node());
	}

	// Add/update remaining LOD's
	for (ComponentPartLod lod : part.Lods) {
		pugi::xml_node lodNode =
				lodsNode.select_node(
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
		pugi::xpath_node_set materialNodes = materialsNode.select_nodes(
				"material");
		for (auto it = materialNodes.begin(); it != materialNodes.end(); ++it) {
			if (it->node().attribute("id").as_int() > lod.Mesh->NumMaterials())
				materialsNode.remove_child(it->node());
		}

		// Add/update remaining materials
		int materialId = 1;
		for (XmfMaterial material : lod.Mesh->GetMaterials()) {
			pugi::xml_node materialNode =
					materialsNode.select_node(
							(format("material[@id='%d']") % materialId).str().c_str()).node();
			if (!materialNode) {
				materialNode = materialsNode.append_child("material");
				materialNode.append_attribute("id").set_value(materialId);
			}
			pugi::xml_attribute refAttr = materialNode.attribute("ref");
			if (!refAttr)
				refAttr = materialNode.append_attribute("ref");

			refAttr.set_value(material.Name);
			materialId++;
		}

		// Write mesh file
		//TODO better solution
		std::string xmfFileName = (format("%s-lod%d.out..xmf") % part.Name
				% lod.LodIndex).str();
		std::string xmfFilePath = (geometryFolderPath / xmfFileName).string();
		lod.Mesh->WriteToFile(xmfFilePath, pIOHandler);
	}
}
