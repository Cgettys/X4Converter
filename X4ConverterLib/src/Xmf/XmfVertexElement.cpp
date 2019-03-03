#include "StdInc.h"

XmfVertexElement::XmfVertexElement ()
{
    Type = D3DDECLTYPE_UNUSED;
}

XmfVertexElement::XmfVertexElement ( D3DDECLTYPE type, D3DDECLUSAGE usage, byte usageIndex )
{
    Type = type;
    Usage = usage;
    UsageIndex = usageIndex;
}
