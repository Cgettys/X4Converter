#include <X4ConverterTools/StdInc.h>
#include <X4ConverterTools/Xmf/XmfMaterial.h>


using namespace boost;

XmfMaterial::XmfMaterial() {
    FirstIndex = 0;
    NumIndices = 0;
    memset(Name, 0, sizeof(Name));
}

XmfMaterial::XmfMaterial(int firstIndex, int numIndices, const std::string &name) {
    if (name.size() >= sizeof(Name))
        throw std::runtime_error(str(format("Material name %s is too long") % name));

    FirstIndex = firstIndex;
    NumIndices = numIndices;
    memset(Name, 0, sizeof(Name));
    memcpy(Name, name.c_str(), name.size());
}

XmfMaterial::XmfMaterial(Assimp::StreamReaderLE &reader) {
    reader >> FirstIndex;
    reader >> NumIndices;
    for (char &c : Name) {
        reader >> c;
    }

}

void XmfMaterial::Write(Assimp::StreamWriter<> &writer) {
    writer << FirstIndex;
    writer << NumIndices;
    for (char &c : Name) {
        writer << c;
    }
}
