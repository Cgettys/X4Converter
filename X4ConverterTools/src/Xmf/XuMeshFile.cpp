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
        pIOHandler->Close(pStream);
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

        pMeshFile->ReadHeader(pStream);
        pMeshFile->ReadBufferDescs(pStream);
        pMeshFile->ReadMaterials(pStream);
        pMeshFile->ReadBuffers(pStream);
        pMeshFile->Validate();

        return pMeshFile;
    } catch (...) {
        throw;
    }
}

void XuMeshFile::ReadHeader(IOStream *pStream) {
    if (pStream->FileSize() < sizeof(XmfHeader)) {
        throw std::runtime_error(".xmf file is too small");
    }
    XmfHeader loadedHeader;
    pStream->Read(&loadedHeader, sizeof(loadedHeader), 1);

    if (memcmp(loadedHeader.Magic, "XUMF\x03", 5)) {
        throw std::runtime_error("Invalid header magic");
    }
    if (loadedHeader.BigEndian) {
        throw std::runtime_error(
                "Big endian .xmf files are not supported by this importer");
    }
    if (loadedHeader.DataBufferDescOffset != 0x40) {
        std::cout << loadedHeader.DataBufferDescOffset << std::endl;
        throw std::runtime_error("Offset should be 0x40");
    }
    if (loadedHeader.DataBufferDescSize > sizeof(XmfDataBufferDesc)) {
        throw std::runtime_error("Data buffer description size is too large");
    }
    if (loadedHeader.MaterialSize != sizeof(XmfMaterial)) {
        throw std::runtime_error("Material size is invalid");
    }
    if (loadedHeader.PrimitiveType != D3DPT_TRIANGLELIST) {
        throw std::runtime_error(
                "File is using a DirectX primitive type that's not supported by this importer");
    }
    header = loadedHeader;
}

void XuMeshFile::ReadBufferDescs(IOStream *pStream) {
    if (pStream->FileSize()
        < header.DataBufferDescOffset
          + header.NumDataBuffers * header.DataBufferDescSize)
        throw std::runtime_error(".xmf file is too small");

    buffers.resize(header.NumDataBuffers);

    pStream->Seek(header.DataBufferDescOffset, aiOrigin_SET);
    for (XmfDataBuffer &buffer : buffers) {
        memset(&buffer.Description, 0, sizeof(buffer.Description));
        pStream->Read(&buffer.Description, header.DataBufferDescSize, 1);
        buffer.NormalizeVertexDeclaration();

        if (buffer.Description.NumSections != 1)
            throw std::runtime_error(
                    "Unexpected number of sections (must be 1)");

        if (buffer.IsVertexBuffer()
            && buffer.Description.ItemSize
               != buffer.GetVertexDeclarationSize()) {
            throw std::runtime_error(
                    "Item size for vertex buffer is incorrect");
        } else if (buffer.IsIndexBuffer()) {
            D3DFORMAT format = buffer.GetIndexFormat();
            if ((format == D3DFMT_INDEX16
                 && buffer.Description.ItemSize != sizeof(ushort))
                || (format == D3DFMT_INDEX32
                    && buffer.Description.ItemSize != sizeof(uint))) {
                throw std::runtime_error(
                        "Item size for index buffer is incorrect");
            }
        }
    }
}

void XuMeshFile::ReadMaterials(IOStream *pStream) {
    materials.resize(header.NumMaterials);
    if (header.NumMaterials > 0) {
        pStream->Read(materials.data(), sizeof(XmfMaterial),
                      header.NumMaterials);
    }
}

void XuMeshFile::ReadBuffers(IOStream *pStream) {
    int baseFileOffset = pStream->Tell();
    std::vector<byte> compressedData;
    for (XmfDataBuffer &buffer : buffers) {
        if (pStream->Tell() - baseFileOffset != buffer.Description.DataOffset){
            throw std::runtime_error("Mismatching buffer data offset");
        }
        if (pStream->FileSize() - pStream->Tell()
            < buffer.GetCompressedDataSize()){
            throw std::runtime_error(".xmf file is too small");
        }
        buffer.AllocData();
        if (!buffer.IsCompressed()) {
            if (buffer.GetCompressedDataSize()
                != buffer.GetUncompressedDataSize()){
                throw std::runtime_error(
                        "Noncompressed buffer has invalid size");
            }
            pStream->Read(buffer.GetData(), 1,
                          buffer.GetUncompressedDataSize());
        } else {
            compressedData.reserve(buffer.GetCompressedDataSize());
            pStream->Read(compressedData.data(), 1,
                          buffer.GetCompressedDataSize());

            unsigned long uncompressedSize = buffer.GetUncompressedDataSize();
            int status = uncompress(buffer.GetData(), &uncompressedSize,
                                    compressedData.data(), buffer.GetCompressedDataSize());
            if (status != Z_OK){
                throw std::runtime_error("Failed to decompress data buffer");
            }
            if (uncompressedSize != buffer.GetUncompressedDataSize()) {
                throw std::runtime_error(
                        "Decompression did not return enough data");
            }
        }
    }
}

void XuMeshFile::Validate() {
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
    WriteHeader(pStream);
    WriteBufferDescs(pStream, compressedBuffers);
    WriteMaterials(pStream);
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

void XuMeshFile::WriteHeader(Assimp::IOStream *pStream) {
    // Note that this ignores any header read into this mesh file since we can't guarantee the old header is accurate
    XmfHeader header;
    memcpy(header.Magic, "XUMF", 4);
    header.Version = 3;
    header.BigEndian = false;
    header.DataBufferDescOffset = 0x40;
    header.NumDataBuffers = buffers.size();
    header.DataBufferDescSize = sizeof(XmfDataBufferDesc);
    header.NumMaterials = materials.size();
    header.MaterialSize = sizeof(XmfMaterial);
    header.PrimitiveType = D3DPT_TRIANGLELIST;
    pStream->Write(&header, sizeof(header), 1);

    for (int i = sizeof(header); i < 0x40; ++i) {
        byte pad = 0;
        pStream->Write(&pad, 1, 1);
    }
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

void XuMeshFile::WriteMaterials(Assimp::IOStream *pStream) {
    for (XmfMaterial &material: materials) {
        pStream->Write(&material, sizeof(material), 1);
    }
}

void XuMeshFile::WriteBuffers(Assimp::IOStream *pStream,
                              std::map<XmfDataBuffer *, std::vector<byte> > &compressedBuffers) {
    for (XmfDataBuffer &buffer: buffers) {
        std::vector<byte> &compressedData = compressedBuffers[&buffer];
        pStream->Write(compressedData.data(), 1, compressedData.size());
    }
}
