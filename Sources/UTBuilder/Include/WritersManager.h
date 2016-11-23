#ifndef _UTBuilder_WritersManager_h__
#define _UTBuilder_WritersManager_h__


#include <string>
#include <set>


#include "ClangCompiler.h"
#include "UnitTestData.h"
#include <context.hpp>

class BaseWriter {
   
public:
  
   BaseWriter() = default;
   virtual ~BaseWriter() = default;
   
   virtual void init( const std::string&         fileName,
                      const UnitTestData*           data,
                      const ClangCompiler*   compiler );
      
   void writeTemplate(const std::string& templateFileName, const std::string& outputNameSuffix);
  
   
   virtual const std::string& getTemplateName() const = 0;
   virtual const std::string& getTemplateSuffix() const = 0;
   
protected:
   
   virtual const Plustache::Context* createContext() = 0;
   
//    const ClangCompiler*   _compiler;
   std::string            _fileName;
   const UnitTestData*          _data;
   
   const ASTinfo*               _info;
   const clang::SourceManager*  _sourceMgr;
  
   static const std::string _templateDir;
};




class WritersManager {
   
public:
   
   WritersManager() = delete;
   
   WritersManager(const std::string&         fileName,
	       const UnitTestData&           data,
	       const ClangCompiler& compiler);
   
   ~WritersManager();
   
   void add(BaseWriter* writer);
   
   void write();
   
private:
   
//    keep a unique_ptr ref of the Writers   
   std::set< std::unique_ptr<BaseWriter> > _writers;
      
      
   // store all these vars and pass them to each Writers as argument when the Writers are added to the WritersManager
   const std::string            _fileName;
   const UnitTestData*          _data;
   const ClangCompiler*		_compiler;
//    const clang::SourceManager&	_sourceMgr;
   
};



#endif // _UTBuilder_WritersManager_h__

