/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FrictionModelAudioProcessor::FrictionModelAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),apvts(*this, nullptr, "Parameters", createParameterLayout())
#endif
{
    /*apvts.addParameterListener("VEL", this);*/
    apvts.addParameterListener("MASS", this);
    /*apvts.addParameterListener("FOR", this);*/
    apvts.addParameterListener("FREQ0", this);
    apvts.addParameterListener("FREQ1", this);
    apvts.addParameterListener("FREQ2", this);
    apvts.addParameterListener("DEC0", this);
    apvts.addParameterListener("DEC1", this);
    apvts.addParameterListener("DEC2", this);
    apvts.addParameterListener("GAINPICK10", this);
    apvts.addParameterListener("GAINPICK11", this);
    apvts.addParameterListener("GAINPICK12", this);
    apvts.addParameterListener("FOR", this);
    apvts.addParameterListener("STR", this);
    apvts.addParameterListener("KST", this);
    apvts.addParameterListener("KDY", this);
    apvts.addParameterListener("STIFF", this);
    apvts.addParameterListener("DISS", this);
    apvts.addParameterListener("VIS", this);
    apvts.addParameterListener("NOIS", this);
    apvts.addParameterListener("BREAK", this);


    /*apvts.addParameterListener("BANG", this);*/
    apvts.addParameterListener("VOL", this);

    init();
}

FrictionModelAudioProcessor::~FrictionModelAudioProcessor()
{
}

//==============================================================================
const juce::String FrictionModelAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FrictionModelAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FrictionModelAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FrictionModelAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FrictionModelAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FrictionModelAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FrictionModelAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FrictionModelAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String FrictionModelAudioProcessor::getProgramName (int index)
{
    return {};
}

void FrictionModelAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void FrictionModelAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    prepare(sampleRate);
    updateVolume();
    updateInertialParameters();
    updateModalParameters();
    updateFrictionParameters();
    reset();
    isActive = true;
}

void FrictionModelAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FrictionModelAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void FrictionModelAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if (!isActive)
        return;
    if (mustUpdateVolume)
        updateVolume();
    if (mustUpdateInertialParameters)
        updateInertialParameters();
    if (mustUpdateModalParameters)
        updateModalParameters();
    if (mustUpdateFrictionParameters)
        updateFrictionParameters();
    //if (mustStrike)
    //    strike();

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto numSamples = buffer.getNumSamples();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample) {
            channelData[sample] = 1000 * model[channel]->process();
            channelData[sample] = juce::jlimit(-1.0f, 1.0f, channelData[sample]);
            //DBG(channelData[sample]);
        }

        mVolume[channel].applyGain(channelData, numSamples);
    }
}

//==============================================================================
bool FrictionModelAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FrictionModelAudioProcessor::createEditor()
{
    /*return new FrictionModelAudioProcessorEditor (*this);*/
    return new juce::GenericAudioProcessorEditor(this);
}

//==============================================================================
void FrictionModelAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::ValueTree copyState = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml = copyState.createXml();
    copyXmlToBinary(*xml.get(), destData);
}

void FrictionModelAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml = getXmlFromBinary(data, sizeInBytes);
    juce::ValueTree copyState = juce::ValueTree::fromXml(*xml.get());
    apvts.replaceState(copyState);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FrictionModelAudioProcessor();
}

void FrictionModelAudioProcessor::init()
{
    char ine1[9] = { "rubberf0" };
    char mod1[9] = { "objectf0" };
    char ine2[9] = { "rubberf1" };
    char mod2[9] = { "objectf1" };

    model[0] = std::make_unique<DspProcessor>(ine1, mod1);
    model[1] = std::make_unique<DspProcessor>(ine2, mod2);
}

void FrictionModelAudioProcessor::prepare(double sampleRate)
{
    for (int channel = 0; channel < numChannels; ++channel) {
        model[channel]->setSampleRate(sampleRate);
    }
}

//===============================================================================
void FrictionModelAudioProcessor::reset()
{
    for (int channel = 0; channel < numChannels; ++channel) {
        mVolume[channel].reset(getSampleRate(), 0.050);
    }
}

