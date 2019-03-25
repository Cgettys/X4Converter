#pragma once

#include <assimp/StreamReader.h>
#include <assimp/StreamWriter.h>
#include <boost/format.hpp>
#include <iostream>

#include "XmfMaterial.h"
#include "XmfDataBufferDesc.h"
#include "../DirectX.h"

class XmfHeader {
public:
    XmfHeader() = default;

    XmfHeader(byte NumDataBuffer, byte NumMaterials);
    explicit XmfHeader(Assimp::StreamReader<> &reader);

    void Write(Assimp::StreamWriter<> &writer);
    std::string validate() const;// Debug method - throws exception if invalid, else returns human readable string
    byte Magic[4];
    byte Version;
    bool BigEndian;
    byte DataBufferDescOffset;
    byte _pad0 = 0;
    byte NumDataBuffers;
    byte DataBufferDescSize;
    byte NumMaterials;
    byte MaterialSize;
    byte _pad1[10];
    dword PrimitiveType;
    const static byte BUFFER_OFFSET = 0x40;// If it's not 0x40 we have an issue
    byte _pad2[BUFFER_OFFSET-0x1A];
};

