#include "../src/PluginProcessor.h"

class AudioFragmenterTests : public juce::UnitTest
{
public:
    AudioFragmenterTests() : UnitTest("AudioFragmenter MVP") {}
    void runTest() override
    {
        beginTest("Parameter definitions and defaults");
        AudioFragmenterAudioProcessor p;
        expect(p.parameters.getRawParameterValue("fragmentLengthMs")->load() == 222.0f);
        expectWithinAbsoluteError(p.parameters.getRawParameterValue("dryWet")->load(), 1.0f, 0.001f);
        expect(p.parameters.getParameter("fragmentLengthMs")->getNumSteps() > 0);

        beginTest("State round trip");
        p.parameters.getParameter("fragmentLengthMs")->setValueNotifyingHost(0.25f);
        p.parameters.getParameter("dryWet")->setValueNotifyingHost(0.37f);
        juce::MemoryBlock state; p.getStateInformation(state);
        AudioFragmenterAudioProcessor restored; restored.setStateInformation(state.getData(), int(state.getSize()));
        expectWithinAbsoluteError(restored.parameters.getRawParameterValue("fragmentLengthMs")->load(), 515.0f, 1.0f);
        expectWithinAbsoluteError(restored.parameters.getRawParameterValue("dryWet")->load(), 0.37f, 0.01f);

        beginTest("Dry/wet endpoints");
        p.prepareToPlay(1000.0, 3);
        juce::AudioBuffer<float> b(2, 3); b.clear(); b.setSample(0, 0, 0.5f); b.setSample(1, 0, -0.5f);
        juce::MidiBuffer midi;
        p.parameters.getParameter("dryWet")->setValueNotifyingHost(0.0f); p.processBlock(b, midi); expectWithinAbsoluteError(b.getSample(0, 0), 0.5f, 0.001f);
        p.parameters.getParameter("dryWet")->setValueNotifyingHost(1.0f); b.clear(); p.processBlock(b, midi); expectWithinAbsoluteError(b.getSample(0, 0), 0.0f, 0.001f);

        beginTest("Length changes apply at a fragment boundary");
        p.parameters.getParameter("fragmentLengthMs")->setValueNotifyingHost(0.0f); p.prepareToPlay(1000.0, 2);
        expectEquals(p.getActiveFragmentSamples(), 20);
        juce::AudioBuffer<float> block(2, 7); block.clear();
        p.parameters.getParameter("fragmentLengthMs")->setValueNotifyingHost(1.0f); p.processBlock(block, midi);
        expectEquals(p.getActiveFragmentSamples(), 20);
        juce::AudioBuffer<float> block2(2, 20); block2.clear(); p.processBlock(block2, midi);
        expectEquals(p.getActiveFragmentSamples(), 2000);
    }
};
static AudioFragmenterTests tests;
juce::UnitTestRunner runner;
int main() { runner.runAllTests(); return 0; }
