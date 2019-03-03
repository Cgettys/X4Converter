#pragma once

class XmfDataBuffer
{
public:
                        XmfDataBuffer                   ();

    XmfDataBufferDesc   Description;

    void                AllocData                       ();
    bool                IsCompressed                    () const;
    bool                IsIndexBuffer                   () const;
    bool                IsVertexBuffer                  () const;
    int                 GetCompressedDataSize           () const;
    int                 GetUncompressedDataSize         () const;
    byte*               GetData                         ();

    void                NormalizeVertexDeclaration      ();
    void                DenormalizeVertexDeclaration    ();
    int                 GetVertexDeclarationSize        ();

    D3DFORMAT           GetIndexFormat                  ();

private:
    std::vector<byte>   _data;
};
