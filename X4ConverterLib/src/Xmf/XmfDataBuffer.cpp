#include "StdInc.h"

XmfDataBuffer::XmfDataBuffer ()
{
    memset ( &Description, 0, sizeof(Description) );
}

void XmfDataBuffer::AllocData ()
{
    _data.resize ( GetUncompressedDataSize () );
}

bool XmfDataBuffer::IsCompressed () const
{
    return Description.Compressed != 0;
}

bool XmfDataBuffer::IsIndexBuffer () const
{
    return Description.Type == 30;
}

bool XmfDataBuffer::IsVertexBuffer () const
{
    return !IsIndexBuffer ();
}

int XmfDataBuffer::GetCompressedDataSize () const
{
    return Description.CompressedDataSize;
}

int XmfDataBuffer::GetUncompressedDataSize () const
{
    return Description.NumSections * Description.NumItemsPerSection * Description.ItemSize;
}

byte* XmfDataBuffer::GetData ()
{
    return _data.data ();
}

void XmfDataBuffer::NormalizeVertexDeclaration ()
{
    if ( !IsVertexBuffer () || Description.NumVertexElements > 0 )
        return;

    Description.NumVertexElements = 1;
    Description.VertexElements[0].Type = Description.Format;
    switch ( Description.Type )
    {
        case 0:
        case 1:
            Description.VertexElements[0].Usage = D3DDECLUSAGE_POSITION;
            break;

        case 2:
        case 3:
            Description.VertexElements[0].Usage = D3DDECLUSAGE_NORMAL;
            break;

        case 4:
            Description.VertexElements[0].Usage = D3DDECLUSAGE_TANGENT;
            break;

        case 5:
            Description.VertexElements[0].Usage = D3DDECLUSAGE_BINORMAL;
            break;

        case 8:
            Description.VertexElements[0].Usage = D3DDECLUSAGE_COLOR;
            break;

        case 20:
            Description.VertexElements[0].Usage = D3DDECLUSAGE_PSIZE;
            break;

        default:
            Description.VertexElements[0].Usage = D3DDECLUSAGE_TEXCOORD;
            break;
    }
    Description.VertexElements[0].UsageIndex = Description.UsageIndex;

    Description.Type = 0;
    Description.UsageIndex = 0;
    Description.Format = 32;
}

void XmfDataBuffer::DenormalizeVertexDeclaration ()
{
    if ( !IsVertexBuffer () || Description.NumVertexElements != 1 )
        return;

    Description.Format = Description.VertexElements[0].Type;
    switch ( Description.VertexElements[0].Usage )
    {
        case D3DDECLUSAGE_POSITION:
            Description.Type = 0;
            break;

        case D3DDECLUSAGE_NORMAL:
            Description.Type = 2;
            break;

        case D3DDECLUSAGE_TANGENT:
            Description.Type = 4;
            break;

        case D3DDECLUSAGE_BINORMAL:
            Description.Type = 5;
            break;

        case D3DDECLUSAGE_COLOR:
            Description.Type = 8;
            break;

        case D3DDECLUSAGE_PSIZE:
            Description.Type = 20;

        default:
            Description.Type = 31;
            break;
    }
    Description.UsageIndex = Description.VertexElements[0].UsageIndex;
    Description.NumVertexElements = 0;
}

int XmfDataBuffer::GetVertexDeclarationSize ()
{
    int size = 0;
    for ( int i = 0; i < Description.NumVertexElements; ++i )
    {
        size += DXUtil::GetVertexElementTypeSize ( (D3DDECLTYPE)Description.VertexElements[i].Type );
    }
    return size;
}

D3DFORMAT XmfDataBuffer::GetIndexFormat ()
{
    if ( !IsIndexBuffer () )
        throw "The data buffer is not an index buffer";

    return Description.Format == 30 ? D3DFMT_INDEX16 : D3DFMT_INDEX32;
}
