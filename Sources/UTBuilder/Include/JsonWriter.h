#ifndef _UTBuilder_JsonWriter_h__
#define _UTBuilder_JsonWriter_h__


#include "DataFile.h"

#include <string>


class JsonWriter {

public:
   JsonWriter(const DataFile& dataFile);
   ~JsonWriter() {}

   void templateFile(const std::string &fileName);
   

private:

   const DataFile& _dataFile;

};

#endif // _UTBuilder_JsonWriter_h__ 
 
