//
// Created by Logan Pazol on 4/22/17.
//

#include "SReflectionProbe.hpp"

SReflectionProbe::SReflectionProbe(bool updates) {

    if (updates)
        event_listener.listenToEvent(EVENT_START_FRAME, EVENT_MEMBER(SReflectionProbe::renderUpdate));

}

void SReflectionProbe::capture() {

    // Delete the cube map we already have
    if (cubemap) {

        cubemap->unload();
        delete cubemap;

    }

    cubemap = SRenderSystem::generateCubeMap(getTranslation());

}

void SReflectionProbe::render(double interpolation) { /* intentionally empty */ }
void SReflectionProbe::update(const SEvent& event) { /* intentionally empty */ }

void SReflectionProbe::renderUpdate(const SEvent& event) { capture(); }