#pragma once

#include <cstdint>
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

        uint64_t GetCompressedDataSize() const {
            return CompressedDataSize;
        }

        uint64_t GetUncompressedDataSize() const {
            return NumSections * NumItemsPerSection * ItemSize;
        }

        bool IsCompressed() const;

        bool IsVertexBuffer() const;

        bool IsIndexBuffer() const;


        int GetVertexDeclarationSize();

        D3DFORMAT GetIndexFormat();

        uint32_t NumItemsPerSection;
        uint32_t NumVertexElements;
        XmfVertexElement VertexElements[16];

        uint32_t DataOffset;
        uint32_t Compressed;
        uint32_t Type;
        uint32_t UsageIndex;
        uint8_t _pad0[4] = {0, 0, 0, 0};
        uint32_t Format;
        uint32_t CompressedDataSize;
        int ItemSize;
        uint32_t NumSections;
        uint8_t _pad1[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

        void DenormalizeVertexDeclaration();

        void NormalizeVertexDeclaration();

    private:
    };
}
