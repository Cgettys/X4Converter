#pragma once

#include <assimp/StreamReader.h>
#include <assimp/StreamWriter.h>

class XmfMaterial {
public:
    XmfMaterial();

    XmfMaterial(int firstIndex, int numIndices, const std::string &name);

    explicit XmfMaterial(Assimp::StreamReader<> &reader);

    void Write(Assimp::StreamWriter<> &writer);

    int FirstIndex;
    int NumIndices;
    char Name[128];
};
