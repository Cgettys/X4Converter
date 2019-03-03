#pragma once

enum D3DPRIMITIVETYPE
{ 
    D3DPT_POINTLIST      = 1,
    D3DPT_LINELIST       = 2,
    D3DPT_LINESTRIP      = 3,
    D3DPT_TRIANGLELIST   = 4,
    D3DPT_TRIANGLESTRIP  = 5,
    D3DPT_TRIANGLEFAN    = 6,
    D3DPT_FORCE_DWORD    = 0x7fffffff
};

enum D3DDECLTYPE
{ 
    D3DDECLTYPE_FLOAT1     = 0,
    D3DDECLTYPE_FLOAT2     = 1,
    D3DDECLTYPE_FLOAT3     = 2,
    D3DDECLTYPE_FLOAT4     = 3,
    D3DDECLTYPE_D3DCOLOR   = 4,
    D3DDECLTYPE_UBYTE4     = 5,
    D3DDECLTYPE_SHORT2     = 6,
    D3DDECLTYPE_SHORT4     = 7,
    D3DDECLTYPE_UBYTE4N    = 8,
    D3DDECLTYPE_SHORT2N    = 9,
    D3DDECLTYPE_SHORT4N    = 10,
    D3DDECLTYPE_USHORT2N   = 11,
    D3DDECLTYPE_USHORT4N   = 12,
    D3DDECLTYPE_UDEC3      = 13,
    D3DDECLTYPE_DEC3N      = 14,
    D3DDECLTYPE_FLOAT16_2  = 15,
    D3DDECLTYPE_FLOAT16_4  = 16,
    D3DDECLTYPE_UNUSED     = 17
};

enum D3DDECLUSAGE
{ 
    D3DDECLUSAGE_POSITION      = 0,
    D3DDECLUSAGE_BLENDWEIGHT   = 1,
    D3DDECLUSAGE_BLENDINDICES  = 2,
    D3DDECLUSAGE_NORMAL        = 3,
    D3DDECLUSAGE_PSIZE         = 4,
    D3DDECLUSAGE_TEXCOORD      = 5,
    D3DDECLUSAGE_TANGENT       = 6,
    D3DDECLUSAGE_BINORMAL      = 7,
    D3DDECLUSAGE_TESSFACTOR    = 8,
    D3DDECLUSAGE_POSITIONT     = 9,
    D3DDECLUSAGE_COLOR         = 10,
    D3DDECLUSAGE_FOG           = 11,
    D3DDECLUSAGE_DEPTH         = 12,
    D3DDECLUSAGE_SAMPLE        = 13
};

enum D3DFORMAT
{
    D3DFMT_UNKNOWN              =  0,

    D3DFMT_R8G8B8               = 20,
    D3DFMT_A8R8G8B8             = 21,
    D3DFMT_X8R8G8B8             = 22,
    D3DFMT_R5G6B5               = 23,
    D3DFMT_X1R5G5B5             = 24,
    D3DFMT_A1R5G5B5             = 25,
    D3DFMT_A4R4G4B4             = 26,
    D3DFMT_R3G3B2               = 27,
    D3DFMT_A8                   = 28,
    D3DFMT_A8R3G3B2             = 29,
    D3DFMT_X4R4G4B4             = 30,
    D3DFMT_A2B10G10R10          = 31,
    D3DFMT_A8B8G8R8             = 32,
    D3DFMT_X8B8G8R8             = 33,
    D3DFMT_G16R16               = 34,
    D3DFMT_A2R10G10B10          = 35,
    D3DFMT_A16B16G16R16         = 36,
    D3DFMT_A8P8                 = 40,
    D3DFMT_P8                   = 41,
    D3DFMT_L8                   = 50,
    D3DFMT_A8L8                 = 51,
    D3DFMT_A4L4                 = 52,
    D3DFMT_V8U8                 = 60,
    D3DFMT_L6V5U5               = 61,
    D3DFMT_X8L8V8U8             = 62,
    D3DFMT_Q8W8V8U8             = 63,
    D3DFMT_V16U16               = 64,
    D3DFMT_A2W10V10U10          = 67,
    D3DFMT_D16_LOCKABLE         = 70,
    D3DFMT_D32                  = 71,
    D3DFMT_D15S1                = 73,
    D3DFMT_D24S8                = 75,
    D3DFMT_D24X8                = 77,
    D3DFMT_D24X4S4              = 79,
    D3DFMT_D16                  = 80,
    D3DFMT_D32F_LOCKABLE        = 82,
    D3DFMT_D24FS8               = 83,
    D3DFMT_L16                  = 81,
    D3DFMT_VERTEXDATA           = 100,
    D3DFMT_INDEX16              = 101,
    D3DFMT_INDEX32              = 102,
    D3DFMT_Q16W16V16U16         = 110,
    D3DFMT_R16F                 = 111,
    D3DFMT_G16R16F              = 112,
    D3DFMT_A16B16G16R16F        = 113,
    D3DFMT_R32F                 = 114,
    D3DFMT_G32R32F              = 115,
    D3DFMT_A32B32G32R32F        = 116,
    D3DFMT_CxV8U8               = 117
};
