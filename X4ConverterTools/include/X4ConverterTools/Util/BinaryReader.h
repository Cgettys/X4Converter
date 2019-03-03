#pragma once
#include <assimp/IOStream.hpp>
class BinaryReader {
public:
	BinaryReader(Assimp::IOStream* pStream);

	Assimp::IOStream* GetStream() const;

	template<typename T>
	T Read() {
		if (_pStream->FileSize() - _pStream->Tell() < sizeof(T))
			throw std::string("Attempt to read past end of stream");

		T result;
		_pStream->Read(&result, sizeof(T), 1);
		return result;
	}

	template<typename T>
	void Read(T* pOut, int count) {
		if (_pStream->FileSize() - _pStream->Tell() < sizeof(T) * count)
			throw std::string("Stream is too short");

		_pStream->Read(pOut, sizeof(T), count);
	}

	template<typename T>
	void Read(std::vector<T>& out, int count) {
		out.resize(count);
		if (count > 0)
			Read(out.data(), count);
	}

	template<typename T>
	void Skip(int count) {
		if (_pStream->FileSize() - _pStream->Tell() < count * sizeof(T))
			throw std::string("Attempt to skip past end of stream");

		_pStream->Seek(count * sizeof(T), aiOrigin_CUR);
	}

	byte ReadByte();
	short ReadInt16();
	int ReadInt32();

	ushort ReadUInt16();
	uint ReadUInt32();

	std::string ReadString();

	bool IsAtEnd();

private:
	Assimp::IOStream* _pStream;
};
