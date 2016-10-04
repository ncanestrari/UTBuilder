#ifndef __SINGLETON_H_
#define __SINGLETON_H_

#include <cstddef>  // defines NULL


template <typename T>
class Singleton {
protected:
   Singleton() {};
   ~Singleton() {};
private:
   Singleton(Singleton const&);
   Singleton& operator=(Singleton const&);
public:

   static T &get(void)
   {
      static T instance;
      return instance;
   }
};


#endif /* __SINGLETON_H_ */