//===============================================================================
void FrictionModelAudioProcessor::addInertialParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    std::function<juce::String(double, int)> valueToTextFunction =
        [](double x, int l) {return juce::String(x, 4);  };
    std::function<double(const juce::String&)> textToValueFunction =
        [](const juce::String& str) {return str.getFloatValue(); };

    //auto velocity = std::make_unique<juce::AudioParameterFloat>("VEL", "Velocity",
    //    juce::NormalisableRange<double>(0.001, 40.0), 3.741,
    //    "m/s", juce::AudioProcessorParameter::genericParameter, 
    //    valueToTextFunction, textToValueFunction);

    auto mass = std::make_unique<juce::AudioParameterFloat>("MASS", "Mass",
        juce::NormalisableRange<float>(0.001, 0.1), 0.011184,
        "Kg", juce::AudioProcessorParameter::genericParameter,
        valueToTextFunction, textToValueFunction);

    //auto force = std::make_unique<juce::AudioParameterFloat>("FOR", "Force", 
    //    juce::NormalisableRange<double>(0.0, 0.2), 0.0,
    //    "N", juce::AudioProcessorParameter::genericParameter, 
    //    valueToTextFunction, textToValueFunction);

    auto group = std::make_unique<juce::AudioProcessorParameterGroup>("sdt.inertial", "HAMMER CONTROLS", "|",
        /*std::move(velocity), */std::move(mass)/*, std::move(force)*/);

    layout.add(std::move(group));
}

void FrictionModelAudioProcessor::addModalParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    std::function<juce::String(double, int)> valueToTextFunction =
        [](double x, int l) {return juce::String(x, 4);  };
    std::function<double(const juce::String&)> textToValueFunction =
        [](const juce::String& str) {return str.getFloatValue(); };

    auto freq0 = std::make_unique<juce::AudioParameterFloat>("FREQ0", "Frequency0",
        juce::NormalisableRange<float>(20.0, 5000), 1155,
        "Hz", juce::AudioProcessorParameter::genericParameter,
        valueToTextFunction, textToValueFunction);

    auto freq1 = std::make_unique<juce::AudioParameterFloat>("FREQ1", "Frequency1",
        juce::NormalisableRange<float>(20.0, 5000), 1848,
        "Hz", juce::AudioProcessorParameter::genericParameter,
        valueToTextFunction, textToValueFunction);

    auto freq2 = std::make_unique<juce::AudioParameterFloat>("FREQ2", "Frequency2",
        juce::NormalisableRange<float>(20.0, 5000), 3580.5,
        "Hz", juce::AudioProcessorParameter::genericParameter,
        valueToTextFunction, textToValueFunction);

    auto dec0 = std::make_unique<juce::AudioParameterFloat>("DEC0", "Decay0",
        juce::NormalisableRange<float>(0.0, 1.0), 0.00231,
        "", juce::AudioProcessorParameter::genericParameter,
        valueToTextFunction, textToValueFunction);

    auto dec1 = std::make_unique<juce::AudioParameterFloat>("DEC1", "Decay1",
        juce::NormalisableRange<float>(0.0, 1.0), 0.00154,
        "", juce::AudioProcessorParameter::genericParameter,
        valueToTextFunction, textToValueFunction);

    auto dec2 = std::make_unique<juce::AudioParameterFloat>("DEC2", "Decay2",
        juce::NormalisableRange<float>(0.0, 1.0), 0.00154,
        "", juce::AudioProcessorParameter::genericParameter,
        valueToTextFunction, textToValueFunction);

    auto gain10 = std::make_unique<juce::AudioParameterFloat>("GAINPICK10", "Gain Pickup1 Mode0",
        juce::NormalisableRange<float>(10, 100), 80,
        "", juce::AudioProcessorParameter::genericParameter,
        valueToTextFunction, textToValueFunction);

    auto gain11 = std::make_unique<juce::AudioParameterFloat>("GAINPICK11", "Gain Pickup1 Mode1",
        juce::NormalisableRange<float>(10, 100), 80,
        "", juce::AudioProcessorParameter::genericParameter,
        valueToTextFunction, textToValueFunction);

    auto gain12 = std::make_unique<juce::AudioParameterFloat>("GAINPICK12", "Gain Pickup1 Mode2",
        juce::NormalisableRange<float>(10, 100), 80,
        "", juce::AudioProcessorParameter::genericParameter,
        valueToTextFunction, textToValueFunction);

    auto group = std::make_unique<juce::AudioProcessorParameterGroup>("sdt.modal", "RESONATOR CONTROLS", "|",
        std::move(freq0), std::move(freq1), std::move(freq2),
        std::move(dec0), std::move(dec1), std::move(dec2), std::move(gain10),
        std::move(gain11), std::move(gain12));

    layout.add(std::move(group));
}

