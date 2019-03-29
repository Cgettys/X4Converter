#pragma once

#include <assimp/StreamReader.h>
#include <assimp/StreamWriter.h>
#include <X4ConverterTools/types.h>
#include <X4ConverterTools/xmf/XmfDataBufferDesc.h>
#include "zlib.h"

namespace xmf {
    class XmfDataBuffer {
    public:
        XmfDataBuffer();

        XmfDataBufferDesc Description;

        explicit XmfDataBuffer(Assimp::StreamReaderLE &descReader);

        void Read(Assimp::StreamReaderLE &reader); // Used to read in the binary data
        void Write(Assimp::StreamWriterLE &writer);

        bool IsCompressed() const;

        bool IsIndexBuffer() const;

        bool IsVertexBuffer() const;

        int GetCompressedDataSize() const;

        int GetUncompressedDataSize() const;

        uint8_t *GetData();


        D3DFORMAT GetIndexFormat();

        void AllocData();

    private:
        std::vector<uint8_t> _data;
    };
}