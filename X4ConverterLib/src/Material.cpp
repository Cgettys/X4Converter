#include "StdInc.h"

using namespace boost;
using namespace boost::algorithm;
using namespace boost::filesystem;

Material::Material ()
{
    _pCollection = nullptr;
}

Material::Material ( MaterialCollection* pCollection, pugi::xml_node node )
{
    _pCollection = pCollection;
    _name = node.attribute ( "name" ).value ();
    
    pugi::xpath_node_set properties = node.select_nodes ( "properties/property" );
    for ( auto it = properties.begin (); it != properties.end (); ++it )
    {
        std::string type = it->node ().attribute ( "type" ).value ();
        std::string name = it->node ().attribute ( "name" ).value ();
        std::string value = it->node ().attribute ( "value" ).value ();

        if ( type == "BitMap" )
        {
            if ( name == "diffuse_map" )
                _diffuseMapFilePath = value;
            else if ( name == "specular_map" )
                _specularMapFilePath = value;
            else if ( name == "normal_map" )
                _normalMapFilePath = value;
            else if ( name == "environment_map" )
                _environmentMapFilePath = value;
        }
        else if ( type == "Float" )
        {
            float fValue = atof ( value.c_str () );
            if ( name == "diffuseStr" )
                _diffuseStrength = fValue;
            else if ( name == "normalStr" )
                _normalStrength = fValue;
            else if ( name == "specularStr" )
                _specularStrength = fValue;
            else if ( name == "environmentStr" )
                _environmentStrength = fValue;
        }
    }
}

std::string Material::GetDiffuseMapFilePath () const
{
    return GetTextureFilePath ( _diffuseMapFilePath );
}

std::string Material::GetSpecularMapFilePath () const
{
    return GetTextureFilePath ( _specularMapFilePath );
}

std::string Material::GetNormalMapFilePath () const
{
    return GetTextureFilePath ( _normalMapFilePath );
}

std::string Material::GetEnvironmentMapFilePath () const
{
    return GetTextureFilePath ( _environmentMapFilePath );
}

aiMaterial* Material::ConvertToAiMaterial ( const boost::filesystem::path& modelFolderPath ) const
{
    aiMaterial* pAiMaterial = new aiMaterial ();
    pAiMaterial->AddProperty ( new aiString ( _pCollection->GetName () + "X" + GetName () ), AI_MATKEY_NAME );

    std::string textureFilePath = GetDecompressedTextureFilePath ( GetDiffuseMapFilePath () );
    if ( !textureFilePath.empty () )
    {
        textureFilePath = PathUtil::GetRelativePath ( textureFilePath, modelFolderPath ).string ();
        pAiMaterial->AddProperty ( new aiString ( textureFilePath ), AI_MATKEY_TEXTURE_DIFFUSE(0) );
    }

    textureFilePath = GetDecompressedTextureFilePath ( GetSpecularMapFilePath () );
    if ( !textureFilePath.empty () )
    {
        textureFilePath = PathUtil::GetRelativePath ( textureFilePath, modelFolderPath ).string ();
        pAiMaterial->AddProperty ( new aiString ( textureFilePath ), AI_MATKEY_TEXTURE_SPECULAR(0) );
    }

    textureFilePath = GetDecompressedTextureFilePath ( GetNormalMapFilePath () );
    if ( !textureFilePath.empty () )
    {
        textureFilePath = PathUtil::GetRelativePath ( textureFilePath, modelFolderPath ).string ();
        pAiMaterial->AddProperty ( new aiString ( textureFilePath ), AI_MATKEY_TEXTURE_NORMALS(0) );
    }

    textureFilePath = GetDecompressedTextureFilePath ( GetEnvironmentMapFilePath () );
    if ( !textureFilePath.empty () )
    {
        textureFilePath = PathUtil::GetRelativePath ( textureFilePath, modelFolderPath ).string ();
        pAiMaterial->AddProperty ( new aiString ( textureFilePath ), AI_MATKEY_TEXTURE_REFLECTION(0) );
    }

    return pAiMaterial;
}

std::string Material::GetTextureFilePath ( const std::string& filePath ) const
{
    static const char* ppszAllowedExtensions[] = { "gz", "dds", "tga", "jpg" };

    if ( filePath.empty () )
        return std::string ();

    path textureFilePath ( _pCollection->GetLibrary ()->_gameBaseFolderPath );
    textureFilePath /= filePath;
    if ( is_regular_file ( textureFilePath ) )
        return textureFilePath.string ();

    if ( textureFilePath.has_extension () )
        return std::string ();

    for ( int i = 0; i < sizeof(ppszAllowedExtensions)/sizeof(ppszAllowedExtensions[0]); ++i )
    {
        textureFilePath.replace_extension ( ppszAllowedExtensions[i] );
        if ( is_regular_file ( textureFilePath ) )
            return textureFilePath.string ();
    }
    return std::string ();
}

std::string Material::GetDecompressedTextureFilePath ( const std::string& filePath )
{
    if ( filePath.empty () )
        return std::string ();

    path textureFilePath ( filePath );
    if ( !iequals ( textureFilePath.extension ().string (), ".gz" ) )
        return textureFilePath.string ();

    path uncompressedPath ( textureFilePath );
    uncompressedPath.replace_extension ( ".dds" );
    if ( is_regular_file ( uncompressedPath ) )
        return uncompressedPath.string ();

    gzFile pGzFile = gzopen ( textureFilePath.string ().c_str (), "rb" );
    if ( !pGzFile )
        return textureFilePath.string ();

    FILE* pDdsFile = fopen ( uncompressedPath.string ().c_str (), "wb+" );
    if ( !pDdsFile )
    {
        gzclose ( pGzFile );
        return textureFilePath.string ();
    }

    byte buffer[0x400];
    int bytesRead;
    do
    {
        bytesRead = gzread ( pGzFile, buffer, sizeof(buffer) );
        fwrite ( buffer, 1, bytesRead, pDdsFile );
    } while ( bytesRead == sizeof(buffer) );

    fclose ( pDdsFile );
    gzclose ( pGzFile );
    return uncompressedPath.string ();
}
