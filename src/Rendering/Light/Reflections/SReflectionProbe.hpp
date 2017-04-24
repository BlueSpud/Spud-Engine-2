//
// Created by Logan Pazol on 4/22/17.
//

#ifndef SReflectionProbe_hpp
#define SReflectionProbe_hpp

#include "Rendering/Objects/SObject.hpp"
#include "Rendering/SRenderSystem.hpp"

class SReflectionProbe : public SObject {

    public:

        SReflectionProbe(bool updates = false);

        void capture();

        virtual void render(double interpolation);
        virtual void update(const SEvent& event);
        void renderUpdate(const SEvent& event);

        float cube_radius = 10.0;

    private:

        SCubeMap* cubemap;

};


#endif //SReflectionProbe_hpp