void FrictionModelAudioProcessor::addFrictionParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    std::function<juce::String(double, int)> valueToTextFunction =
        [](double x, int l) {return juce::String(x, 4);  };
    std::function<double(const juce::String&)> textToValueFunction =
        [](const juce::String& str) {return str.getFloatValue(); };

    auto force = std::make_unique<juce::AudioParameterFloat>("FOR", "Force",
        juce::NormalisableRange<float>(500, 5000), 1606.616753,
        "", juce::AudioProcessorParameter::genericParameter,
        valueToTextFunction, textToValueFunction);

    auto stribeck = std::make_unique<juce::AudioParameterFloat>("STR", "Stribeck",
        juce::NormalisableRange<float>(0.096, 1.0), 0.103036,
        "", juce::AudioProcessorParameter::genericParameter,
        valueToTextFunction, textToValueFunction);

    auto kStatic = std::make_unique<juce::AudioParameterFloat>("KST", "kStatic",
        juce::NormalisableRange<float>(0.4, 1.0), 0.922222,
        "", juce::AudioProcessorParameter::genericParameter,
        valueToTextFunction, textToValueFunction);

    auto kDynamic = std::make_unique<juce::AudioParameterFloat>("KDY", "kDynamic",
        juce::NormalisableRange<float>(0.01, 0.5), 0.3775,
        "", juce::AudioProcessorParameter::genericParameter,
        valueToTextFunction, textToValueFunction);

    auto stiffness = std::make_unique<juce::AudioParameterFloat>("STIFF", "Stiffness",
        juce::NormalisableRange<float>(500.0, 5000.0), 1606.616753,
        "", juce::AudioProcessorParameter::genericParameter,
        valueToTextFunction, textToValueFunction);

    auto dissipation = std::make_unique<juce::AudioParameterFloat>("DISS", "Dissipation",
        juce::NormalisableRange<float>(0.0, 40.0), 35.925925,
        "", juce::AudioProcessorParameter::genericParameter,
        valueToTextFunction, textToValueFunction);

    auto viscosity = std::make_unique<juce::AudioParameterFloat>("VIS", "Viscosity",
        juce::NormalisableRange<float>(0.0, 10.0), 5.555556,
        "", juce::AudioProcessorParameter::genericParameter,
        valueToTextFunction, textToValueFunction);

    auto noisiness = std::make_unique<juce::AudioParameterFloat>("NOIS", "Noisiness",
        juce::NormalisableRange<float>(0.01, 1.0), 0.844167,
        "", juce::AudioProcessorParameter::genericParameter,
        valueToTextFunction, textToValueFunction);

    auto breakAway = std::make_unique<juce::AudioParameterFloat>("BREAK", "Breakaway",
        juce::NormalisableRange<float>(0.1, 1.0), 0.725,
        "", juce::AudioProcessorParameter::genericParameter,
        valueToTextFunction, textToValueFunction);

    auto group = std::make_unique<juce::AudioProcessorParameterGroup>("sdt.friction", "FRICTION CONTROLS", "|",
        std::move(force), std::move(stribeck), std::move(kStatic),
        std::move(kDynamic), std::move(stiffness), std::move(dissipation), std::move(viscosity),
        std::move(noisiness), std::move(breakAway));

    layout.add(std::move(group));

}

void FrictionModelAudioProcessor::addGainParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    std::function<juce::String(float, int)> valueToTextFunction =
        [](float x, int l) {return juce::String(x, 4);  };
    std::function<float(const juce::String&)> textToValueFunction =
        [](const juce::String& str) {return str.getFloatValue(); };

    /*auto bang = std::make_unique<juce::AudioParameterBool>("BANG", "bang", false);*/

    auto volume = std::make_unique<juce::AudioParameterFloat>("VOL", "Volume",
        juce::NormalisableRange< float >(-40.0f, 10.0f), 0.0f,
        "dB", juce::AudioProcessorParameter::genericParameter,
        valueToTextFunction, textToValueFunction);

    auto group = std::make_unique<juce::AudioProcessorParameterGroup>("sdt.gain", "VOLUME CONTROLS", "|",
        std::move(volume)/*, std::move(bang)*/);

    layout.add(std::move(group));

}

juce::AudioProcessorValueTreeState::ParameterLayout FrictionModelAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    FrictionModelAudioProcessor::addInertialParameters(layout);
    FrictionModelAudioProcessor::addModalParameters(layout);
    FrictionModelAudioProcessor::addFrictionParameters(layout);
    FrictionModelAudioProcessor::addGainParameters(layout);
    return layout;
}

//===============================================================================
void FrictionModelAudioProcessor::updateInertialParameters()
{
    mustUpdateInertialParameters = false;
    auto m = apvts.getRawParameterValue("MASS");
    //auto vel = apvts.getRawParameterValue("VEL");
    /*auto f = apvts.getRawParameterValue("FOR");*/

    double mass = m->load();
    //double velocity = vel->load();
    /*double force = f->load();*/

    for (int channel = 0; channel < numChannels; ++channel) {
        model[channel]->inertialResonator->setInertialParameters(mass, 1.0);
        /*model[channel]->setExtenalForce(-1 * force);*/
        /*model[channel]->setStrike(0.0, -1 * velocity);*/
    }
    //strike();
}

