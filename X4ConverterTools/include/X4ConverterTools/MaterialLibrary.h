#pragma once

class MaterialLibrary
{
    friend Material;

public:
                                                    MaterialLibrary     ( const std::string& gameBaseFolderPath );

    MaterialCollection*                             GetCollection       ( const std::string& name );
    Material*                                       GetMaterial         ( const std::string& dottedName );

private:
    std::string                                     _gameBaseFolderPath;
    pugi::xml_document                              _doc;
    std::map < std::string, MaterialCollection >    _collections;
};
