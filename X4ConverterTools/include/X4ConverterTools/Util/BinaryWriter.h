#pragma once

class BinaryWriter
{
public:
                                BinaryWriter            ( Assimp::IOStream* pStream );

    Assimp::IOStream*           GetStream               () const;

    template < typename T >
    void                        Write                   ( const T& value )
    {
        Write ( &value, 1 );
    }

    template < typename T >
    void                        Write                   ( const T* pValue, int count )
    {
        _pStream->Write ( pValue, sizeof(T), count );
    }

    template < typename T >
    void                        Write                   ( const std::vector<T>& values )
    {
        if ( !values.empty () )
            Write ( values.data (), values.size () );
    }

    template < typename T>
    void                        Write                   ( const std::string& str )
    {
        Write<int> ( str.size () );
        if ( !str.empty () )
            Write ( str.data (), str.size () );
    }

private:
    Assimp::IOStream*           _pStream;
};
