#ifndef _CLangASTBuilder_writer_h__
#define _CLangASTBuilder_writer_h__

#include <clang/Basic/SourceManager.h>

#include <string>
#include <sstream>
#include <set>

#include "Results.h"


namespace clang {
   class FunctionDecl;
   class TypedefNameDecl;
}

namespace Plustache {
   class Context;
}

class Writer 
{  
  
public:
  
   Writer(const std::string&            fileName,
	  const clang::SourceManager&   sourceMgr);
  
   ~Writer() {}
   
   
   void createFiles();
   
   
private:  
   
   void CreateMockFile(void);
   
   void CreateUnitTestFile(void);
   
   void CreateSerializationFile(void);
   
   
   void CreateSerializationJsonfile(const FunctionDeclKeySetMap& funcDeclMap
                                    , const std::string & objectName
                                    , const std::string  &outFileName
                                    , const bool addMocks = false );
   
   
   std::shared_ptr<const Plustache::Context> CreateMockContext(  const std::set<std::string>            &includePaths,
                                                                        const FunctionDeclKeySetMap   &funcDeclsMap );
   
   std::shared_ptr<const Plustache::Context> CreateUnitTestContext( const std::set<std::string>            &includePaths,
                                                                           const FunctionDeclKeySetMap   &funcDeclsMap );
   
   
   std::shared_ptr<const Plustache::Context> CreateSerializationContext( const std::set<std::string>            &includePaths,
                                                                         const std::set<const clang::TypedefNameDecl*>&   typedefNameDecls );
   
   
   std::shared_ptr<const Plustache::Context> CreateSerializationStructuresContext( const std::set<std::string>            &includePaths,
                                                                          const FunctionDeclKeySetMap   &funcDeclsMap );
   
   std::shared_ptr<const Plustache::Context> CreateStructuresToSerializeContext( const std::set<std::string>            &includePaths,
                                                                          const FunctionDeclKeySetMap   &funcDeclsMap );
   
   
   void WriteTemplate(   std::shared_ptr<const Plustache::Context>      context,
                                const std::string& templateFileName,
                                const std::string  &outFileName);
   
   
   void MockFunctionFFF(const clang::FunctionDecl* const    funcDecl,
                               std::ostringstream&           out );
   
   
   
   const std::string  _fileName;
   const clang::SourceManager&   _sourceMgr;
   
};


#endif // _CLangASTBuilder_writer_h__

