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
        expectEquals(int(p.parameters.getRawParameterValue("recentSlices")->load()), 4);
        expectEquals(p.parameters.getParameter("recentSlices")->getNumSteps(), 7);
        expect(p.parameters.getParameter("fragmentLengthMs")->getNumSteps() > 0);

        beginTest("State round trip");
        p.parameters.getParameter("fragmentLengthMs")->setValueNotifyingHost(0.25f);
        p.parameters.getParameter("dryWet")->setValueNotifyingHost(0.37f);
        juce::MemoryBlock state; p.getStateInformation(state);
        AudioFragmenterAudioProcessor restored; restored.setStateInformation(state.getData(), int(state.getSize()));
        expectWithinAbsoluteError(restored.parameters.getRawParameterValue("fragmentLengthMs")->load(), 515.0f, 1.0f);
        expectWithinAbsoluteError(restored.parameters.getRawParameterValue("dryWet")->load(), 0.37f, 0.01f);

        beginTest("Recent slices state round trip and old state fallback");
        p.parameters.getParameter("recentSlices")->setValueNotifyingHost(0.75f);
        juce::MemoryBlock recentState; p.getStateInformation(recentState);
        AudioFragmenterAudioProcessor recentRestored; recentRestored.setStateInformation(recentState.getData(), int(recentState.getSize()));
        expectEquals(int(recentRestored.parameters.getRawParameterValue("recentSlices")->load()), 6);
        auto oldTree = p.parameters.copyState(); oldTree.removeProperty("recentSlices", nullptr);
        AudioFragmenterAudioProcessor oldRestored;
        oldRestored.parameters.replaceState(oldTree);
        expectEquals(int(oldRestored.parameters.getRawParameterValue("recentSlices")->load()), 4);

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

        beginTest("Wet fragment has bounded half-cosine fades");
        AudioFragmenterAudioProcessor faded;
        faded.prepareToPlay(1000.0, 20);
        faded.parameters.getParameter("fragmentLengthMs")->setValueNotifyingHost(0.0f);
        faded.parameters.getParameter("dryWet")->setValueNotifyingHost(1.0f);
        juce::AudioBuffer<float> source(2, 20);
        source.clear(); for (int i = 0; i < source.getNumSamples(); ++i) { source.setSample(0, i, 1.0f); source.setSample(1, i, 1.0f); }
        faded.processBlock(source, midi);
        juce::AudioBuffer<float> playback(2, 20);
        playback.clear(); for (int i = 0; i < playback.getNumSamples(); ++i) { playback.setSample(0, i, 2.0f); playback.setSample(1, i, 2.0f); }
        faded.processBlock(playback, midi);
        expectWithinAbsoluteError(playback.getSample(0, 0), 0.0f, 0.001f);
        expectWithinAbsoluteError(playback.getSample(0, 1), 1.0f, 0.001f);
        expectWithinAbsoluteError(playback.getSample(0, 18), 1.0f, 0.001f);
        expectWithinAbsoluteError(playback.getSample(0, 19), 0.0f, 0.001f);

        beginTest("Short selected fragment fades out before the next boundary");
        AudioFragmenterAudioProcessor shortPlayback;
        shortPlayback.prepareToPlay(1000.0, 2000);
        shortPlayback.parameters.getParameter("fragmentLengthMs")->setValueNotifyingHost(0.0f);
        shortPlayback.parameters.getParameter("dryWet")->setValueNotifyingHost(1.0f);
        juce::AudioBuffer<float> shortSource(2, 20); shortSource.clear(); for (int i = 0; i < shortSource.getNumSamples(); ++i) { shortSource.setSample(0, i, 1.0f); shortSource.setSample(1, i, 1.0f); }
        shortPlayback.processBlock(shortSource, midi);
        shortPlayback.parameters.getParameter("fragmentLengthMs")->setValueNotifyingHost(1.0f);
        juce::AudioBuffer<float> longPlayback(2, 2000); longPlayback.clear(); for (int i = 0; i < longPlayback.getNumSamples(); ++i) { longPlayback.setSample(0, i, 2.0f); longPlayback.setSample(1, i, 2.0f); }
        shortPlayback.processBlock(longPlayback, midi);
        expectWithinAbsoluteError(longPlayback.getSample(0, 0), 0.0f, 0.001f);
        expectWithinAbsoluteError(longPlayback.getSample(0, 1), 1.0f, 0.001f);
        expectWithinAbsoluteError(longPlayback.getSample(0, 19), 0.0f, 0.001f);
        expectWithinAbsoluteError(longPlayback.getSample(0, 20), 0.0f, 0.001f);

        beginTest("Envelope processing is independent of host block size");
        auto render = [](int blockSize)
        {
            AudioFragmenterAudioProcessor processor;
            processor.prepareToPlay(1000.0, blockSize);
            processor.parameters.getParameter("fragmentLengthMs")->setValueNotifyingHost(0.0f);
            processor.parameters.getParameter("dryWet")->setValueNotifyingHost(1.0f);
            std::vector<float> output(80, 0.0f);
            juce::MidiBuffer midiBuffer;
            for (int offset = 0; offset < int(output.size()); offset += blockSize)
            {
                const int count = juce::jmin(blockSize, int(output.size()) - offset);
                juce::AudioBuffer<float> block(2, count); block.clear();
                const float value = float(1 + (offset / 20));
                for (int i = 0; i < count; ++i) { block.setSample(0, i, value); block.setSample(1, i, value); }
                processor.processBlock(block, midiBuffer);
                for (int i = 0; i < count; ++i) output[size_t(offset + i)] = block.getSample(0, i);
            }
            return output;
        };
        const auto blockOne = render(1), blockSeven = render(7);
        expectEquals(int(blockOne.size()), int(blockSeven.size()));
        for (size_t i = 0; i < blockOne.size(); ++i)
            expectWithinAbsoluteError(blockOne[i], blockSeven[i], 0.0001f);
    }
};
static AudioFragmenterTests tests;
juce::UnitTestRunner runner;
int main() { runner.runAllTests(); return 0; }
