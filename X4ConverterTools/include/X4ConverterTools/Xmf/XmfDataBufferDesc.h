#pragma once

#include <iostream>
#include <boost/format.hpp>
#include <assimp/StreamReader.h>
#include <assimp/StreamWriter.h>
#include "XmfVertexElement.h"

#include "../DirectX.h"
#include "../Util/DXUtil.h"

class XmfDataBufferDesc {
public:
    XmfDataBufferDesc() = default;

    explicit XmfDataBufferDesc(Assimp::StreamReaderLE &reader);

    void Write(Assimp::StreamWriterLE &writer);

    std::string validate();
    dword Type;
    dword UsageIndex;
    dword DataOffset;
    dword Compressed;
    byte _pad0[4] = {0, 0, 0, 0};
    dword Format;
    dword CompressedDataSize;
    dword NumItemsPerSection;
    dword ItemSize;
    dword NumSections;
    byte _pad1[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    dword NumVertexElements;
    XmfVertexElement VertexElements[16];

    bool IsVertexBuffer() const;

    bool IsIndexBuffer() const;


    int GetVertexDeclarationSize();

    D3DFORMAT GetIndexFormat();

    void DenormalizeVertexDeclaration();

    void NormalizeVertexDeclaration();

};
