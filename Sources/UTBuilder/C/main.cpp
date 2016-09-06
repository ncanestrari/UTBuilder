
#include "Action.h"
#include "utils.h"

#include <clang/AST/Expr.h>
#include <clang/Frontend/ASTUnit.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/CommandLine.h>

#include <iostream>
using std::cout;

using std::string;
using std::vector;



static llvm::cl::OptionCategory testToolCategory("test tool options");
static llvm::cl::extrahelp commonHelp( clang::tooling::CommonOptionsParser::HelpMessage);
static llvm::cl::extrahelp additionalHelp("\n .... some more help");

int main(int argc, const char *argv[])
{
   string fileNamePath(argv[1]);
   string comcode = utils::extractCommercialCodePath(fileNamePath);
   string package = utils::extractPackagePath(fileNamePath);

   vector<string>* include_folders = utils::getIncludePaths(comcode+package);
   for(const auto inc : *include_folders)
      cout << inc << "\n";

   clang::tooling::CommonOptionsParser _optionParser(argc, argv, testToolCategory);
   clang::tooling::ClangTool _tool(_optionParser.getCompilations(), _optionParser.getSourcePathList());

   int result = _tool.run(clang::tooling::newFrontendActionFactory<Action>().get());

   return 0;
}
