
#include <stdio.h>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>


#include <X4ConverterTools/API.h>

using namespace boost::algorithm;
using namespace boost::filesystem;

void PrintUsage ()
{
    printf ( "Usage: XRConvertersMain <action> <folder with .dat file contents> <.xml/.xac/.dae file>\n" );
    printf ( "<action>:\n" );
    printf ( "  importxac: convert .xac to .dae\n" );
    printf ( "  importxmf: convert .xml/.xmf to .dae\n" );
    printf ( "  exportxac: convert .dae to .xac\n" );
    printf ( "  exportxmf: convert .dae to .xml/.xmf\n" );
}

int main ( int argCount, const char** ppszArgs )
{
    if ( argCount < 4 )
    {
        PrintUsage ();
        return 1;
    }

    std::string action = ppszArgs[1];

    path gameBaseFolderPath = canonical ( ppszArgs[2] ).make_preferred ();
    if ( !is_directory ( gameBaseFolderPath ) )
    {
        printf ( "Specified .dat file content folder does not exist.\n" );
        return 1;
    }

    path inputFilePath = canonical ( ppszArgs[3] ).make_preferred ();
    if ( !is_regular_file ( inputFilePath ) )
    {
        printf ( "Input file does not exist.\n" );
        return 1;
    }

    path outputFilePath ( inputFilePath );
    char szError[256];
    bool success;
    if ( action == "importxmf" )
    {
        // .xml/.xmf -> .dae
        outputFilePath.replace_extension ( ".dae" );
        success = ConvertXmlToDae (
            gameBaseFolderPath.string ().c_str (),
            inputFilePath.string ().c_str (),
            outputFilePath.string ().c_str (),
            szError,
            sizeof(szError)
        );
    }
    else if ( action == "importxac" )
    {
        // .xac -> .dae
        outputFilePath.replace_extension ( ".dae" );
//        success = ConvertXacToDae (
//            gameBaseFolderPath.string ().c_str (),
//            inputFilePath.string ().c_str (),
//            outputFilePath.string ().c_str (),
//            szError,
//            sizeof(szError)
//        );
    }
    else if ( action == "exportxmf" )
    {
        // .dae -> .xml/.xmf
        outputFilePath.replace_extension ( ".xml" );
        success = ConvertDaeToXml (
            gameBaseFolderPath.string ().c_str (),
            inputFilePath.string ().c_str (),
            outputFilePath.string ().c_str (),
            szError,
            sizeof(szError)
        );
    }
    else if ( action == "exportxac" )
    {
        // .dae -> .xac
//        outputFilePath.replace_extension ( ".xac" );
//        success = ConvertDaeToXac (
//            gameBaseFolderPath.string ().c_str (),
//            inputFilePath.string ().c_str (),
//            outputFilePath.string ().c_str (),
//            szError,
//            sizeof(szError)
//        );
    }
    else
    {
        printf ( "Unknown action.\n\n" );
        PrintUsage ();
        return 1;
    }
    
    if ( !success )
        printf ( "%s\n", szError );
}
