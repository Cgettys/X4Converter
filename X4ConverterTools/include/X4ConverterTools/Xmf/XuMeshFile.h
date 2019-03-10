#pragma once

#include <map>
#include "zlib.h"
#include "../Vendor/half/half.hpp"
#include "../Types.h"
#include <string>
#include <vector>


#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH

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
    std::vector<XmfDataBuffer> &GetBuffers() { return _buffers; };

    XmfDataBuffer *GetIndexBuffer();

    std::vector<XmfVertexElement> GetVertexDeclaration();

    int NumVertices();

    int NumIndices();

    std::vector<XmfMaterial> &GetMaterials() { return _materials; };

    int NumMaterials() { return _materials.size(); }

    void AddMaterial(int firstIndex, int numIndices, const std::string &name);

    static std::shared_ptr<XuMeshFile> ReadFromFile(const std::string &filePath, Assimp::IOSystem *pIOHandler);

    static std::shared_ptr<XuMeshFile> ReadFromIOStream(Assimp::IOStream *pStream);

    void WriteToFile(const std::string &filePath, Assimp::IOSystem *pIOHandler);

    void WriteToIOStream(Assimp::IOStream *pStream);

private:
    static XmfHeader ReadHeader(Assimp::IOStream *pStream);

    void ReadBufferDescs(Assimp::IOStream *pStream, XmfHeader &header);

    void ReadMaterials(Assimp::IOStream *pStream, XmfHeader &header);

    void ReadBuffers(Assimp::IOStream *pStream, XmfHeader &header);

    void Validate();

    std::map<XmfDataBuffer *, std::vector<byte> > CompressBuffers();

    void WriteHeader(Assimp::IOStream *pStream);

    void WriteBufferDescs(Assimp::IOStream *pStream, std::map<XmfDataBuffer *, std::vector<byte> > &compressedBuffers);

    void WriteMaterials(Assimp::IOStream *pStream);

    void WriteBuffers(Assimp::IOStream *pStream, std::map<XmfDataBuffer *, std::vector<byte> > &compressedBuffers);

    std::vector<XmfDataBuffer> _buffers;
    std::vector<XmfMaterial> _materials;
};
