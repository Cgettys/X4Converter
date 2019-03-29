#pragma once

#include <assimp/StreamReader.h>
#include <assimp/StreamWriter.h>
#include <X4ConverterTools/types.h>
#include "X4ConverterTools/types/DirectX.h"
namespace xmf {
    class XmfVertexElement {

    public:
        XmfVertexElement();

        XmfVertexElement(D3DDECLTYPE type, D3DDECLUSAGE usage, uint8_t usageIndex = 0);

        explicit XmfVertexElement(Assimp::StreamReaderLE &reader);

        void Write(Assimp::StreamWriterLE &writer);

        uint32_t Type;
        uint8_t Usage;
        uint8_t UsageIndex;
        uint8_t _pad0[2] = {0, 0};
    };
}