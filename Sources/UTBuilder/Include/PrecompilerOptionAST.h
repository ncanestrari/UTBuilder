#ifndef PRECOMPILEROPTIONAST_H
#define PRECOMPILEROPTIONAST_H


#define LIST_SIZE(list) (sizeof((list))/sizeof(*(list))) // calculate the length of the array

namespace PrecompilerOptionAST{

const char *MiddlewareMocks[] = {//Maybe in an external list???
   "Em_Malloc", "Em_IMalloc", "Em_Realloc", "Em_Free", "Em_IRealloc", //That is all
   "IsMemoryStatusOK", 
   "PrintLine", 
   "AllocateInitPrObject", "AllocatePrObject", "AllocatePrObjectHdr", "IncreasePrObject"
   "TrapKeyIsOn",
   //Add here and in mock.template if needed
};

// additional paths needed for compiling (system or compiler specific)
const char *additionalIncludePaths[] = {
   "/usr/include",
   "/usr/lib64/clang/3.5.0/include", //this might differ!!!
};

// additional predefined macros needed for preprocessing
const char *additionalPredefinedMacros[] = {
   "UNIT_TEST",
   "__UNIX64__",
   "__64BIT__",
   "__LINUX64__",
   "NDEBUG"
};

}

#endif /* PRECOMPILEROPTIONAST_H */