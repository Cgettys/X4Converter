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
    byte Magic[4];                      // bytes   0 - 3; identifies the file
    byte Version;                       // byte        4; file version
    byte IsBigEndian;                   // byte        5; if == 0 we have little endian (default)
    byte SizeOfHeader;                  // byte        6; sizeof(XU_MESH_FILE_HEADER), should be 64byte
    byte reserved0 = 0;                 // byte        7; Officially reserved
    byte NumDataBuffers;                // byte        8; how many databuffers this file has stored
    byte DataBufferDescSize;            // byte        9; sizeof(DATA_BUFFER_DESC)
    byte NumMaterials;                  // byte       10; how many attribute ranges we have stored TODO rename
    byte MaterialSize;                  // byte       11; sizeof(ATTRIBUTE_TABLE_DESC) TODO rename

    byte Culling_CW;                    // byte       12; false == CCW, other is CW TODO validate
    byte RightHand;                     // byte       13; 0 == lefthand D3D convention, other is righthand openGL format todo validate
    dword NumVertices;                  // bytes 14 - 17; TODO use for validation
    dword NumIndices;                   // bytes 18 - 21; TODO use for validation

    dword PrimitiveType;                // bytes 22 - 25;
    dword MeshOptimization;	            // bytes 26 - 29; MeshOptimizationType - enum MeshOptimizationType applied
    float BoundingBoxCenter[3];		    // bytes 30 - 41; virtual center of the mesh TODO calculate
    float BoundingBoxSize[3];		    // bytes 42 - 53; max absolute extents from the center (aligned coords) TODO calculate
    byte pad[10];                       // bytes 54 - 63

    const static byte EXPECTED_HEADER_SIZE = 0x40;// If it's not 0x40 we have an issue

};

