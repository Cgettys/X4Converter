#pragma once

#include <assimp/StreamReader.h>
#include <assimp/StreamWriter.h>
#include <X4ConverterTools/Types.h>
#include <X4ConverterTools/Xmf/XmfDataBufferDesc.h>
#include "zlib.h"
class XmfDataBuffer {
public:
    XmfDataBuffer();

    XmfDataBufferDesc Description;
    explicit XmfDataBuffer(Assimp::StreamReader<> &descReader);

    void Read(Assimp::StreamReader<> &reader); // Used to read in the binary data
    void Write(Assimp::StreamWriter<> &writer);

    bool IsCompressed() const;

    bool IsIndexBuffer() const;

    bool IsVertexBuffer() const;

    int GetCompressedDataSize() const;

    int GetUncompressedDataSize() const;

    byte *GetData();


    D3DFORMAT GetIndexFormat();

    void AllocData();
private:
    std::vector<byte> _data;
};
