#include "StdInc.h"

BinaryReader::BinaryReader ( Assimp::IOStream* pStream )
{
    _pStream = pStream;
}

Assimp::IOStream* BinaryReader::GetStream () const
{
    return _pStream;
}

byte BinaryReader::ReadByte ()
{
    return Read < byte > ();
}

short BinaryReader::ReadInt16 ()
{
    return Read < short > ();
}

int BinaryReader::ReadInt32 ()
{
    return Read < int > ();
}

ushort BinaryReader::ReadUInt16 ()
{
    return Read < ushort > ();
}

uint BinaryReader::ReadUInt32 ()
{
    return Read < uint> ();
}

std::string BinaryReader::ReadString ()
{
    int length = ReadInt32 ();
    std::string result;
    result.resize ( length );
    if ( length > 0 )
        Read ( const_cast < char* > ( result.data () ), length );
    
    return result;
}

bool BinaryReader::IsAtEnd ()
{
    return _pStream->Tell () == _pStream->FileSize ();
}
