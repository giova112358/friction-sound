/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class FrictionModelAudioProcessorEditor  : public juce::AudioProcessorEditor,
    juce::Button::Listener
{
public:
    FrictionModelAudioProcessorEditor (FrictionModelAudioProcessor&);
    ~FrictionModelAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    //==============================================================================
    void buttonClicked(juce::Button* button) override;

private:
    struct RightSidePanel : public Component
    {
        RightSidePanel(FrictionModelAudioProcessor& p) : audioProcessor(p)
        {
            stribeckSlider = std::make_unique<juce::Slider >(juce::Slider::SliderStyle::Rotary, juce::Slider::TextBoxBelow);
            addAndMakeVisible(stribeckSlider.get());
            stribeckAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "STR", *stribeckSlider);
            stribeckLabel = std::make_unique<juce::Label>("", "Stribeck");
            addAndMakeVisible(stribeckLabel.get());
            stribeckLabel->attachToComponent(stribeckSlider.get(), false);
            stribeckLabel->setJustificationType(juce::Justification::centredTop);

            stiffnessSlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::Rotary, juce::Slider::TextBoxBelow);
            addAndMakeVisible(stiffnessSlider.get());
            stiffnessAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "STIFF", *stiffnessSlider);
            stiffnessLabel = std::make_unique<juce::Label>("", "Stiffness");
            addAndMakeVisible(stiffnessLabel.get());
            stiffnessLabel->attachToComponent(stiffnessSlider.get(), false);
            stiffnessLabel->setJustificationType(juce::Justification::centredBottom);


            dissipationSlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::Rotary, juce::Slider::TextBoxBelow);
            addAndMakeVisible(dissipationSlider.get());
            dissipationAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DISS", *dissipationSlider);
            dissipationLabel = std::make_unique<juce::Label>("", "Dissipation");
            addAndMakeVisible(dissipationLabel.get());
            dissipationLabel->attachToComponent(dissipationSlider.get(), false);
            dissipationLabel->setJustificationType(juce::Justification::centredBottom);

            kStaticSlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::Rotary, juce::Slider::TextBoxBelow);
            addAndMakeVisible(kStaticSlider.get());
            kStaticAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "KST", *kStaticSlider);
            kStaticLabel = std::make_unique<juce::Label>("", "KStatic");
            addAndMakeVisible(kStaticLabel.get());
            kStaticLabel->attachToComponent(kStaticSlider.get(), false);
            kStaticLabel->setJustificationType(juce::Justification::centredBottom);

            kDynamicSlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::Rotary, juce::Slider::TextBoxBelow);
            addAndMakeVisible(kDynamicSlider.get());
            kDynamicAttch = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "KDY", *kDynamicSlider);
            kDynamicLabel = std::make_unique<juce::Label>("", "KDynamic");
            addAndMakeVisible(kDynamicLabel.get());
            kDynamicLabel->attachToComponent(kDynamicSlider.get(), false);
            kDynamicLabel->setJustificationType(juce::Justification::centredBottom);

            viscositySlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::Rotary, juce::Slider::TextBoxBelow);
            addAndMakeVisible(viscositySlider.get());
            viscosityAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "VIS", *viscositySlider);
            viscosityLabel = std::make_unique<juce::Label>("", "Viscosity");
            addAndMakeVisible(viscosityLabel.get());
            viscosityLabel->attachToComponent(viscositySlider.get(), false);
            viscosityLabel->setJustificationType(juce::Justification::centredBottom);

            noisinessSlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::Rotary, juce::Slider::TextBoxBelow);
            addAndMakeVisible(noisinessSlider.get());
            noisinessAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "NOIS", *noisinessSlider);
            noisinessLabel = std::make_unique<juce::Label>("", "Noisiness");
            addAndMakeVisible(noisinessLabel.get());
            noisinessLabel->attachToComponent(noisinessSlider.get(), false);
            noisinessLabel->setJustificationType(juce::Justification::centredBottom);

            breakAwaySlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::Rotary, juce::Slider::TextBoxBelow);
            addAndMakeVisible(breakAwaySlider.get());
            breakAwayAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "BREAK", *breakAwaySlider);
            breakAwayLabel = std::make_unique<juce::Label>("", "Break Away");
            addAndMakeVisible(noisinessLabel.get());
            breakAwayLabel->attachToComponent(breakAwaySlider.get(), false);
            breakAwayLabel->setJustificationType(juce::Justification::centredBottom);

            volSlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::Rotary, juce::Slider::TextBoxBelow);
            addAndMakeVisible(volSlider.get());
            volAttch = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "VOL", *volSlider);
            volLabel = std::make_unique<juce::Label>("", "Gain");
            addAndMakeVisible(volLabel.get());
            volLabel->attachToComponent(volSlider.get(), false);
            volLabel->setJustificationType(juce::Justification::centredBottom);

        }

        void paint(juce::Graphics& g) override
        {
            /*juce::Rectangle<int> titleArea(0, 10, getWidth(), 10);

            g.setColour(juce::Colours::white);
            g.drawText("IMPACT CONTROLS ", titleArea, juce::Justification::centredTop);*/

            /*juce::Rectangle <float> area(getWidth()-5, getHeight()+10);

            g.setColour(juce::Colours::yellow);
            g.drawRoundedRectangle(area, 20.0f, 2.0f);*/
        }

        void resized() override
        {
            auto bounds = getLocalBounds();
            /*bounds.removeFromTop(20);*/
            bounds.removeFromTop(20);
            juce::Grid grid;
            using Track = juce::Grid::TrackInfo;
            using Fr = juce::Grid::Fr;

            grid.items.add((juce::GridItem(stiffnessSlider.get())));
            grid.items.add((juce::GridItem(dissipationSlider.get())));
            grid.items.add((juce::GridItem(viscositySlider.get())));
            grid.items.add((juce::GridItem(kDynamicSlider.get())));
            grid.items.add((juce::GridItem(kStaticSlider.get())));
            grid.items.add((juce::GridItem(breakAwaySlider.get())));
            grid.items.add((juce::GridItem(stribeckSlider.get())));
            grid.items.add((juce::GridItem(noisinessSlider.get())));
            grid.items.add((juce::GridItem(volSlider.get())));


            grid.templateRows = { Track(Fr(1)), Track(Fr(1)), Track(Fr(1))//,
            /*Track(Fr(1)), Track(Fr(1)), Track(Fr(1)), Track(Fr(1)),
            Track(Fr(1)), Track(Fr(1))*/ };
            grid.templateColumns = { Track(Fr(1)), Track(Fr(1)), Track(Fr(1)) };
            grid.columnGap = juce::Grid::Px(10);
            grid.rowGap = juce::Grid::Px(50);

            grid.performLayout(bounds);
        }

        std::unique_ptr<juce::Slider> volSlider, stribeckSlider, kStaticSlider, kDynamicSlider,
            stiffnessSlider, dissipationSlider, viscositySlider, noisinessSlider,
            breakAwaySlider;

        std::unique_ptr<juce::Label> volLabel, stribeckLabel, kStaticLabel, kDynamicLabel,
            stiffnessLabel, dissipationLabel, viscosityLabel, noisinessLabel,
            breakAwayLabel;

        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volAttch, stribeckAttach, kStaticAttach, kDynamicAttch,
            stiffnessAttach, dissipationAttach, viscosityAttach, noisinessAttach,
            breakAwayAttach;


        FrictionModelAudioProcessor& audioProcessor;
    };

    struct LeftSidePanel : public Component
    {
        LeftSidePanel(FrictionModelAudioProcessor& p) : audioProcessor(p)
        {
            massSlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::Rotary, juce::Slider::TextBoxBelow);
            addAndMakeVisible(massSlider.get());
            massAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "MASS", *massSlider);
            massLabel = std::make_unique<juce::Label>("", "Mass");
            addAndMakeVisible(massLabel.get());
            massLabel->attachToComponent(massSlider.get(), false);
            massLabel->setJustificationType(juce::Justification::centredBottom);

            extForceSlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::Rotary, juce::Slider::TextBoxBelow);
            addAndMakeVisible(extForceSlider.get());
            extForceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "EXTFOR", *extForceSlider);
            extForceLabel = std::make_unique<juce::Label>("", "External Force");
            addAndMakeVisible(extForceLabel.get());
            extForceLabel->attachToComponent(extForceSlider.get(), false);
            extForceLabel->setJustificationType(juce::Justification::centredBottom);

            forceSlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::Rotary, juce::Slider::TextBoxBelow);
            addAndMakeVisible(forceSlider.get());
            forceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "FOR", *forceSlider);
            forceLabel = std::make_unique<juce::Label>("", "Force");
            addAndMakeVisible(forceLabel.get());
            forceLabel->attachToComponent(forceSlider.get(), false);
            forceLabel->setJustificationType(juce::Justification::centredBottom);
        }

        void paint(juce::Graphics& g) override
        {
            /*juce::Rectangle<int> titleArea(0, 10, getWidth(), 10);

            g.setColour(juce::Colours::white);
            g.drawText("RESONATOR CONTROLS ", titleArea, juce::Justification::centredTop);*/

            /*juce::Rectangle <float> area(getWidth()-5, getHeight()+10);

            g.setColour(juce::Colours::yellow);
            g.drawRoundedRectangle(area, 20.0f, 2.0f);*/
        }

        void resized() override
        {
            auto bounds = getLocalBounds();
            bounds.removeFromTop(40);
            juce::Grid grid;
            using Track = juce::Grid::TrackInfo;
            using Fr = juce::Grid::Fr;

            grid.items.add((juce::GridItem(massSlider.get())));
            grid.items.add((juce::GridItem(extForceSlider.get())));
            grid.items.add((juce::GridItem(forceSlider.get())));

            grid.templateRows = { Track(Fr(1)), Track(Fr(1)), Track(Fr(1)),};
            grid.templateColumns = { Track(Fr(1)) };
            grid.columnGap = juce::Grid::Px(10);
            grid.rowGap = juce::Grid::Px(50);

            grid.performLayout(bounds);
        }

        std::unique_ptr<juce::Slider> massSlider, forceSlider, extForceSlider;

        std::unique_ptr<juce::Label>
            massLabel, forceLabel, extForceLabel;

        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
            massAttachment, forceAttachment, extForceAttachment;

        FrictionModelAudioProcessor& audioProcessor;
    };

    struct MainPanel : public Component
    {
        MainPanel(FrictionModelAudioProcessor& p) : audioProcessor(p)
        {
            freq0Slider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::LinearHorizontal, juce::Slider::TextBoxBelow);
            addAndMakeVisible(freq0Slider.get());
            freq0Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "FREQ0", *freq0Slider);
            freq0Label = std::make_unique<juce::Label>("", "Frequency 0");
            addAndMakeVisible(freq0Label.get());
            freq0Label->attachToComponent(freq0Slider.get(), false);
            freq0Label->setJustificationType(juce::Justification::centredBottom);

            freq1Slider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::LinearHorizontal, juce::Slider::TextBoxBelow);
            addAndMakeVisible(freq1Slider.get());
            freq1Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "FREQ1", *freq1Slider);
            freq1Label = std::make_unique<juce::Label>("", "Frequency 1");
            addAndMakeVisible(freq1Label.get());
            freq1Label->attachToComponent(freq1Slider.get(), false);
            freq1Label->setJustificationType(juce::Justification::centredBottom);

            freq2Slider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::LinearHorizontal, juce::Slider::TextBoxBelow);
            addAndMakeVisible(freq2Slider.get());
            freq2Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "FREQ2", *freq2Slider);
            freq2Label = std::make_unique<juce::Label>("", "Frequency 2");
            addAndMakeVisible(freq2Label.get());
            freq2Label->attachToComponent(freq2Slider.get(), false);
            freq2Label->setJustificationType(juce::Justification::centredBottom);

            decay0Slider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::LinearHorizontal, juce::Slider::TextBoxBelow);
            addAndMakeVisible(decay0Slider.get());
            decay0Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DEC0", *decay0Slider);
            decay0Label = std::make_unique<juce::Label>("", "Decay 0");
            addAndMakeVisible(decay0Label.get());
            decay0Label->attachToComponent(decay0Slider.get(), false);
            decay0Label->setJustificationType(juce::Justification::centredBottom);

            decay1Slider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::LinearHorizontal, juce::Slider::TextBoxBelow);
            addAndMakeVisible(decay1Slider.get());
            decay1Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DEC1", *decay1Slider);
            decay1Label = std::make_unique<juce::Label>("", "Decay 1");
            addAndMakeVisible(decay1Label.get());
            decay1Label->attachToComponent(decay1Slider.get(), false);
            decay1Label->setJustificationType(juce::Justification::centredBottom);

            decay2Slider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::LinearHorizontal, juce::Slider::TextBoxBelow);
            addAndMakeVisible(decay2Slider.get());
            decay2Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DEC2", *decay2Slider);
            decay2Label = std::make_unique<juce::Label>("", "Decay 2");
            addAndMakeVisible(decay2Label.get());
            decay2Label->attachToComponent(decay2Slider.get(), false);
            decay2Label->setJustificationType(juce::Justification::centredBottom);

            gain10Slider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::LinearHorizontal, juce::Slider::TextBoxBelow);
            addAndMakeVisible(gain10Slider.get());
            gain10Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "GAINPICK10", *gain10Slider);
            gain10Label = std::make_unique<juce::Label>("", "Gain Pickup1 Mode0");
            addAndMakeVisible(gain10Label.get());
            gain10Label->attachToComponent(gain10Slider.get(), false);
            gain10Label->setJustificationType(juce::Justification::centredBottom);

            gain11Slider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::LinearHorizontal, juce::Slider::TextBoxBelow);
            addAndMakeVisible(gain11Slider.get());
            gain11Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "GAINPICK11", *gain11Slider);
            gain11Label = std::make_unique<juce::Label>("", "Gain Pickup1 Mode1");
            addAndMakeVisible(gain11Label.get());
            gain11Label->attachToComponent(gain11Slider.get(), false);
            gain11Label->setJustificationType(juce::Justification::centredBottom);

            gain12Slider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::LinearHorizontal, juce::Slider::TextBoxBelow);
            addAndMakeVisible(gain12Slider.get());
            gain12Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "GAINPICK12", *gain12Slider);
            gain12Label = std::make_unique<juce::Label>("", "Gain Pickup1 Mode2");
            addAndMakeVisible(gain12Label.get());
            gain12Label->attachToComponent(gain12Slider.get(), false);
            gain12Label->setJustificationType(juce::Justification::centredBottom);
        }

        void paint(juce::Graphics& g) override
        {
            /*auto bounds = getLocalBounds();
            auto textBounds = bounds.removeFromTop(5);
            g.setColour(juce::Colours::white);
            g.setFont(juce::Font(12.0f).italicised().withExtraKerningFactor(0.1f));
            g.drawFittedText("RESONATOR CONTROLS", textBounds, juce::Justification::centred, 1);*/
            /*juce::Rectangle<int> titleArea(-10, -10, getWidth(), 10);

            g.setColour(juce::Colours::white);
            g.drawText("INERTIAL CONTROLS ", titleArea, juce::Justification::centredTop);*/

            /*juce::Rectangle <float> area(getWidth()-5, getHeight()+10);

            g.setColour(juce::Colours::yellow);
            g.drawRoundedRectangle(area, 20.0f, 2.0f);*/

        }

        void resized() override
        {
            auto bounds = getLocalBounds();
            /*bounds.removeFromTop(15);*/
            bounds.removeFromTop(20);
            juce::Grid grid;
            using Track = juce::Grid::TrackInfo;
            using Fr = juce::Grid::Fr;

            grid.items.add((juce::GridItem(freq0Slider.get())));
            grid.items.add((juce::GridItem(freq1Slider.get())));
            grid.items.add((juce::GridItem(freq2Slider.get())));
            grid.items.add((juce::GridItem(decay0Slider.get())));
            grid.items.add((juce::GridItem(decay1Slider.get())));
            grid.items.add((juce::GridItem(decay2Slider.get())));
            grid.items.add((juce::GridItem(gain10Slider.get())));
            grid.items.add((juce::GridItem(gain11Slider.get())));
            grid.items.add((juce::GridItem(gain12Slider.get())));

            grid.templateRows = { Track(Fr(1)), Track(Fr(1)), Track(Fr(1)),
            Track(Fr(1)), Track(Fr(1)), Track(Fr(1)), Track(Fr(1)), Track(Fr(1)),
            Track(Fr(1)) };
            grid.templateColumns = { Track(Fr(1)) };
            grid.columnGap = juce::Grid::Px(10);
            grid.rowGap = juce::Grid::Px(50);

            grid.performLayout(bounds);
        }

        std::unique_ptr<juce::Slider> freq0Slider, freq1Slider, freq2Slider, decay0Slider,
            decay1Slider, decay2Slider, gain10Slider, gain11Slider, gain12Slider;

        std::unique_ptr<juce::Label> freq0Label, freq1Label, freq2Label, decay0Label,
            decay1Label, decay2Label, gain10Label, gain11Label, gain12Label;

        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freq0Attachment,
            freq1Attachment, freq2Attachment, decay0Attachment, decay1Attachment, decay2Attachment,
            gain10Attachment, gain11Attachment, gain12Attachment;

        FrictionModelAudioProcessor& audioProcessor;
    };

    RightSidePanel rightPanel;
    LeftSidePanel leftPanel;
    MainPanel mainPanel;

    std::unique_ptr<juce::TextButton> presButton;
    int currentPres = 1;
    int loadCurrentPres = 1;
    juce::XmlElement xmlState{ juce::XmlElement("MYPLUGINSETTINGS") };
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FrictionModelAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FrictionModelAudioProcessorEditor)
};
