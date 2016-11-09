
#ifndef _CLangASTBuilder_writer_h__
#define _CLangASTBuilder_writer_h__

#include <clang/Basic/SourceManager.h>

#include <string>
#include <sstream>
#include <set>

#include "Results.h"
#include "FunctionTestDataFile.h"
#include "UnitTestData.h"

#include "Writers.h"

namespace clang {
class FunctionDecl;
class TypedefNameDecl;
}

namespace Plustache {
class Context;
}

class BaseWriter;


class TemplateInfo {
public:
   std::string _name;
   std::string _outputSuffix;   
};


class FilesWriter {
   
public:
   
   FilesWriter() = delete;
   
   FilesWriter(const std::string&         fileName,
	       const UnitTestData&           data,
	       const clang::SourceManager&   sourceMgr);
   
   ~FilesWriter();
   
   void add(BaseWriter* writer);
   
   void write();
   
private:
   
   const std::string            _fileName;
   const UnitTestData&          _data;
   const clang::SourceManager&	_sourceMgr;
   
   std::set< std::unique_ptr<BaseWriter> > _writers;
   
};


class Writer {

public:
   Writer(const UnitTestData& data,
          const std::string            &fileName,
          const clang::SourceManager   &sourceMgr);
   ~Writer() {}

   void createFiles(void);
   void createUnitTestFile(void);

private:

   void CreateMockFile(void);
   void CreateSerializationFile(void);

   const Plustache::Context* CreateMockContext(const std::set<std::string>   &includePaths,
                                                               const FunctionDeclKeySetMap   &funcDeclsMap);
   const Plustache::Context* CreateUnitTestContext(const std::set<std::string>   &includePaths,
                                                                   const FunctionTestCollection      *funcData);
   const Plustache::Context* CreateSerializationContext(const std::set<std::string>                      &includePaths,
                                                                        const std::set<const clang::TypedefNameDecl *>   &typedefNameDecls);
   const Plustache::Context* CreateSerializationStructuresContext(const std::set<std::string>   &includePaths,
                                                                                  const FunctionDeclKeySetMap   &funcDeclsMap);
   const Plustache::Context* CreateStructuresToSerializeContext(const std::set<std::string>   &includePaths,
                                                                                const FunctionDeclKeySetMap   &funcDeclsMap);

   void WriteTemplate(const Plustache::Context*  context,
                      const std::string                         &templateFileName,
                      const std::string                         &outFileName);

   void MockFunctionFFF(const clang::FunctionDecl *const    funcDecl,
                        std::ostringstream                 &out);

   
   const UnitTestData&          _data;
   const std::string            _fileName;
   const clang::SourceManager&	_sourceMgr;
};


#endif // _CLangASTBuilder_writer_h__

