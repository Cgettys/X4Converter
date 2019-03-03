#include "StdInc.h"

XacChunkHeader::XacChunkHeader ()
{
    Type = 0;
    Length = 0;
    Version = 0;
}

XacChunkHeader::XacChunkHeader ( int type, int version )
{
    Type = type;
    Length = 0;
    Version = version;
}
