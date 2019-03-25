struct XmfFile {
    // 64 byte header
    byte magic[4] = "XUMF";                 // bytes 0 - 3 Xu Mesh File
    byte version = 3;                       // byte  4
    byte bBigEndian = 0;                    // byte  5
    byte dataBufferDescOffset = 0x40;       // byte  6 file offset of first data buffer desc
    byte pad;                               // byte  7 [Maybe padding?]
    byte numDataBuffers = 2;                // byte  8 one vertex buffer, one index buffer
    byte dataBufferDescSize = 0xBC;         // byte  9 size of one data buffer desc
    byte numAttributeRanges = 1;            // byte 10
    byte attributeRangeSize = 0x88;         // byte 11
    byte pad[10];                           // byte 12 - 21 [Not actually padding, at least in part - what is it?]
    int32 primitiveType = 4;                // byte 22 - 25 [suspect, not aligned - is this actually a byte?] D3DPRIMITIVETYPE; determines how the index buffer is used to draw polygons. 4 = triangle list
    byte pad[dataBufferDescOffset - 0x1A]   // byte 26 - 64 [actually padding?]

    // Everything else
    DataBufferDesc descs[h.numDataBuffers]; // describes a DirectX 9 vertex or index buffer
    AttributeRange ranges[h.numAttributeRanges];

    // For each data buffer in descs [couldn't come up with a better way to put it than he did, array of arrays basically, much like the format of the .ANI file in this regard]
    byte bufferData[compressedDataSize]; // content of the DirectX vertex/index buffer, may be zlib-compressed. if bCompressed = 1,
    // the uncompressed size is numSections * itemsPerSection * itemSize
};
struct DataBufferDesc {
    int32 type;                     // bytes 0 - 3
    // Value:
    // 0x1E -> index buffer, otherwise vertex buffer
    // for vertex buffers with numVertexElements = 0, indicates the usage of a single implicit element:
    //   type    D3DDECLUSAGE
    //   0, 1    POSITION
    //   2, 3    NORMAL
    //   4       TANGENT
    //   5       BINORMAL
    //   8       COLOR
    //   20      PSIZE
    //   others  TEXCOORD

    int32 usageIndex;               // bytes 4 - 7   for vertex buffers with numVertexElements = 0, indicates the usage index of the single implicit element
    int32 dataOffset;               // bytes 8 - 11  file offset = dataBufferDescOffset + numDataBuffers*dataBufferDescSize + numAttributeRanges*attributeRangeSize + dataOffset
    int32 bCompressed;              // bytes 12 - 15 if 1, the data is compressed using zlib's compress() function and can be uncompressed with uncompress()
    byte pad[4];                    // bytes 16 - 19 [padding?]
    int32 format;                   // bytes 20 - 23 for index buffers, this is the index format: 0x1E -> 16-bit indices, otherwise 32-bit indices.
    // for vertex buffers with numVertexElements = 0, this is the type (D3DDECLTYPE) of the single implicit element otherwise 0x20

    int32 compressedDataSize;       // bytes 24 - 27 equal to uncompressed size if bCompressed = 0
    int32 itemsPerSection;          // bytes 28 - 31 number of vertices/indices
    int32 itemSize;                 // bytes 32 - 35 size in bytes of a single vertex/index
    int32 numSections = 1;          // bytes 36 - 39
    byte pad[16];                   // bytes 40 - 55
    int32 numVertexElements;        // bytes 56 - 59 for the vertex buffer, number of elements in the DirectX vertex declaration. May be 0,
    // in which case a single declaration is built using the preceding fields.
    VertexDeclElement elements[16]; // bytes fixed size array, only the first numVertexElements items are used. each item is mapped to a D3DVERTEXELEMENT9
};
struct VertexDeclElement {
    int32 type;         // bytes 0 - 3  D3DDECLTYPE
    byte usage;         // byte  4      D3DDECLUSAGE
    byte usageIndex;    // byte  5
    byte pad[2];        // bytes 6 - 7 [Not actually padding]
};
struct AttributeRange {
    // seemingly unused
    int32 mystery0; // bytes 0 - 3
    int32 mystery1; // bytes 4 - 7
    char name[128]; // bytes 8 - 135; sometimes materialCollection + "." + materialName; actual material for the mesh is defined in the mesh .xml in <part>

};