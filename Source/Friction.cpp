/*
  ==============================================================================

    Friction.cpp
    Created: 28 Nov 2020 4:19:30pm
    Author:  Giova

  ==============================================================================
*/

#include "Friction.h"

Friction::Friction(char* key0, char* key1)
{
    friction = SDTFriction_new();
    SDT_registerInteractor(friction, key0, key1);
}

Friction::~Friction()
{
}

void Friction::setFrictionParameters(double force, double stribeck, double kStatic, double kDynamic,
    double stiffness, double dissipation, double viscosity, double noisiness,
    double breakAway, long contact0, long contact1)
{
    SDTFriction_setNormalForce(friction, force);
    SDTFriction_setStribeckVelocity(friction, stribeck);
    SDTFriction_setStaticCoefficient(friction, kStatic);
    SDTFriction_setDynamicCoefficient(friction, kDynamic);
    SDTFriction_setStiffness(friction, stiffness);
    SDTFriction_setDissipation(friction, dissipation);
    SDTFriction_setViscosity(friction, viscosity);
    SDTFriction_setNoisiness(friction, noisiness);
    SDTFriction_setBreakAway(friction, breakAway);

    SDTInteractor_setFirstPoint(friction, contact0);
    SDTInteractor_setSecondPoint(friction, contact1);
}

SDTInteractor* Friction::getSDTObj()
{
    return this->friction;
}