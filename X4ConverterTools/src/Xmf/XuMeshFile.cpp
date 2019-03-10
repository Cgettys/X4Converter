#include <X4ConverterTools/Xmf/XuMeshFile.h>
using namespace Assimp;
using namespace boost;

XmfDataBuffer* XuMeshFile::GetIndexBuffer() {
	for (auto it = _buffers.begin(); it != _buffers.end(); ++it) {
		if (it->IsIndexBuffer())
			return &(*it);
	}
	return NULL;
}

std::vector<XmfVertexElement> XuMeshFile::GetVertexDeclaration() {
	std::vector<XmfVertexElement> result;
	for (auto it = _buffers.begin(); it != _buffers.end(); ++it) {
		for (int i = 0; i < it->Description.NumVertexElements; ++i) {
			result.push_back(it->Description.VertexElements[i]);
		}
	}
	return result;
}

int XuMeshFile::NumVertices() {
	for (auto it = _buffers.begin(); it != _buffers.end(); ++it) {
		if (it->IsVertexBuffer())
			return it->Description.NumItemsPerSection;
	}
	return 0;
}

int XuMeshFile::NumIndices() {
	XmfDataBuffer* pIndexBuffer = GetIndexBuffer();
	return pIndexBuffer != NULL ?
			pIndexBuffer->Description.NumItemsPerSection : 0;
}

void XuMeshFile::AddMaterial(int firstIndex, int numIndices,
		const std::string& name) {
	_materials.push_back(XmfMaterial(firstIndex, numIndices, name));
}
std::shared_ptr<XuMeshFile> XuMeshFile::ReadFromFile(
		const std::string& filePath, Assimp::IOSystem* pIOHandler) {
	IOStream* pStream;
	try {
		pStream = pIOHandler->Open(filePath, "rb");
		auto result = XuMeshFile::ReadFromIOStream(pStream);
		pIOHandler->Close(pStream);
		return result;
	} catch (...) {
		pIOHandler->Close(pStream);
		throw;
	}
}
std::shared_ptr<XuMeshFile> XuMeshFile::ReadFromIOStream(IOStream* pStream) {
	if (!pStream) {
		// TODO exception?
		std::cerr << "Warning: no IOStream provided";
		return std::shared_ptr<XuMeshFile>();
	}
	try {
		std::shared_ptr<XuMeshFile> pMeshFile = std::make_shared<XuMeshFile>();

		XmfHeader header = ReadHeader(pStream);
		pMeshFile->ReadBufferDescs(pStream, header);
		pMeshFile->ReadMaterials(pStream, header);
		pMeshFile->ReadBuffers(pStream, header);
		pMeshFile->Validate();

		return pMeshFile;
	} catch (...) {
		throw;
	}
}

XmfHeader XuMeshFile::ReadHeader(IOStream* pStream) {
	if (pStream->FileSize() < sizeof(XmfHeader)) {
		throw std::runtime_error(".xmf file is too small");
	}
	XmfHeader header;
	pStream->Read(&header, sizeof(header), 1);

	if (memcmp(header.Magic, "XUMF\x03", 5)) {
		throw std::runtime_error("Invalid header magic");
	}
	if (header.BigEndian) {
		throw std::runtime_error(
				"Big endian .xmf files are not supported by this importer");
	}
	if (header.DataBufferDescOffset != 0x40) {
		std::cout << header.DataBufferDescOffset << std::endl;
		throw std::runtime_error("Offset should be 0x40");
	}
	if (header.DataBufferDescSize > sizeof(XmfDataBufferDesc)) {
		throw std::runtime_error("Data buffer description size is too large");
	}
	if (header.MaterialSize != sizeof(XmfMaterial)) {
		throw std::runtime_error("Material size is invalid");
	}
	if (header.PrimitiveType != D3DPT_TRIANGLELIST) {
		throw std::runtime_error(
				"File is using a DirectX primitive type that's not supported by this importer");
	}
	return header;
}

