#include "StdInc.h"

using namespace boost;

MaterialCollection::MaterialCollection ()
{
    _pLibrary = nullptr;
}

MaterialCollection::MaterialCollection ( MaterialLibrary* pLibrary, pugi::xml_node node )
{
    _pLibrary = pLibrary;
    _node = node;
    _name = node.attribute ( "name" ).value ();
}

Material* MaterialCollection::GetMaterial ( const std::string& name )
{
    auto it = _materials.find ( name );
    if ( it != _materials.end () )
        return &(it->second);

    pugi::xml_node materialNode = _node.select_single_node ( (format("material[@name='%s']") % name).str().c_str() ).node ();
    if ( !materialNode )
        return nullptr;

    _materials[name] = Material ( this, materialNode );
    return &_materials[name];
}
