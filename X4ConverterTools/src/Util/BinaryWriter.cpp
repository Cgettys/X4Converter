#include <X4ConverterTools/StdInc.h>

BinaryWriter::BinaryWriter ( Assimp::IOStream* pStream )
{
    _pStream = pStream;
}

Assimp::IOStream* BinaryWriter::GetStream () const
{
    return _pStream;
}
