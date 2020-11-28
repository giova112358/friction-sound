/*
  ==============================================================================

    Friction.h
    Created: 28 Nov 2020 4:19:30pm
    Author:  Giova

    Basic solid friction interaction algorithm. The interaction is
    represented by the structure resonator-interactor-resonator
    object models expose two pickup points, for the resonating object,
    and one pickup point for the exciter. Each pickup point gives information
    about displacement and velocity, and can as well be used to apply
    an external force to the resonator.
  ==============================================================================
*/

#pragma once
#include "SDTCommon.h"
#include "SDTSolids.h"

class Friction
{
public:
    Friction(char* key0, char* key1);
    ~Friction();

    /*
     ============================================================================
     Simulates a non-linear friction, by computing the
     friction force from the relative velocity between the two
     contact points
     =============================================================================
    */
    void setFrictionParameters(double force, double stribeck, double kStatic, double kDynamic, 
        double stiffness, double dissipation, double viscosity, double noisiness, 
        double breakAway, long contact0, long contact1);

    SDTInteractor* getSDTObj();

private:
    SDTInteractor* friction; /*friction inetractor object*/


};