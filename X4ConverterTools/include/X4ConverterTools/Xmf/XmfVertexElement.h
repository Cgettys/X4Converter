#pragma once

#include <assimp/StreamReader.h>
#include <assimp/StreamWriter.h>
#include <X4ConverterTools/Types.h>
#include "../DirectX.h"

class XmfVertexElement {

public:
    XmfVertexElement();

    XmfVertexElement(D3DDECLTYPE type, D3DDECLUSAGE usage, byte usageIndex = 0);

    explicit XmfVertexElement(Assimp::StreamReader<> &reader);

    void Write(Assimp::StreamWriter<> &writer);

    dword Type;
    byte Usage;
    byte UsageIndex;
    byte _pad0[2] = {0, 0};
};