void FrictionModelAudioProcessor::updateModalParameters()
{
    mustUpdateModalParameters = false;
    auto freq0 = apvts.getRawParameterValue("FREQ0");
    auto freq1 = apvts.getRawParameterValue("FREQ1");
    auto freq2 = apvts.getRawParameterValue("FREQ2");
    auto dec0 = apvts.getRawParameterValue("DEC0");
    auto dec1 = apvts.getRawParameterValue("DEC1");
    auto dec2 = apvts.getRawParameterValue("DEC2");
    auto gain10 = apvts.getRawParameterValue("GAINPICK10");
    auto gain11 = apvts.getRawParameterValue("GAINPICK11");
    auto gain12 = apvts.getRawParameterValue("GAINPICK12");

    double freqs[SDT_MAX_MODES]{ freq0->load(), freq1->load(), freq2->load() };
    double decays[SDT_MAX_MODES]{ dec0->load(), dec1->load(), dec2->load() };
    double gains[SDT_MAX_PICKUPS][SDT_MAX_MODES]{ {gain10->load(), gain11->load(), gain12->load()}, {0.0, 0.0, 0.0} };

    for (int channel = 0; channel < numChannels; ++channel) {
        model[channel]->modalResonator->setModalParameters(freqs, decays, gains, 1.0, 3);
    }
    //strike();
}

void FrictionModelAudioProcessor::updateFrictionParameters()
{
    mustUpdateFrictionParameters = false;
    auto forc = apvts.getRawParameterValue("FOR");
    auto stri = apvts.getRawParameterValue("STR");
    auto kSta = apvts.getRawParameterValue("KST");
    auto kDyn = apvts.getRawParameterValue("KDY");
    auto stif = apvts.getRawParameterValue("STIFF");
    auto diss = apvts.getRawParameterValue("DISS");
    auto visc = apvts.getRawParameterValue("VIS");
    auto noisi = apvts.getRawParameterValue("NOIS");
    auto breakA = apvts.getRawParameterValue("BREAK");

    double force = forc->load();
    double stribeck = stri->load();
    double kStatic = kSta->load();
    double kDynamic = kDyn->load();
    double stiffness = stif->load();
    double dissipation = diss->load();
    double viscosity = visc->load();
    double noisiness = noisi->load();
    double breakAway = breakA->load();

    for (int channel = 0; channel < numChannels; ++channel) {
        model[channel]->frictionModel->setFrictionParameters(force, stribeck,
            kStatic, kDynamic, stiffness, dissipation,
            viscosity, noisiness, breakAway, 0, 0);
    }
}

void FrictionModelAudioProcessor::updateVolume()
{
    mustUpdateVolume = false;

    auto volume = apvts.getRawParameterValue("VOL");

    for (int channel = 0; channel < numChannels; ++channel) {
        mVolume[channel].setTargetValue(juce::Decibels::decibelsToGain(volume->load()));
    }
}

void FrictionModelAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    //if (parameterID == "VEL")
    //    mustUpdateInertialParameters = true;
    //if (parameterID == "FOR")
    //    mustUpdateInertialParameters = true;
    if (parameterID == "MASS")
        mustUpdateInertialParameters = true;
    if (parameterID == "FREQ0")
        mustUpdateModalParameters = true;
    if (parameterID == "FREQ1")
        mustUpdateModalParameters = true;
    if (parameterID == "FREQ2")
        mustUpdateModalParameters = true;
    if (parameterID == "DEC0")
        mustUpdateModalParameters = true;
    if (parameterID == "DEC1")
        mustUpdateModalParameters = true;
    if (parameterID == "DEC2")
        mustUpdateModalParameters = true;
    if (parameterID == "GAINPICK10")
        mustUpdateModalParameters = true;
    if (parameterID == "GAINPICK11")
        mustUpdateModalParameters = true;
    if (parameterID == "GAINPICK12")
        mustUpdateModalParameters = true;
    if (parameterID == "FOR" )
        mustUpdateFrictionParameters = true;
    if (parameterID == "STR" )
        mustUpdateFrictionParameters = true;
    if (parameterID == "KST")
        mustUpdateFrictionParameters = true;
    if (parameterID == "KDY")
        mustUpdateFrictionParameters = true;
    if (parameterID == "STIFF")
        mustUpdateFrictionParameters = true;
    if (parameterID == "DISS")
        mustUpdateFrictionParameters = true;
    if (parameterID == "VIS")
        mustUpdateFrictionParameters = true;
    if (parameterID == "NOIS")
        mustUpdateFrictionParameters = true;
    if (parameterID == "BREAK")
        mustUpdateFrictionParameters = true;
    if (parameterID == "VOL")
        mustUpdateVolume = true;
    //if (parameterID == "BANG")
    //    mustStrike = true;
}
