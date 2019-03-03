#pragma once

class XuMeshFile
{
public:
    std::vector < XmfDataBuffer >&      GetBuffers              ()  { return _buffers; };
    XmfDataBuffer*                      GetIndexBuffer          ();

    std::vector < XmfVertexElement >    GetVertexDeclaration    ();

    int                                 NumVertices             ();
    int                                 NumIndices              ();

    std::vector < XmfMaterial >&        GetMaterials            ()  { return _materials; };
    int                                 NumMaterials            ()  { return _materials.size(); }
    void                                AddMaterial             ( int firstIndex, int numIndices, const std::string& name );

    static std::shared_ptr<XuMeshFile>  ReadFromFile            ( const std::string& filePath, Assimp::IOSystem* pIOHandler );
    void                                WriteToFile             ( const std::string& filePath, Assimp::IOSystem* pIOHandler );

private:
    static XmfHeader                    ReadHeader              ( Assimp::IOStream* pStream );
    void                                ReadBufferDescs         ( Assimp::IOStream* pStream, XmfHeader& header );
    void                                ReadMaterials           ( Assimp::IOStream* pStream, XmfHeader& header );
    void                                ReadBuffers             ( Assimp::IOStream* pStream, XmfHeader& header );
    void                                Validate                ();

    std::map < XmfDataBuffer*, std::vector<byte> >  CompressBuffers ();
    void                                WriteHeader             ( Assimp::IOStream* pStream );
    void                                WriteBufferDescs        ( Assimp::IOStream* pStream, std::map < XmfDataBuffer*, std::vector<byte> >& compressedBuffers );
    void                                WriteMaterials          ( Assimp::IOStream* pStream );
    void                                WriteBuffers            ( Assimp::IOStream* pStream, std::map < XmfDataBuffer*, std::vector<byte> >& compressedBuffers );

    std::vector < XmfDataBuffer >       _buffers;
    std::vector < XmfMaterial >         _materials;
};
