
#ifndef _UTBuilder_Writers_h__
#define _UTBuilder_Writers_h__

#include "WritersManager.h"

// #include <clang/Basic/SourceManager.h>






class MockWriter : public BaseWriter 
{
   public:
  
   MockWriter();
   virtual ~MockWriter() = default;
   
   virtual const std::string& getTemplateName() const  override final { return _templateName; }
   virtual const std::string& getTemplateSuffix() const  override final { return _templateSuffix; }
   
   static const std::string _templateSuffix;
   
protected:
   
   virtual const Plustache::Context* createContext() override final;
   
    static const std::string _templateName;
    
    
};



class SerializationWriter : public BaseWriter 
{
   public:
  
   SerializationWriter();
   virtual ~SerializationWriter() = default;
   
   virtual const std::string& getTemplateName() const  override final { return _templateName; }
   virtual const std::string& getTemplateSuffix() const  override final { return _templateSuffix; }
   
   static const std::string _templateSuffix;
   
protected:
   
   virtual const Plustache::Context* createContext() override final;
   
   static const std::string _templateName;
   
};



class StructuresToSerializeWriter : public BaseWriter 
{
   public:
  
   StructuresToSerializeWriter();
   virtual ~StructuresToSerializeWriter() = default;
   
   virtual const std::string& getTemplateName() const  override final { return _templateName; }
   virtual const std::string& getTemplateSuffix() const  override final { return _templateSuffix; }
   
   static const std::string _templateSuffix;
   
protected:
   
   virtual const Plustache::Context* createContext() override final;
   
   static const std::string _templateName;
   
};



class UnitTestFileWriter : public BaseWriter 
{
   public:
  
   UnitTestFileWriter();
   virtual ~UnitTestFileWriter() = default;
   
   virtual const std::string& getTemplateName() const  override final { return _templateName; }
   virtual const std::string& getTemplateSuffix() const  override final { return _templateSuffix; }
   
   static const std::string _templateSuffix;
   
protected:
   
   virtual const Plustache::Context* createContext() override final;
   
   static const std::string _templateName;
   
};


class CMakeFileWriter : public BaseWriter 
{
   public:
  
   CMakeFileWriter();
   virtual ~CMakeFileWriter() = default;
   
   
   virtual void init( const std::string&         fileName,
		      const UnitTestData*           data,
		      const ClangCompiler*   compiler ) override final;
   
		      
   virtual const std::string& getTemplateName() const { return _templateName; }
   virtual const std::string& getTemplateSuffix() const { return _templateSuffix; }
   
protected:
   
   virtual const Plustache::Context* createContext() override final;
   
   static const std::string _templateName;
   static const std::string _templateSuffix;
   
private:
   
   std::string _sourceFileName;
   std::string _testFileName;
   
   const std::set<boost::filesystem::path>* _sources;
   const std::set<boost::filesystem::path>* _includePaths;
   const boost::filesystem::path* _workspace;
};




class GlobalsWriter : public BaseWriter 
{
   public:
  
   GlobalsWriter();
   virtual ~GlobalsWriter() = default;
   
   virtual const std::string& getTemplateName() const  override final { return _templateName; }
   virtual const std::string& getTemplateSuffix() const  override final { return _templateSuffix; }
   
   static const std::string _templateSuffix;
   
protected:
   
   virtual const Plustache::Context* createContext() override final;
   
   static const std::string _templateName;
    
    
};


#endif // _UTBuilder_Writers_h__

