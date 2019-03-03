#include "StdInc.h"

#include <cstring>
#include <stdio.h>
#include <stdlib.h>
extern "C" bool ConvertXmlToDae ( const char* pszGameBaseFolderPath, const char* pszXmlFilePath, const char* pszDaeFilePath, char* pszError, int iMaxErrorSize )
{
    Assimp::Importer importer;
    importer.RegisterLoader ( new XmfImporter ( pszGameBaseFolderPath ) );
    const aiScene* pScene = importer.ReadFile ( pszXmlFilePath, 0 );
    if ( !pScene )
    {
        strncpy( pszError, importer.GetErrorString (), iMaxErrorSize );
        return false;
    }

    Assimp::Exporter exporter;
    aiReturn result = exporter.Export ( pScene, "collada", pszDaeFilePath );
    if ( result != aiReturn_SUCCESS )
    {
       strncpy( pszError, exporter.GetErrorString (), iMaxErrorSize  );
        return false;
    }

    return true;
}

extern "C" bool ConvertDaeToXml ( const char* pszGameBaseFolderPath, const char* pszDaeFilePath, const char* pszXmfFilePath, char* pszError, int iMaxErrorSize )
{
    Assimp::Importer importer;
    const aiScene* pScene = importer.ReadFile ( pszDaeFilePath, 0 );
    if ( !pScene )
    {
       strncpy( pszError, importer.GetErrorString (), iMaxErrorSize );
        return false;
    }

    Assimp::Exporter exporter;
    exporter.RegisterExporter ( XmfExporter::Format );
    XmfExporter::GameBaseFolderPath = pszGameBaseFolderPath;
    aiReturn result = exporter.Export ( pScene, "xmf", pszXmfFilePath );
    if ( result != aiReturn_SUCCESS )
    {
       strncpy( pszError, exporter.GetErrorString (), iMaxErrorSize );
        return false;
    }

    return true;
}

extern "C" bool ConvertXacToDae ( const char* pszGameBaseFolderPath, const char* pszXacFilePath, const char* pszDaeFilePath, char* pszError, int iMaxErrorSize )
{
    Assimp::Importer importer;
    importer.RegisterLoader ( new XacImporter ( pszGameBaseFolderPath ) );
    const aiScene* pScene = importer.ReadFile ( pszXacFilePath, 0 );
    if ( !pScene )
    {
       strncpy( pszError, importer.GetErrorString (), iMaxErrorSize );
        return false;
    }

    Assimp::Exporter exporter;
    aiReturn result = exporter.Export ( pScene, "collada", pszDaeFilePath );
    if ( result != aiReturn_SUCCESS )
    {
       strncpy( pszError, exporter.GetErrorString (), iMaxErrorSize );
        return false;
    }

    return true;
}

extern "C" bool ConvertDaeToXac ( const char* pszGameBaseFolderPath, const char* pszDaeFilePath, const char* pszXacFilePath, char* pszError, int iMaxErrorSize )
{
    Assimp::Importer importer;
    const aiScene* pScene = importer.ReadFile ( pszDaeFilePath, 0 );
    if ( !pScene )
    {
       strncpy( pszError, importer.GetErrorString (), iMaxErrorSize );
        return false;
    }

    Assimp::Exporter exporter;
    exporter.RegisterExporter ( XacExporter::Format );
    XacExporter::GameBaseFolderPath = pszGameBaseFolderPath;
    aiReturn result = exporter.Export ( pScene, "xac", pszXacFilePath );
    if ( result != aiReturn_SUCCESS )
    {
       strncpy( pszError, exporter.GetErrorString (), iMaxErrorSize );
        return false;
    }

    return true;
}
