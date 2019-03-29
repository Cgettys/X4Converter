#pragma once

#include <assimp/StreamReader.h>
#include <assimp/StreamWriter.h>
#include <X4ConverterTools/types.h>
#include "X4ConverterTools/types/DirectX.h"
namespace xmf {
    class XmfVertexElement {

    public:
        XmfVertexElement();

        XmfVertexElement(D3DDECLTYPE type, D3DDECLUSAGE usage, byte usageIndex = 0);

        explicit XmfVertexElement(Assimp::StreamReaderLE &reader);

        void Write(Assimp::StreamWriterLE &writer);

        dword Type;
        byte Usage;
        byte UsageIndex;
        byte _pad0[2] = {0, 0};
    };
}