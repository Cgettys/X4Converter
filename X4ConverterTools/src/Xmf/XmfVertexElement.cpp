#include <X4ConverterTools/StdInc.h>

XmfVertexElement::XmfVertexElement ()
{
    Type = D3DDECLTYPE_UNUSED;
    Usage = D3DDECLUSAGE_FOG;// Arbitrary unusually usage so it's easy to spot if we're dumb
    UsageIndex = 0; // Arbitrary number
}

XmfVertexElement::XmfVertexElement ( D3DDECLTYPE type, D3DDECLUSAGE usage, byte usageIndex )
{
    Type = type;
    Usage = usage;
    UsageIndex = usageIndex;
}
