/*****************************************************************************\
* (c) Copyright 2013 CERN                                                     *
*                                                                             *
* This software is distributed under the terms of the GNU General Public      *
* Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".   *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/

/// @author Marco Clemencic <marco.clemencic@cern.ch>
/// @author Benedikt Hegner <benedikt.hegner@cern.ch>

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <list>
#include <memory>

#include <getopt.h>
#include <dlfcn.h>

#include <PM4hep/PluginManager.h>

void help(std::string argv0) {
  std::cout << "Usage: " << argv0 << " [option] library1 [library2 ...]\n"
      "\n list the component factories present in the given libraries\n\n"
      "Options:\n\n"
      "  -h, --help       show this help message and exit\n"
      "  -o OUTPUT, --output OUTPUT\n"
      "                   write the list of factories on the file OUTPUT, use - for\n"
      "                   standard output (default)\n"
      "  -a, --all        write a list of all plugins found in\n"
      "                   LD_LIBRARY_PATH to OUTPUT\n"
	    << std::endl;
}

void usage(std::string argv0) {
  std::cout << "Usage: " << argv0 << " [option] library1 [library2 ...]\n"
    "Try `" << argv0 << " -h' for more information.\n"
	    << std::endl;
}

int main(int argc, char* argv[]) {
  const auto& reg = PM4hep::PluginManager::Details::Registry::instance();

  // cache to keep track of the loaded factories
  auto loaded = reg.loadedFactories();

  // whether to dump all plugins found in LD_LIBRARY_PATH
  bool dumpAll = false;

  // Parse command line
  std::list<char*> libs;
  std::string output_opt("-");
  {
    std::string argv0(argv[0]);
    {
      auto i = argv0.rfind('/');
      if (i != std::string::npos)
        argv0 = argv0.substr(i+1);
    }

    int i = 1;
    while (i < argc) {
      const std::string arg(argv[i]);
      if (arg == "-o" || arg == "--output") {
        if (++i < argc) {
          output_opt = argv[i];
        } else {
	  std::cerr << "ERROR: missing argument for option " << arg << std::endl;
	  std::cerr << "See `" << argv0 << " -h' for more details." << std::endl;
          return EXIT_FAILURE;
        }
      } else if (arg == "-h" || arg == "--help") {
        help(argv0);
        return EXIT_SUCCESS;
      } else if (arg == "-a" || arg == "--all") {
        dumpAll = true;
      } else {
        libs.push_back(argv[i]);
      }
      ++i;
    }
    if (libs.empty() && dumpAll == false) {
      usage(argv0);
      return EXIT_FAILURE;
    }
  }

  // handle output option
  std::unique_ptr<std::ostream> output_file;
  if (output_opt != "-") {
    output_file = std::unique_ptr<std::ostream>(new std::ofstream(output_opt.c_str()));
  }
  std::ostream &output = (output_file ? *output_file : std::cout);

  // if asked to dump all go through everything loaded already and dump it
    if (dumpAll){
      for(auto f : reg.factories()){
        output << f.second.library << ":"<<f.second.rtype << ":" << f.second.className << std::endl;
    }
   return 0;
  }

  // loop over the list of libraries passed on the command line
  // and search for the not initially loaded ones
  for (char* lib: libs) {
    if (dlopen(lib, RTLD_LAZY | RTLD_LOCAL)) {
      for(auto f : reg.loadedFactories() ){
	auto& info = reg.getInfoWithInterface(f);
        if (loaded.find(f) == loaded.end()) {
	  output << lib << ":" << info.rtype << ":" << info.id << std::endl;
        } else {
          std::cerr << "WARNING: factory for '" << f << "already found in " << info.library << std::endl; 
        }
      }     
    } else {
      std::cerr << "ERROR: failed to load " << lib << std::endl;
    }
  }
  return 0;
}
