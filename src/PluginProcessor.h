#pragma once

#include <JuceHeader.h>

#include "CustomEditor.h"
#include "MidiPortsController.h"

class PluginProcessor : public AudioProcessor {
   public:
    PluginProcessor() : AudioProcessor(BusesProperties()), collector(), controller(&collector, *this) {}

    void prepareToPlay(double sampleRate, int) override { collector.reset(sampleRate); }
    void releaseResources() override {}

    void processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) override {
        buffer.clear();
        controller.processBlock(midiMessages, buffer.getNumSamples());
    }

    const String getName() const override { return ProjectInfo::projectName; }

    AudioProcessorEditor *createEditor() override { return new CustomEditor(*this, &controller); };

    bool hasEditor() const override { return true; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    bool isMidiEffect() const override { return true; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const String getProgramName(int) override { return {}; }
    void changeProgramName(int, const String &) override {}

    double getTailLengthSeconds() const override { return 0; }

    void getStateInformation(MemoryBlock &destData) override {
        auto xml = controller.createXml();
        copyXmlToBinary(*xml, destData);
    }

    void setStateInformation(const void *data, int sizeInBytes) override {
        auto xmlState = getXmlFromBinary(data, sizeInBytes);

        if (xmlState.get() != nullptr) controller.loadFromXml(*xmlState);
    }

   private:
    MidiPortsController controller;
    MidiMessageCollector collector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)
};
