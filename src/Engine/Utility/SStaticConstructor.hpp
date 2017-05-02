//
// Created by Logan Pazol on 5/2/17.
//

#ifndef SPUDENGINE2_SSTATICCONSTRUCTOR_HPP
#define SPUDENGINE2_SSTATICCONSTRUCTOR_HPP

template<void(*func)()>
struct SStaticConstructor  {

    struct constructor { constructor() { func(); } void f() {} };
    static constructor c;

};

template<void(*func)()>
typename SStaticConstructor<func>::constructor SStaticConstructor<func>::c;

#endif //SPUDENGINE2_SSTATICCONSTRUCTOR_HPP
