#pragma once
#include <JuceHeader.h>

class AudioFragmenterAudioProcessor final : public juce::AudioProcessor
{
public:
    AudioFragmenterAudioProcessor();
    ~AudioFragmenterAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout&) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "AudioFragmenter"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    juce::AudioProcessorValueTreeState parameters;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    int getActiveFragmentSamples() const noexcept { return activeFragmentSamples; }
    bool getActiveFadeEnabled() const noexcept { return latchedFadeEnabled; }

private:
    static constexpr int maxRecentSlices = 8;
    struct FragmentInfo { int64_t start = 0; int length = 0; };
    uint32_t nextRandom() noexcept;
    float getWetEnvelopeGain(int position, int length) const noexcept;

    double currentSampleRate = 44100.0;
    int maxBlockSize = 0, activeFragmentSamples = 1;
    int latchedRecentSlices = 4;
    bool latchedFadeEnabled = true;
    int64_t sampleCursor = 0, fragmentStart = 0;
    int recentRead = 0, historyCount = 0, historyWrite = 0;
    int previousSelection = -1;
    int64_t previousSelectionStart = -1;
    FragmentInfo history[maxRecentSlices];
    uint32_t randomState = 0;
    juce::AudioBuffer<float> ring;
    int ringCapacity = 0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioFragmenterAudioProcessor)
};

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter();
