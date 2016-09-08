#ifndef _CLangASTBuilder_writer_h__
#define _CLangASTBuilder_writer_h__

#include <clang/Basic/SourceManager.h>

#include <string>
#include <sstream>
#include <set>


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
   
   
   
   std::shared_ptr<const Plustache::Context> CreateMockContext(  const std::set<std::string>            &includePaths,
                                                                        const std::set<const clang::FunctionDecl*>   &funcDecls );
   
   std::shared_ptr<const Plustache::Context> CreateUnitTestContext( const std::set<std::string>            &includePaths,
                                                                           const std::set<const clang::FunctionDecl*>   &funcDecls );
   
   
   std::shared_ptr<const Plustache::Context> CreateSerializationContext( const std::set<std::string>            &includePaths,
                                                                         const std::set<const clang::TypedefNameDecl*>&   typedefNameDecls );
   
   
   void WriteTemplate(   std::shared_ptr<const Plustache::Context>      context,
                                const std::string& templateFileName,
                                const std::string  &outFileName);
   
   
   void MockFunctionFFF(const clang::FunctionDecl* const    funcDecl,
                               std::ostringstream&           out );
   
   
   
   const std::string  _fileName;
   const clang::SourceManager&   _sourceMgr;
   
};


#endif // _CLangASTBuilder_writer_h__

