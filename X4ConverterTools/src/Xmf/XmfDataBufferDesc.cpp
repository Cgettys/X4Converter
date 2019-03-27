//
// Created by cg on 3/25/19.
//
#include <X4ConverterTools/Xmf/XmfDataBufferDesc.h>

using namespace Assimp;
using namespace boost;

XmfDataBufferDesc::XmfDataBufferDesc(StreamReaderLE &reader) {
    reader >> Type;
    reader >> UsageIndex;
    reader >> DataOffset;
    reader >> Compressed;
    for (byte &b : _pad0) {
        reader >> b;
    }
    reader >> Format;
    reader >> CompressedDataSize;
    reader >> NumItemsPerSection;
    reader >> ItemSize;
    reader >> NumSections;
    for (byte &b : _pad1) {
        reader >> b;
    }
    reader >> NumVertexElements;
    for (XmfVertexElement &e : VertexElements) {
        e = XmfVertexElement(reader);
    }
    //TODO make this validation code?
//    for (byte &b : _pad0) {
//        if (b != 0) {
//            throw std::runtime_error("Non-zero pad byte in _pad0");
//        }
//    }
//
//    for (byte &b : _pad1) {
//        if (b != 0) {
//            std::cout << (int) b << std::endl;
//            throw std::runtime_error("Non-zero pad byte in _pad1");
//        }
//    }
    NormalizeVertexDeclaration();
    validate();
}

bool XmfDataBufferDesc::IsVertexBuffer() const {
    return !IsIndexBuffer();
}

bool XmfDataBufferDesc::IsIndexBuffer() const {
    return Type == 30;
}

void XmfDataBufferDesc::NormalizeVertexDeclaration() {
    if (!IsVertexBuffer() || NumVertexElements > 0)
        return;

    NumVertexElements = 1;
    VertexElements[0].Type = Format;
    switch (Type) {
        case 0:
        case 1:
            VertexElements[0].Usage = D3DDECLUSAGE_POSITION;
            break;

        case 2:
        case 3:
            VertexElements[0].Usage = D3DDECLUSAGE_NORMAL;
            break;

        case 4:
            VertexElements[0].Usage = D3DDECLUSAGE_TANGENT;
            break;

        case 5:
            VertexElements[0].Usage = D3DDECLUSAGE_BINORMAL;
            break;

        case 8:
            VertexElements[0].Usage = D3DDECLUSAGE_COLOR;
            break;

        case 20:
            VertexElements[0].Usage = D3DDECLUSAGE_PSIZE;
            break;

        default:
            VertexElements[0].Usage = D3DDECLUSAGE_TEXCOORD;
            break;
    }
    VertexElements[0].UsageIndex = UsageIndex;

    Type = 0;
    UsageIndex = 0;
    Format = 32;
}

void XmfDataBufferDesc::DenormalizeVertexDeclaration() {
    if (!IsVertexBuffer() || NumVertexElements != 1)
        return;

    Format = VertexElements[0].Type;
    switch (VertexElements[0].Usage) {
        case D3DDECLUSAGE_POSITION:
            Type = 0;
            break;

        case D3DDECLUSAGE_NORMAL:
            Type = 2;
            break;

        case D3DDECLUSAGE_TANGENT:
            Type = 4;
            break;

        case D3DDECLUSAGE_BINORMAL:
            Type = 5;
            break;

        case D3DDECLUSAGE_COLOR:
            Type = 8;
            break;

        case D3DDECLUSAGE_PSIZE:
            Type = 20;
            break;

        default:
            Type = 31;
            break;
    }
    UsageIndex = VertexElements[0].UsageIndex;
    NumVertexElements = 0;
}

int XmfDataBufferDesc::GetVertexDeclarationSize() {
    int size = 0;
    for (int i = 0; i < NumVertexElements; ++i) {
        size += DXUtil::GetVertexElementTypeSize((D3DDECLTYPE) VertexElements[i].Type);
    }
    return size;
}

D3DFORMAT XmfDataBufferDesc::GetIndexFormat() {
    if (!IsIndexBuffer()) {
        throw std::runtime_error("The data buffer is not an index buffer");
    }
    return Format == 30 ? D3DFMT_INDEX16 : D3DFMT_INDEX32;
}

void XmfDataBufferDesc::Write(Assimp::StreamWriterLE &writer) {

}

std::string XmfDataBufferDesc::validate() {
    std::string ret = "DataBufferDesc:\n";
    bool valid = true;

    ret.append(str(format("\tType: %1%\n") % Type));
    ret.append(str(format("\tUsageIndex: %1%\n") % UsageIndex));
    ret.append(str(format("\tDataOffset: %1%\n") % DataOffset));
    ret.append(str(format("\tCompressed: %1%\n") % Compressed));
    //TODO ret.append(str(format("(w*): %1%\n")%Pad));
    ret.append(str(format("\tFormat: %1%\n") % Format));
    ret.append(str(format("\tCompressedDataSize: %1%\n") % CompressedDataSize));
    ret.append(str(format("\tNumItemsPerSection: %1%\n") % NumItemsPerSection));
    ret.append(str(format("\tItemSize: %1%\n") % ItemSize));
    ret.append(str(format("\tNumSections: %1%\n") % NumSections));
    // TODO more padding
    ret.append(str(format("\tNumVertexElements: %1%\n") % NumVertexElements));
    // TODO Vertex elements
    if (NumSections != 1) {
        ret.append("\tERROR: Unexpected number of sections (must be 1)");
        valid = false;
    }
    if (IsVertexBuffer() && ItemSize != GetVertexDeclarationSize()) {
        ret.append("\tERROR: Item size for vertex buffer is incorrect");
        valid = false;
    } else if (IsIndexBuffer()) {
        D3DFORMAT format = GetIndexFormat();
        if ((format == D3DFMT_INDEX16 && ItemSize != sizeof(ushort)) ||
            (format == D3DFMT_INDEX32 && ItemSize != sizeof(uint))) {
            ret.append("\tERROR: Item size for index buffer is incorrect");
            valid = false;
        }
    }
    if (!valid) {
        throw std::runtime_error(ret);
    }
    return ret;
}
