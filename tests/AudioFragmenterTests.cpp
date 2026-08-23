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
        expectWithinAbsoluteError(p.parameters.getRawParameterValue("fadeEnabled")->load(), 1.0f, 0.001f);
        expectEquals(p.parameters.getParameter("recentSlices")->getNumSteps(), 7);
        expect(p.parameters.getParameter("fragmentLengthMs")->getNumSteps() > 0);

        beginTest("State round trip");
        p.parameters.getParameter("fragmentLengthMs")->setValueNotifyingHost(0.25f);
        p.parameters.getParameter("dryWet")->setValueNotifyingHost(0.37f);
        juce::MemoryBlock state; p.getStateInformation(state);
        AudioFragmenterAudioProcessor restored; restored.setStateInformation(state.getData(), int(state.getSize()));
        expectWithinAbsoluteError(restored.parameters.getRawParameterValue("fragmentLengthMs")->load(), 515.0f, 1.0f);
        expectWithinAbsoluteError(restored.parameters.getRawParameterValue("dryWet")->load(), 0.37f, 0.01f);

        beginTest("Fade state round trip and old state fallback");
        p.parameters.getParameter("fadeEnabled")->setValueNotifyingHost(0.0f);
        juce::MemoryBlock fadeState; p.getStateInformation(fadeState);
        AudioFragmenterAudioProcessor fadeRestored; fadeRestored.setStateInformation(fadeState.getData(), int(fadeState.getSize()));
        expectWithinAbsoluteError(fadeRestored.parameters.getRawParameterValue("fadeEnabled")->load(), 0.0f, 0.001f);
        auto oldFadeTree = p.parameters.copyState(); oldFadeTree.removeProperty("fadeEnabled", nullptr);
        AudioFragmenterAudioProcessor oldFadeRestored;
        oldFadeRestored.parameters.replaceState(oldFadeTree);
        expectWithinAbsoluteError(oldFadeRestored.parameters.getRawParameterValue("fadeEnabled")->load(), 1.0f, 0.001f);

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

        beginTest("Fade off preserves hard wet boundaries");
        AudioFragmenterAudioProcessor unfaded;
        unfaded.prepareToPlay(1000.0, 20);
        unfaded.parameters.getParameter("fragmentLengthMs")->setValueNotifyingHost(0.0f);
        unfaded.parameters.getParameter("dryWet")->setValueNotifyingHost(1.0f);
        unfaded.parameters.getParameter("fadeEnabled")->setValueNotifyingHost(0.0f);
        juce::AudioBuffer<float> unfadedSource(2, 20); unfadedSource.clear();
        for (int i = 0; i < 20; ++i) { unfadedSource.setSample(0, i, 1.0f); unfadedSource.setSample(1, i, 1.0f); }
        unfaded.processBlock(unfadedSource, midi);
        juce::AudioBuffer<float> unfadedPlayback(2, 20); unfadedPlayback.clear();
        unfaded.processBlock(unfadedPlayback, midi);
        expectWithinAbsoluteError(unfadedPlayback.getSample(0, 0), 1.0f, 0.001f);
        expectWithinAbsoluteError(unfadedPlayback.getSample(0, 19), 1.0f, 0.001f);

        beginTest("Fade changes apply at a fragment boundary");
        AudioFragmenterAudioProcessor latched;
        latched.parameters.getParameter("fragmentLengthMs")->setValueNotifyingHost(0.1f);
        latched.parameters.getParameter("dryWet")->setValueNotifyingHost(1.0f);
        latched.prepareToPlay(1000.0, 100);
        juce::AudioBuffer<float> latchedSource(2, 100); latchedSource.clear();
        for (int i = 0; i < 100; ++i) { latchedSource.setSample(0, i, 1.0f); latchedSource.setSample(1, i, 1.0f); }
        latched.processBlock(latchedSource, midi);
        expectEquals(latched.getActiveFragmentSamples(), 100);
        latched.parameters.getParameter("fadeEnabled")->setValueNotifyingHost(0.0f);
        juce::AudioBuffer<float> latchedPart(2, 1); latchedPart.clear();
        latched.processBlock(latchedPart, midi);
        expect(latched.getActiveFadeEnabled());
        juce::AudioBuffer<float> latchedRemainder(2, 99); latchedRemainder.clear();
        latched.processBlock(latchedRemainder, midi);
        expect(latched.getActiveFadeEnabled());
        juce::AudioBuffer<float> latchedNext(2, 1); latchedNext.clear();
        latched.processBlock(latchedNext, midi);
        expect(!latched.getActiveFadeEnabled());

        beginTest("Fade remains safe at multiple samplerates");
        for (const double sampleRate : { 8000.0, 44100.0, 48000.0, 96000.0 })
        {
            AudioFragmenterAudioProcessor samplerated;
            samplerated.prepareToPlay(sampleRate, 64);
            samplerated.parameters.getParameter("fragmentLengthMs")->setValueNotifyingHost(0.0f);
            juce::AudioBuffer<float> input(2, 20); input.clear();
            samplerated.processBlock(input, midi);
            juce::AudioBuffer<float> output(2, 20); output.clear();
            samplerated.processBlock(output, midi);
            expect(std::isfinite(output.getSample(0, 0)));
            expect(std::isfinite(output.getSample(0, 19)));
        }

        beginTest("Fade duration follows one eighth and twenty millisecond cap");
        auto checkFadeLength = [this, &midi](float normalizedLength, int lengthSamples, int fadeSamples)
        {
            AudioFragmenterAudioProcessor processor;
            processor.parameters.getParameter("fragmentLengthMs")->setValueNotifyingHost(normalizedLength);
            processor.parameters.getParameter("dryWet")->setValueNotifyingHost(1.0f);
            processor.prepareToPlay(1000.0, lengthSamples);
            juce::AudioBuffer<float> source(2, lengthSamples); source.clear();
            for (int i = 0; i < lengthSamples; ++i) { source.setSample(0, i, 1.0f); source.setSample(1, i, 1.0f); }
            processor.processBlock(source, midi);
            juce::AudioBuffer<float> playback(2, lengthSamples); playback.clear();
            processor.processBlock(playback, midi);
            expectWithinAbsoluteError(playback.getSample(0, 0), 0.0f, 0.001f);
            expectWithinAbsoluteError(playback.getSample(0, fadeSamples - 1), 1.0f, 0.001f);
            expectWithinAbsoluteError(playback.getSample(0, lengthSamples - fadeSamples), 1.0f, 0.001f);
            expectWithinAbsoluteError(playback.getSample(0, lengthSamples - 1), 0.0f, 0.001f);
        };
        checkFadeLength(80.0f / 1980.0f, 100, 12);
        checkFadeLength(140.0f / 1980.0f, 160, 20);

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
