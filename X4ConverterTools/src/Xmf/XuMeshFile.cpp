#include <X4ConverterTools/Xmf/XuMeshFile.h>

using namespace Assimp;
using namespace boost;

XmfDataBuffer *XuMeshFile::GetIndexBuffer() {
    for (auto &_buffer : buffers) {
        if (_buffer.IsIndexBuffer())
            return &_buffer;
    }
    return nullptr;
}

std::vector<XmfVertexElement> XuMeshFile::GetVertexDeclaration() {
    std::vector<XmfVertexElement> result;
    for (auto &_buffer : buffers) {
        for (int i = 0; i < _buffer.Description.NumVertexElements; ++i) {
            result.push_back(_buffer.Description.VertexElements[i]);
        }
    }
    return result;
}

int XuMeshFile::NumVertices() {
    for (auto &_buffer : buffers) {
        if (_buffer.IsVertexBuffer())
            return _buffer.Description.NumItemsPerSection;
    }
    return 0;
}

int XuMeshFile::NumIndices() {
    XmfDataBuffer *pIndexBuffer = GetIndexBuffer();
    return pIndexBuffer != nullptr ?
           pIndexBuffer->Description.NumItemsPerSection : 0;
}

void XuMeshFile::AddMaterial(int firstIndex, int numIndices,
                             const std::string &name) {
    materials.emplace_back(firstIndex, numIndices, name);
}

std::shared_ptr<XuMeshFile> XuMeshFile::ReadFromFile(
        const std::string &filePath, Assimp::IOSystem *pIOHandler) {
    IOStream *pStream;
    try {
        pStream = pIOHandler->Open(filePath, "rb");
        auto result = XuMeshFile::ReadFromIOStream(pStream);
//        pIOHandler->Close(pStream);
        return result;
    } catch (...) {
        throw;
    }
}

std::shared_ptr<XuMeshFile> XuMeshFile::ReadFromIOStream(IOStream *pStream) {
    if (!pStream) {
        // TODO exception?
        std::cerr << "Warning: no IOStream provided";
        return std::shared_ptr<XuMeshFile>();
    }
    try {
        std::shared_ptr<XuMeshFile> pMeshFile = std::make_shared<XuMeshFile>();

        if (pStream->FileSize() < 16) {
            throw std::runtime_error(".xmf file is too small (< 16 bytes)");
        }

        auto pStreamReader = StreamReaderLE(pStream, false);
        pMeshFile->header = XmfHeader(pStreamReader);

        // Read in Buffer descs
        if (pStream->FileSize() < pMeshFile->header.SizeOfHeader
              + pMeshFile->header.NumDataBuffers * pMeshFile->header.DataBufferDescSize) {
            throw std::runtime_error(".xmf file is too small based on header");
        }

        pMeshFile->ReadBufferDescs(pStreamReader);
        // Materials
        for (int i = 0; i < pMeshFile->header.NumMaterials; i ++){
            pMeshFile->materials.emplace_back(pStreamReader);
        }

        // Buffers
        pMeshFile->ReadBuffers(pStreamReader);
        pMeshFile->Validate();

        return pMeshFile;
    } catch (...) {
        throw;
    }
}

void XuMeshFile::ReadBufferDescs(StreamReaderLE& pStreamReader) {


//    pStreamReader.SetCurrentPos(header.SizeOfHeader);
    for (int x = 0; x < header.NumDataBuffers; x++) {
        buffers.emplace_back(pStreamReader);
    }
}

void XuMeshFile::ReadBuffers(StreamReaderLE & pStreamReader) {
    for (XmfDataBuffer &buffer : buffers) {
        buffer.Read(pStreamReader);
    }
}

void XuMeshFile::Validate() {
    header.validate();
    int numVertices = -1;
    for (XmfDataBuffer &buffer : buffers) {
        if (!buffer.IsVertexBuffer()) {
            continue;
        }
        if (numVertices == -1) {
            numVertices = buffer.Description.NumItemsPerSection;
        } else if (buffer.Description.NumItemsPerSection != numVertices) {
            throw std::runtime_error(
                    "Inconsistent vertex count across vertex buffers");
        }
    }
}

void XuMeshFile::WriteToFile(const std::string &filePath,
                             IOSystem *pIOHandler) {
    IOStream *pStream = pIOHandler->Open(filePath, "wb+");
    if (!pStream) {
        throw std::runtime_error((format("Failed to open %1% for writing") % filePath.c_str()).str());
    }
    WriteToIOStream(pStream);
    pIOHandler->Close(pStream);
}

void XuMeshFile::WriteToIOStream(IOStream *pStream) {
    std::map<XmfDataBuffer *, std::vector<byte> > compressedBuffers =
            CompressBuffers();

    header= XmfHeader(buffers.size(),materials.size());
    auto pStreamWriter = StreamWriter<>(pStream, false);
    header.Write(pStreamWriter);
    pStreamWriter.Flush();

    WriteBufferDescs(pStream, compressedBuffers);
    for (XmfMaterial &material: materials) {
        material.Write(pStreamWriter);
    }
    pStreamWriter.Flush();
    WriteBuffers(pStream, compressedBuffers);
}

std::map<XmfDataBuffer *, std::vector<byte> > XuMeshFile::CompressBuffers() {
    std::map<XmfDataBuffer *, std::vector<byte> > result;
    for (XmfDataBuffer &buffer: buffers) {
        std::vector<byte> &compressedData = result[&buffer];
        compressedData.resize(compressBound(buffer.GetUncompressedDataSize()));

        ulong compressedSize = compressedData.size();
        int status = compress(compressedData.data(), &compressedSize,
                              buffer.GetData(), buffer.GetUncompressedDataSize());
        if (status != Z_OK)
            throw std::runtime_error("Failed to compress XMF data buffer");

        compressedData.resize(compressedSize);
    }
    return result;
}


void XuMeshFile::WriteBufferDescs(Assimp::IOStream *pStream,
                                  std::map<XmfDataBuffer *, std::vector<byte> > &compressedBuffers) {
    int dataOffset = 0;
    for (XmfDataBuffer &buffer: buffers) {
        buffer.Description.DataOffset = dataOffset;
        buffer.Description.Compressed = 1;
        buffer.Description.CompressedDataSize =
                compressedBuffers[&buffer].size();
        pStream->Write(&buffer.Description, sizeof(buffer.Description), 1);
        dataOffset += buffer.Description.CompressedDataSize;
    }
}

void XuMeshFile::WriteBuffers(Assimp::IOStream *pStream,
                              std::map<XmfDataBuffer *, std::vector<byte> > &compressedBuffers) {
    for (XmfDataBuffer &buffer: buffers) {
        std::vector<byte> &compressedData = compressedBuffers[&buffer];
        pStream->Write(compressedData.data(), 1, compressedData.size());
    }
}
