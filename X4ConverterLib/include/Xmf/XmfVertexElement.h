#pragma once

#pragma pack(push)
#pragma pack(1)

struct XmfVertexElement
{
                        XmfVertexElement            ();
                        XmfVertexElement            ( D3DDECLTYPE type, D3DDECLUSAGE usage, byte usageIndex = 0 );

    dword               Type;
    byte                Usage;
    byte                UsageIndex;
    byte                _pad0[2];
};

#pragma pack(pop)
