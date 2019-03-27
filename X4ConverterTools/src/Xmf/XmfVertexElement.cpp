#include <X4ConverterTools/StdInc.h>
using namespace boost;
XmfVertexElement::XmfVertexElement() {
    Type = D3DDECLTYPE_UNUSED;
}

XmfVertexElement::XmfVertexElement(D3DDECLTYPE type, D3DDECLUSAGE usage, byte usageIndex) {
    Type = type;
    Usage = usage;
    UsageIndex = usageIndex;
}

XmfVertexElement::XmfVertexElement(Assimp::StreamReaderLE &reader) {
    reader >> Type;
    reader >> Usage;
    reader >> UsageIndex;
    reader >> _pad0[0] >> _pad0[1];

//    if (_pad0[0]!=0 || _pad0[1] !=0){
//        std::cout << str(format("%1% %2%") % (int)_pad0[0] % (int)_pad0[1])<<std::endl;
//        throw std::runtime_error("_pad0 must be 0!");
//    }
}

void XmfVertexElement::Write(Assimp::StreamWriterLE &writer) {
    // TODO
}

