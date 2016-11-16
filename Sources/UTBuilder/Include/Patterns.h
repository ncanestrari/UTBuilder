#ifndef _UTBuilder_Patterns_h__
#define _UTBuilder_Patterns_h__



template <typename T>
class Singleton {
protected:
   Singleton() = default;
   ~Singleton() = default;
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


// Prototype pattern interface
class Prototype
{
   
public:
   
   virtual Prototype* clone(const char * value) const = 0;
   
   virtual ~Prototype() = default;
   
};

#endif /* __UTBuilder_Patterns_h__ */
