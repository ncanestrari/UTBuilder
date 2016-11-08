
#ifndef _UTBuilder_Writers_h__
#define _UTBuilder_Writers_h__

#include <clang/Basic/SourceManager.h>
#include "UnitTestData.h"
#include <context.hpp>



class BaseWriter {
   
public:
  
   BaseWriter( const std::string&         fileName,
	       const UnitTestData&           data,
	       const clang::SourceManager&   sourceMgr );
   
   virtual ~BaseWriter() {}
   
   void WriteTemplate(const std::string& templateFileName, const char* outputNameSuffix);
  
   
protected:
   
   virtual const Plustache::Context* createContext() = 0;
   
   const std::string            _fileName;
   const UnitTestData&          _data;
   const clang::SourceManager&	_sourceMgr;
  
};



class MockWriter : public BaseWriter 
{
   public:
  
   MockWriter( const std::string&         fileName,
	       const UnitTestData&           data,
	       const clang::SourceManager&   sourceMgr );
   
protected:
   
   virtual const Plustache::Context* createContext() override final;
   
};


class SerializationWriter : public BaseWriter 
{
   public:
  
   SerializationWriter( const std::string&         fileName,
	       const UnitTestData&           data,
	       const clang::SourceManager&   sourceMgr );
   
protected:
   
   virtual const Plustache::Context* createContext() override final;
   
};

class StructuresToSerializeWriter : public BaseWriter 
{
   public:
  
   StructuresToSerializeWriter( const std::string&         fileName,
	       const UnitTestData&           data,
	       const clang::SourceManager&   sourceMgr );
   
protected:
   
   virtual const Plustache::Context* createContext() override final;
   
};


class UnitTestFile : public BaseWriter 
{
   public:
  
   UnitTestFile( const std::string&         fileName,
	       const UnitTestData&           data,
	       const clang::SourceManager&   sourceMgr );
   
protected:
   
   virtual const Plustache::Context* createContext() override final;
   
};



#endif // _UTBuilder_Writers_h__

