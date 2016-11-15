
#ifndef _UTBuilder_Writers_h__
#define _UTBuilder_Writers_h__

// #include <clang/Basic/SourceManager.h>
#include "ClangCompiler.h"
#include "UnitTestData.h"
#include <context.hpp>



class BaseWriter {
   
public:
  
   BaseWriter();
   
   virtual ~BaseWriter() {}
   
   virtual void init( const std::string&         fileName,
		      const UnitTestData*           data,
		      const ClangCompiler*   compiler );
      
   void writeTemplate(const std::string& templateFileName, const std::string& outputNameSuffix);
  
   
   virtual const std::string& getTeamplateName() const = 0;
   virtual const std::string& getTeamplateSuffix() const = 0;
   
protected:
   
   virtual const Plustache::Context* createContext() = 0;
   
//    const ClangCompiler*   _compiler;
   std::string            _fileName;
   const UnitTestData*          _data;
   
   const ASTinfo* 		_info;
   const clang::SourceManager*	_sourceMgr;
  
   static const std::string _templateDir;
};



class MockWriter : public BaseWriter 
{
   public:
  
   MockWriter();
   
   virtual const std::string& getTeamplateName() const { return _templateName; }
   virtual const std::string& getTeamplateSuffix() const { return _templateSuffix; }
   
protected:
   
   virtual const Plustache::Context* createContext() override final;
   
    static const std::string _templateName;
    static const std::string _templateSuffix;
    
};



class SerializationWriter : public BaseWriter 
{
   public:
  
   SerializationWriter();
   
    virtual const std::string& getTeamplateName() const { return _templateName; }
   virtual const std::string& getTeamplateSuffix() const { return _templateSuffix; }
   
protected:
   
   virtual const Plustache::Context* createContext() override final;
   
   static const std::string _templateName;
   static const std::string _templateSuffix;
};



class StructuresToSerializeWriter : public BaseWriter 
{
   public:
  
   StructuresToSerializeWriter();
   
   virtual const std::string& getTeamplateName() const { return _templateName; }
   virtual const std::string& getTeamplateSuffix() const { return _templateSuffix; }
   
protected:
   
   virtual const Plustache::Context* createContext() override final;
   
   static const std::string _templateName;
   static const std::string _templateSuffix;
};



class UnitTestFileWriter : public BaseWriter 
{
   public:
  
   UnitTestFileWriter();
   
   
   virtual const std::string& getTeamplateName() const { return _templateName; }
   virtual const std::string& getTeamplateSuffix() const { return _templateSuffix; }
   
protected:
   
   virtual const Plustache::Context* createContext() override final;
   
   static const std::string _templateName;
   static const std::string _templateSuffix;
   
};



#endif // _UTBuilder_Writers_h__

