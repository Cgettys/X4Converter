#pragma once

#include <map>
#include "zlib.h"
#include "../Vendor/half/half.hpp"
#include "../Types.h"
#include <string>
#include <vector>


#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>

#include "../DirectX.h"
#include "../Util/DXUtil.h"
#include "../Util/BinaryReader.h"
#include "../Util/BinaryWriter.h"

#include "XmfHeader.h"
#include "XmfVertexElement.h"
#include "XmfMaterial.h"
#include "XmfDataBufferDesc.h"
#include "XmfDataBuffer.h"

class XuMeshFile {
public:
    // TODO something better that takes into account writing?
    XmfHeader GetHeader() { return header; };

    std::vector<XmfDataBuffer> &GetBuffers() { return buffers; };

    XmfDataBuffer *GetIndexBuffer();

    std::vector<XmfVertexElement> GetVertexDeclaration();


    int NumVertices();

    int NumIndices();

    std::vector<XmfMaterial> &GetMaterials() { return materials; };

    int NumMaterials() { return materials.size(); }

    void AddMaterial(int firstIndex, int numIndices, const std::string &name);

    static std::shared_ptr<XuMeshFile> ReadFromFile(const std::string &filePath, Assimp::IOSystem *pIOHandler);

    static std::shared_ptr<XuMeshFile> ReadFromIOStream(Assimp::IOStream *pStream);

    void WriteToFile(const std::string &filePath, Assimp::IOSystem *pIOHandler);

    void WriteToIOStream(Assimp::IOStream *pStream);

private:
    void ReadHeader(Assimp::IOStream *pStream);

    void ReadBufferDescs(Assimp::IOStream *pStream);

    void ReadMaterials(Assimp::IOStream *pStream);

    void ReadBuffers(Assimp::IOStream *pStream);

    void Validate();

    std::map<XmfDataBuffer *, std::vector<byte> > CompressBuffers();

    void WriteHeader(Assimp::IOStream *pStream);

    void WriteBufferDescs(Assimp::IOStream *pStream, std::map<XmfDataBuffer *, std::vector<byte> > &compressedBuffers);

    void WriteMaterials(Assimp::IOStream *pStream);

    void WriteBuffers(Assimp::IOStream *pStream, std::map<XmfDataBuffer *, std::vector<byte> > &compressedBuffers);

    XmfHeader header;
    std::vector<XmfDataBuffer> buffers;
    std::vector<XmfMaterial> materials;
};
