// Copyright Vladimir Prus 2002-2004.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)
// Modified to suit this project by Charlie Gettys in 2019
/* Shows how to use both command line and config file. */

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iostream>
#include <fstream>
#include <iterator>
#include <stdio.h>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <X4ConverterTools/API.h>

using namespace boost::algorithm;
using namespace boost::filesystem;

// A helper function to simplify the main part.
template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
	std::copy(v.begin(), v.end(), std::ostream_iterator<T>(os, " "));
	return os;
}

int main(int ac, char* av[]) {
	try {
		std::string action;
		std::string dat_dir;
		std::string target;
		std::string ext_dir;
		std::string config_file;
		// allowed only on command line
		po::options_description generic("Generic options");
		generic.add_options()
				("version,v", "print version string")
				("help","produce help message")
				;

		const char* action_string = "action to perform:\n"
				 "\timportxac: convert .xac to .dae\n"
				 "\timportxmf: convert .xml/.xmf to .dae\n"
				 "\texportxac: convert .dae to .xac\n"
				 "\texportxmf: convert .dae to .xml/.xmf\n";
		// Declare a group of options that will be
		// allowed both on command line and in
		// config file
		po::options_description config("Configuration");
		config.add_options()
				("action", po::value<std::string>(&action), action_string)
				("datdir", po::value<std::string>(&dat_dir),"path where unpacked catalog files can be found")
				("target", po::value<std::string>(&target), "target .xml/.xac/.dae file")
				("extdir", po::value<std::string>(&ext_dir), "path to folder where extensions are stored")
				("config", po::value<std::string>(&config_file), "path to config file")
				;

		po::options_description cmdline_options;
		cmdline_options.add(generic).add(config);

		po::options_description config_file_options;
		config_file_options.add(config);

		po::options_description visible("Allowed options");
		visible.add(generic).add(config);

		po::positional_options_description p;
		p.add("action", 1);
		p.add("datdir", 1);
		p.add("target", 1);
		p.add("extdir", 1);
		po::variables_map vm;
		// Store command line options first. Boost prefers options that were stored first, so commandline wins over config file
		store(
				po::command_line_parser(ac, av).options(cmdline_options).positional(
						p).run(), vm);
		notify(vm);

		if (vm.count("config")) {
			ifstream ifs(config_file.c_str());
			if (!ifs) {
				std::cout << "can not open config file: " << config_file << "\n";
				return 0;
			} else {
				store(parse_config_file(ifs, config_file_options), vm);
				notify(vm);
			}
		}

		if (vm.count("help")) {
			printf(
					"Usage: X4ConvertersMain <action> <folder with .dat file contents> <.xml/.xac/.dae file> <extensions folder (optional)>\n");
			std::cout << visible << "\n";
			return 0;
		}

		if (vm.count("version")) {
			std::cout << "Multiple sources example, version 1.0\n";
			return 0;
		}


	path gameBaseFolderPath = canonical(dat_dir).make_preferred();
	if (!is_directory(gameBaseFolderPath)) {
		printf("Specified .dat file content folder does not exist.\n");
		return 1;
	}

	path inputFilePath = canonical(target).make_preferred();
	if (!is_regular_file(inputFilePath)) {
		printf("Input file does not exist.\n");
		return 1;
	}

	path outputFilePath(inputFilePath);
	char szError[256];
	bool success;
	if (action == "importxmf") {
		// .xml/.xmf -> .dae
		outputFilePath.replace_extension(".dae");
		success = ConvertXmlToDae(gameBaseFolderPath.string().c_str(),
				inputFilePath.string().c_str(), outputFilePath.string().c_str(),
				szError, sizeof(szError));
	} else if (action == "exportxmf") {
		// .dae -> .xml/.xmf
		outputFilePath.replace_extension(".xml");
		success = ConvertDaeToXml(gameBaseFolderPath.string().c_str(),
				inputFilePath.string().c_str(), outputFilePath.string().c_str(),
				szError, sizeof(szError));
	} else if (action == "importxac") {
		printf("Currently not supported, sorry\n");
		// .xac -> .dae
//		outputFilePath.replace_extension(".dae");
//        success = ConvertXacToDae (
//            gameBaseFolderPath.string ().c_str (),
//            inputFilePath.string ().c_str (),
//            outputFilePath.string ().c_str (),
//            szError,
//            sizeof(szError)
//        );
	}else if (action == "exportxac") {
		printf("Currently not supported, sorry\n");
		// .dae -> .xac
//        outputFilePath.replace_extension ( ".xac" );
//        success = ConvertDaeToXac (
//            gameBaseFolderPath.string ().c_str (),
//            inputFilePath.string ().c_str (),
//            outputFilePath.string ().c_str (),
//            szError,
//            sizeof(szError)
//        );
	} else {
		printf("Unknown action.\n\n");
		printf(
				"Usage: X4ConvertersMain <action> <folder with .dat file contents> <.xml/.xac/.dae file> <extensions folder (optional)>\n");
		std::cout << visible << "\n";
		return 1;
	}

	if (!success){
		printf("%s\n", szError);
	}
	} catch (std::exception& e) {
		std::cout << e.what() << "\n";
		return 1;
	}
	return 0;
}
