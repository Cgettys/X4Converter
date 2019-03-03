#include "StdInc.h"

using namespace boost;
using namespace boost::filesystem;

MaterialLibrary::MaterialLibrary ( const std::string& gameBaseFolderPath )
    : _gameBaseFolderPath ( gameBaseFolderPath )
{
    std::string filePath = (path(gameBaseFolderPath) / "libraries" / "material_library.xml").string ();
    if ( !is_regular_file ( filePath ) )
        throw (format("Failed to load material library: %s does not exist") % filePath).str ();

    auto result = _doc.load_file ( filePath.c_str () );
    if ( result.status != pugi::status_ok )
        throw (format("Failed to parse material library: %s") % result.description ()).str ();
}

MaterialCollection* MaterialLibrary::GetCollection ( const std::string& name )
{
    auto it = _collections.find ( name );
    if ( it != _collections.end () )
        return &(it->second);

    pugi::xml_node collectionNode = _doc.select_single_node ( (format("/materiallibrary/collection[@name='%s']") % name).str().c_str() ).node ();
    if ( !collectionNode )
        return nullptr;

    _collections[name] = MaterialCollection ( this, collectionNode );
    return &_collections[name];
}

Material* MaterialLibrary::GetMaterial ( const std::string& dottedName )
{
    std::smatch match;
    if ( !std::regex_match ( dottedName, match, std::regex("(\\w+)\\.(\\w+)") ) )
        return nullptr;

    MaterialCollection* pCollection = GetCollection ( match[1] );
    if ( !pCollection )
        return nullptr;

    return pCollection->GetMaterial ( match[2] );
}
