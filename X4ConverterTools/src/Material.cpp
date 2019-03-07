#include <X4ConverterTools/Material.h>
using namespace boost;
using namespace boost::algorithm;
using namespace boost::filesystem;

Material::Material() {
	_pCollectionName = "";

	_diffuseStrength = NAN;
	_normalStrength = NAN;
	_specularStrength = NAN;
	_environmentStrength = NAN;

}

Material::Material(std::string pCollectionName, pugi::xml_node node) {
	_pCollectionName = pCollectionName;
	_name = node.attribute("name").value();

	pugi::xpath_node_set properties = node.select_nodes("properties/property");
	for (auto it = properties.begin(); it != properties.end(); ++it) {
		std::string type = it->node().attribute("type").value();
		std::string name = it->node().attribute("name").value();
		std::string value = it->node().attribute("value").value();

		if (type == "BitMap") {
			if (name == "diffuse_map")
				_diffuseMapFilePath = value;
			else if (name == "specular_map")
				_specularMapFilePath = value;
			else if (name == "normal_map")
				_normalMapFilePath = value;
			else if (name == "environment_map")
				_environmentMapFilePath = value;
		} else if (type == "Float") {
			float fValue = atof(value.c_str());
			if (name == "diffuseStr")
				_diffuseStrength = fValue;
			else if (name == "normalStr")
				_normalStrength = fValue;
			else if (name == "specularStr")
				_specularStrength = fValue;
			else if (name == "environmentStr")
				_environmentStrength = fValue;
		}
	}
}

aiMaterial* Material::ConvertToAiMaterial(
		const path& modelFolderPath) const {
	aiMaterial* pAiMaterial = new aiMaterial();
	aiString* name = new aiString(_pCollectionName + "X" + GetName());
	pAiMaterial->AddProperty(name, AI_MATKEY_NAME);
	delete name;

	aiString temp = aiString();
	std::string textureFilePath = GetDecompressedTextureFilePath(_diffuseMapFilePath, modelFolderPath);
	if (!textureFilePath.empty()) {
		temp =
				PathUtil::GetRelativePath(textureFilePath, modelFolderPath).string();
		pAiMaterial->AddProperty(&temp, AI_MATKEY_TEXTURE_DIFFUSE(0));
	}

	textureFilePath = GetDecompressedTextureFilePath(_specularMapFilePath, modelFolderPath);
	if (!textureFilePath.empty()) {
		temp =
				PathUtil::GetRelativePath(textureFilePath, modelFolderPath).string();
		pAiMaterial->AddProperty(&temp, AI_MATKEY_TEXTURE_SPECULAR(0));
	}

	textureFilePath = GetDecompressedTextureFilePath(_normalMapFilePath, modelFolderPath);
	if (!textureFilePath.empty()) {
		temp =
				PathUtil::GetRelativePath(textureFilePath, modelFolderPath).string();
		pAiMaterial->AddProperty(&temp, AI_MATKEY_TEXTURE_NORMALS(0));
	}

	textureFilePath = GetDecompressedTextureFilePath(_environmentMapFilePath, modelFolderPath);
	if (!textureFilePath.empty()) {
		temp =
				PathUtil::GetRelativePath(textureFilePath, modelFolderPath).string();
		pAiMaterial->AddProperty(&temp, AI_MATKEY_TEXTURE_REFLECTION(0));
	}
	return pAiMaterial;
}

std::string Material::GetTextureFilePath(const std::string filePath, const path& modelFolderPath) const {
	static const char* ppszAllowedExtensions[] = { "gz", "dds", "tga", "jpg" };

	if (filePath.empty())
		return std::string();

	path textureFilePath(modelFolderPath);
	textureFilePath /= filePath;
	if (is_regular_file(textureFilePath))
		return textureFilePath.string();

	if (textureFilePath.has_extension())
		return std::string();

	for (int i = 0;
			i < sizeof(ppszAllowedExtensions) / sizeof(ppszAllowedExtensions[0]);
			++i) {
		textureFilePath.replace_extension(ppszAllowedExtensions[i]);
		if (is_regular_file(textureFilePath))
			return textureFilePath.string();
	}
	return std::string();
}

std::string Material::GetDecompressedTextureFilePath(const std::string compressedFilePath, const path& modelFolderPath) const {
	std::string filePath = GetTextureFilePath(compressedFilePath,modelFolderPath);
	if (filePath.empty())
		return std::string();

	path textureFilePath(filePath);
	if (!iequals(textureFilePath.extension().string(), ".gz"))
		return textureFilePath.string();

	path uncompressedPath(textureFilePath);
	uncompressedPath.replace_extension(".dds");
	if (is_regular_file(uncompressedPath))
		return uncompressedPath.string();

	gzFile pGzFile = gzopen(textureFilePath.string().c_str(), "rb");
	if (!pGzFile)
		return textureFilePath.string();

	FILE* pDdsFile = fopen(uncompressedPath.string().c_str(), "wb+");
	if (!pDdsFile) {
		gzclose(pGzFile);
		return textureFilePath.string();
	}

	byte buffer[0x400];
	int bytesRead;
	do {
		bytesRead = gzread(pGzFile, buffer, sizeof(buffer));
		fwrite(buffer, 1, bytesRead, pDdsFile);
	} while (bytesRead == sizeof(buffer));

	fclose(pDdsFile);
	gzclose(pGzFile);
	return uncompressedPath.string();
}
