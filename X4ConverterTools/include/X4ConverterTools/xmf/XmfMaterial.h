#pragma once

#include <assimp/StreamReader.h>
#include <assimp/StreamWriter.h>

namespace xmf {
    class XmfMaterial {
    public:
        XmfMaterial();

        XmfMaterial(int firstIndex, int numIndices, const std::string &name);

        XmfMaterial(Assimp::StreamReaderLE &reader);

        void Write(Assimp::StreamWriterLE &writer);

        int FirstIndex;
        int NumIndices;
        char Name[128];
    };
}