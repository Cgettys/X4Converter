#pragma once

#include <iostream>
#include <boost/format.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <assimp/StreamReader.h>
#include <assimp/StreamWriter.h>
#include <X4ConverterTools/xmf/XmfVertexElement.h>
#include <X4ConverterTools/util/DXUtil.h>

namespace xmf {
    class XmfDataBufferDesc {
    public:
        XmfDataBufferDesc() = default;

        explicit XmfDataBufferDesc(Assimp::StreamReaderLE &reader);

        void Write(Assimp::StreamWriterLE &writer);

        std::string validate();

        uint32_t Type;
        uint32_t UsageIndex;
        uint32_t DataOffset;
        uint32_t Compressed;
        uint8_t _pad0[4] = {0, 0, 0, 0};
        uint32_t Format;
        uint32_t CompressedDataSize;
        uint32_t NumItemsPerSection;
        int ItemSize;
        uint32_t NumSections;
        uint8_t _pad1[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        uint32_t NumVertexElements;
        XmfVertexElement VertexElements[16];

        bool IsVertexBuffer() const;

        bool IsIndexBuffer() const;


        int GetVertexDeclarationSize();

        D3DFORMAT GetIndexFormat();

        void DenormalizeVertexDeclaration();

        void NormalizeVertexDeclaration();

    };
}
