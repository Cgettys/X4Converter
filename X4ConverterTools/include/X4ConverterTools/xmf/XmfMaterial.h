#pragma once

#include <assimp/StreamReader.h>
#include <assimp/StreamWriter.h>
#include <boost/format.hpp>

namespace xmf {
    class XmfMaterial {
    public:
        XmfMaterial();

        XmfMaterial(int firstIndex, int numIndices, const std::string &name);

        explicit XmfMaterial(Assimp::StreamReaderLE &reader);

        void Write(Assimp::StreamWriterLE &writer);

        int FirstIndex;
        int NumIndices;
        char Name[128];
    };
}