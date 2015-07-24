//===-- BuildFileCommand.cpp ----------------------------------------------===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2015 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See http://swift.org/LICENSE.txt for license information
// See http://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//

#include "llbuild/Commands/Commands.h"

#include "llbuild/BuildSystem/BuildFile.h"

using namespace llbuild;
using namespace llbuild::buildsystem;

namespace {

class ParseBuildFileDelegate : public BuildFileDelegate {
public:
  ~ParseBuildFileDelegate() {}

  virtual void error(const std::string& filename,
                     const std::string& message) override {
    fprintf(stderr, "%s: error: %s\n", filename.c_str(), message.c_str());
  }

  virtual bool configureClient(const std::string& name,
                               uint32_t version,
                               const property_list_type& properties) override {
    // Dump the client information.
    printf("client ('%s', version: %u)\n", name.c_str(), version);
    for (const auto& property: properties) {
      printf("  -- '%s': '%s'\n", property.first.c_str(),
             property.second.c_str());

    }
    printf("\n");

    return true;
  }
};

static void parseUsage() {
  int optionWidth = 20;
  fprintf(stderr, "Usage: %s buildfile parse [options] <path>\n",
          ::getprogname());
  fprintf(stderr, "\nOptions:\n");
  fprintf(stderr, "  %-*s %s\n", optionWidth, "--help",
          "show this help message and exit");
  ::exit(1);
}

static int executeParseCommand(std::vector<std::string> args) {
  while (!args.empty() && args[0][0] == '-') {
    const std::string option = args[0];
    args.erase(args.begin());

    if (option == "--")
      break;

    if (option == "--help") {
      parseUsage();
    }
  }

  if (args.size() != 1) {
    fprintf(stderr, "error: %s: invalid number of arguments\n", getprogname());
    parseUsage();
  }

  std::string filename = args[0].c_str();

  // Load the BuildFile.
  fprintf(stderr, "note: parsing '%s'\n", filename.c_str());
  ParseBuildFileDelegate delegate;
  BuildFile buildFile(filename, delegate);
  buildFile.load();

  return 0;
}

}

#pragma mark - Build Engine Top-Level Command

static void usage() {
  fprintf(stderr, "Usage: %s buildsystem [--help] <command> [<args>]\n",
          getprogname());
  fprintf(stderr, "\n");
  fprintf(stderr, "Available commands:\n");
  fprintf(stderr, "  parse         -- Parse a build file\n");
  fprintf(stderr, "\n");
  exit(1);
}

int commands::executeBuildSystemCommand(const std::vector<std::string> &args) {
  // Expect the first argument to be the name of another subtool to delegate to.
  if (args.empty() || args[0] == "--help")
    usage();

  if (args[0] == "parse") {
    return executeParseCommand({args.begin()+1, args.end()});
  } else {
    fprintf(stderr, "error: %s: unknown command '%s'\n", getprogname(),
            args[0].c_str());
    return 1;
  }
}
