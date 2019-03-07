#pragma once
#include <map>
#include <boost/format.hpp>
#include <boost/range.hpp>
#include <X4ConverterTools/Material.h>
#include "pugixml.hpp"
class MaterialCollection
{
public:								    MaterialCollection  ();
                                        MaterialCollection  ( pugi::xml_node node );

    Material*                           GetMaterial         ( const std::string& name );

    const std::string&                  GetName             () const    { return _name; }

private:

    std::string                         _name;
    pugi::xml_node                      _node;
    std::map < std::string, Material >  _materials;
};
