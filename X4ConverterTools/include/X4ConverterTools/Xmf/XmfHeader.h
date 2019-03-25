#pragma once

#pragma once

#include <assimp/StreamReader.h>
#include <boost/format.hpp>
#include <iostream>


#include "XmfMaterial.h"
#include "XmfDataBufferDesc.h"
#include "../DirectX.h"

class XmfHeader {
public:
    XmfHeader() = default;

    explicit XmfHeader(Assimp::StreamReader<> &reader);

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
    byte _pad1[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    dword PrimitiveType;
};

