#pragma once
bool ConvertXmlToDae ( const char* pszGameBaseFolderPath, const char* pszXmlFilePath, const char* pszDaeFilePath, char* pszError, int iMaxErrorSize );
bool ConvertDaeToXml ( const char* pszGameBaseFolderPath, const char* pszDaeFilePath, const char* pszXmlFilePath, char* pszError, int iMaxErrorSize );

extern "C" bool C_API_ConvertXmlToDae ( const char* pszGameBaseFolderPath, const char* pszXmlFilePath, const char* pszDaeFilePath, char* pszError, int iMaxErrorSize );
extern "C" bool C_API_ConvertDaeToXml ( const char* pszGameBaseFolderPath, const char* pszDaeFilePath, const char* pszXmlFilePath, char* pszError, int iMaxErrorSize );

//extern "C" bool ConvertXacToDae ( const char* pszGameBaseFolderPath, const char* pszXacFilePath, const char* pszDaeFilePath, char* pszError, int iMaxErrorSize );
//extern "C" bool ConvertDaeToXac ( const char* pszGameBaseFolderPath, const char* pszDaeFilePath, const char* pszXacFilePath, char* pszError, int iMaxErrorSize );