void XuMeshFile::ReadBufferDescs(IOStream* pStream, XmfHeader& header) {
	if (pStream->FileSize()
			< header.DataBufferDescOffset
					+ header.NumDataBuffers * header.DataBufferDescSize)
		throw std::runtime_error(".xmf file is too small");

	_buffers.resize(header.NumDataBuffers);

	pStream->Seek(header.DataBufferDescOffset, aiOrigin_SET);
	for (XmfDataBuffer& buffer : _buffers) {
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

void XuMeshFile::ReadMaterials(IOStream* pStream, XmfHeader& header) {
	_materials.resize(header.NumMaterials);
	if (header.NumMaterials > 0)
		pStream->Read(_materials.data(), sizeof(XmfMaterial),
				header.NumMaterials);
}

void XuMeshFile::ReadBuffers(IOStream* pStream, XmfHeader& header) {
	int baseFileOffset = pStream->Tell();
	std::vector<byte> compressedData;
	for (XmfDataBuffer& buffer : _buffers) {
		if (pStream->Tell() - baseFileOffset != buffer.Description.DataOffset)
			throw std::runtime_error("Mismatching buffer data offset");

		if (pStream->FileSize() - pStream->Tell()
				< buffer.GetCompressedDataSize())
			throw std::runtime_error(".xmf file is too small");

		buffer.AllocData();
		if (!buffer.IsCompressed()) {
			if (buffer.GetCompressedDataSize()
					!= buffer.GetUncompressedDataSize())
				throw std::runtime_error(
						"Noncompressed buffer has invalid size");

			pStream->Read(buffer.GetData(), 1,
					buffer.GetUncompressedDataSize());
		} else {
			compressedData.reserve(buffer.GetCompressedDataSize());
			pStream->Read(compressedData.data(), 1,
					buffer.GetCompressedDataSize());

			unsigned long uncompressedSize = buffer.GetUncompressedDataSize();
			int status = uncompress(buffer.GetData(), &uncompressedSize,
					compressedData.data(), buffer.GetCompressedDataSize());
			if (status != Z_OK)
				throw std::runtime_error("Failed to decompress data buffer");

			if (uncompressedSize != buffer.GetUncompressedDataSize())
				throw std::runtime_error(
						"Decompression did not return enough data");
		}
	}
}

void XuMeshFile::Validate() {
	int numVertices = -1;
	for (XmfDataBuffer& buffer : _buffers) {
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

void XuMeshFile::WriteToFile(const std::string& filePath,
		IOSystem* pIOHandler) {
	IOStream* pStream = pIOHandler->Open(filePath, "wb+");
	if (!pStream) {
		throw(format("Failed to open %1 for writing") % filePath).str();
	}
	WriteToIOStream(pStream);
	pIOHandler->Close(pStream);
}
void XuMeshFile::WriteToIOStream(IOStream* pStream) {
	std::map<XmfDataBuffer*, std::vector<byte> > compressedBuffers =
			CompressBuffers();
	WriteHeader(pStream);
	WriteBufferDescs(pStream, compressedBuffers);
	WriteMaterials(pStream);
	WriteBuffers(pStream, compressedBuffers);
}

std::map<XmfDataBuffer*, std::vector<byte> > XuMeshFile::CompressBuffers() {
	std::map<XmfDataBuffer*, std::vector<byte> > result;
	for(XmfDataBuffer & buffer: _buffers)
	{
		std::vector<byte>& compressedData = result[&buffer];
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

void XuMeshFile::WriteHeader(Assimp::IOStream* pStream) {
	XmfHeader header;
	memcpy(header.Magic, "XUMF", 4);
	header.Version = 3;
	header.BigEndian = false;
	header.DataBufferDescOffset = 0x40;
	header.NumDataBuffers = _buffers.size();
	header.DataBufferDescSize = sizeof(XmfDataBufferDesc);
	header.NumMaterials = _materials.size();
	header.MaterialSize = sizeof(XmfMaterial);
	header.PrimitiveType = D3DPT_TRIANGLELIST;
	pStream->Write(&header, sizeof(header), 1);

	for (int i = sizeof(header); i < 0x40; ++i) {
		byte pad = 0;
		pStream->Write(&pad, 1, 1);
	}
}

void XuMeshFile::WriteBufferDescs(Assimp::IOStream* pStream,
		std::map<XmfDataBuffer*, std::vector<byte> >& compressedBuffers) {
	int dataOffset = 0;
	for(XmfDataBuffer & buffer: _buffers)
	{
		buffer.Description.DataOffset = dataOffset;
		buffer.Description.Compressed = 1;
		buffer.Description.CompressedDataSize =
				compressedBuffers[&buffer].size();
		pStream->Write(&buffer.Description, sizeof(buffer.Description), 1);
		dataOffset += buffer.Description.CompressedDataSize;
	}
}

void XuMeshFile::WriteMaterials(Assimp::IOStream* pStream) {
	for(XmfMaterial & material: _materials)
	{
		pStream->Write(&material, sizeof(material), 1);
	}
}

void XuMeshFile::WriteBuffers(Assimp::IOStream* pStream,
		std::map<XmfDataBuffer*, std::vector<byte> >& compressedBuffers) {
	for(XmfDataBuffer & buffer: _buffers)
	{
		std::vector<byte>& compressedData = compressedBuffers[&buffer];
		pStream->Write(compressedData.data(), 1, compressedData.size());
	}
}
