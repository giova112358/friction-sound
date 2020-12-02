/*
  ==============================================================================

    DspProcessor.cpp
    Created: 27 Nov 2020 4:00:33pm
    Author:  Giova

  ==============================================================================
*/

#include "DspProcessor.h"

DspProcessor::DspProcessor(char* key0, char* key1)
{
    inertialResonator = std::make_unique<Inertial>(key0);
    modalResonator = std::make_unique<Modal>(key1, nModes, nPickups);

    frictionModel = std::make_unique<Friction>(key0, key1);  
}

DspProcessor::~DspProcessor()
{
}

void DspProcessor::setExternalForce(double f)
{
    externalForce = f;
    force.store(f);
    
}

void DspProcessor::setSampleRate(double sampleRate)
{
    SDT_setSampleRate(sampleRate);
}

double DspProcessor::process(double f)
{
    /*force.store(f);*/
    SDTInteractor_dsp(frictionModel.get()->getSDTObj(), /*externalForce*//*force.load()*/f, 0, 0, 0, 0, 0, tmpOuts);
    return tmpOuts[1];
}

//======================================================================================
void DspProcessor::reset()
{

}
