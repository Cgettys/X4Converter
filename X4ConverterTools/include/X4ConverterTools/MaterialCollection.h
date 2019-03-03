#pragma once

class MaterialLibrary;

class MaterialCollection
{
public:
                                        MaterialCollection  ();
                                        MaterialCollection  ( MaterialLibrary* pLibrary, pugi::xml_node node );

    MaterialLibrary*                    GetLibrary          () const    { return _pLibrary; }
    Material*                           GetMaterial         ( const std::string& name );

    const std::string&                  GetName             () const    { return _name; }

private:
    MaterialLibrary*                    _pLibrary;

    std::string                         _name;
    pugi::xml_node                      _node;
    std::map < std::string, Material >  _materials;
};
