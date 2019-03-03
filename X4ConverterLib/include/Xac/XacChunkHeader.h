#pragma once

struct XacChunkHeader
{
            XacChunkHeader          ();
            XacChunkHeader          ( int type, int version );

    int     Type;
    int     Length;
    int     Version;
};
