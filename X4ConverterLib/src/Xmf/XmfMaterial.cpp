#include "StdInc.h"

using namespace boost;

XmfMaterial::XmfMaterial ()
{
    FirstIndex = 0;
    NumIndices = 0;
    memset ( Name, 0, sizeof(Name) );
}

XmfMaterial::XmfMaterial ( int firstIndex, int numIndices, const std::string& name )
{
    if ( name.size () >= sizeof(Name) )
        throw (format("Material name %s is too long") % name).str ();

    FirstIndex = firstIndex;
    NumIndices = numIndices;
    memset ( Name, 0, sizeof(Name) );
    memcpy ( Name, name.c_str (), name.size () );
}
