//
//  SClass.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 5/02/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#ifndef SClass_hpp
#define SClass_hpp

#include <iostream>
#include "Level/SLevelFactory.hpp"

class SClass {

    public:

        virtual bool isSubclassOfClass(SClass* other) = 0;

        bool isEqualToClass(SClass* other) { return getHash() == other->getHash(); }

        template <class T>
        void setup() {

            // Save template specific pointers
            // This allows us to store a template without making the class a template class
            check_other = &doCheckOther<T>;
            ptr = &getFauxPtr<T>;
            hash = typeid(T).hash_code();

        }

        bool callCheckOther(SClass* other) {

            // Just call pointer, done to not violate encapsulation
            return check_other(other);

        }

        template <class T>
        bool objectIsClass(T* obj) { return typeid(T).hash_code() == hash; }

    protected:

        template <class T>
        static bool doCheckOther(SClass* other) {

            // Checks if the other class can be cast to this class
            // Uses catch to avoid using dynamic_cast which requires polymorphic objects
            try { other->ptr(); }
            catch (T* caught) { return caught; }
            catch (...) { return 0; }
            return 0;

        }

        template <class T>
        static void getFauxPtr() {

            // Return a fake pointer of template T. This is used for inheritance testing but should never be used elsewhere
            throw (T*)0xffffffff;

        }

        virtual size_t getHash() = 0;

        void (*ptr)();
        bool (*check_other)(SClass* other);
        size_t hash;

};

template <class T>
class SClassT : public SClass {

    public:

        virtual bool isSubclassOfClass(SClass* other) {

            // Checks if this can be cast to the other class, if it can, this class inherited from the other
            return other->callCheckOther(this);

        }

    protected:

        virtual size_t getHash() { return typeid(T).hash_code(); }

};

#define SCLASS_ABSTRACT(n) public:  \
           static SClass* staticClass() { \
                                            static SClass* _class = nullptr; \
                                            if (!_class) { \
                                                _class = new SClassT<n>(); \
                                                _class->setup<n>(); \
                                            } \
                                            return _class; \
                                        } \
\
                  virtual bool isKindOfClass(SClass* _class) { return _class->objectIsClass(this); } \
                  virtual bool inheritsFromClass(SClass* _class) { return n::staticClass()->isSubclassOfClass(_class); } \
                  virtual SClass* getClass() { return n::staticClass(); } \

#define SCLASS(n) SCLASS_ABSTRACT(n) \
                  REGISTER_CLASS(n)

#endif /* SClass_hpp */
