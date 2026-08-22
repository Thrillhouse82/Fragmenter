#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace { constexpr float minLength = 20.0f, maxLength = 2000.0f, defaultLength = 222.0f; }

AudioFragmenterAudioProcessor::AudioFragmenterAudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout AudioFragmenterAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
    p.push_back(std::make_unique<juce::AudioParameterFloat>("fragmentLengthMs", "Fragment Length",
        juce::NormalisableRange<float>(minLength, maxLength, 1.0f), defaultLength,
        juce::AudioParameterFloatAttributes().withLabel(" ms")));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("dryWet", "Dry/Wet",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 1.0f,
        juce::AudioParameterFloatAttributes()
            .withLabel("%")
            .withCategory(juce::AudioProcessorParameter::genericParameter)
            .withStringFromValueFunction([](float v, int) { return juce::String(juce::roundToInt(v * 100.0f)) + "%"; })));
    return { p.begin(), p.end() };
}

void AudioFragmenterAudioProcessor::prepareToPlay(double sr, int block)
{
    currentSampleRate = sr; maxBlockSize = juce::jmax(1, block);
    const int maxSamples = juce::jmax(1, juce::roundToInt(maxLength * float(sr) / 1000.0f));
    ringCapacity = 2 * maxSamples + maxBlockSize;
    ring.setSize(2, ringCapacity, false, true, true);
    ring.clear();
    activeFragmentSamples = juce::jmax(1, juce::roundToInt(parameters.getRawParameterValue("fragmentLengthMs")->load() * float(sr) / 1000.0f));
    sampleCursor = fragmentStart = recentStart = 0; recentLength = recentRead = 0; hasRecent = false;
}

void AudioFragmenterAudioProcessor::releaseResources() {}

bool AudioFragmenterAudioProcessor::isBusesLayoutSupported(const BusesLayout& l) const
{
    return l.getMainInputChannelSet() == juce::AudioChannelSet::stereo()
        && l.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void AudioFragmenterAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    const float wet = juce::jlimit(0.0f, 1.0f, parameters.getRawParameterValue("dryWet")->load());
    const float dry = 1.0f - wet;
    const int n = buffer.getNumSamples();
    for (int i = 0; i < n; ++i)
    {
        if (sampleCursor > fragmentStart && sampleCursor - fragmentStart >= activeFragmentSamples)
        {
            recentStart = fragmentStart; recentLength = activeFragmentSamples; recentRead = 0; hasRecent = true;
            fragmentStart = sampleCursor;
            const float requested = parameters.getRawParameterValue("fragmentLengthMs")->load();
            activeFragmentSamples = juce::jlimit(1, ringCapacity - maxBlockSize,
                juce::roundToInt(requested * float(currentSampleRate) / 1000.0f));
        }
        const float inL = buffer.getSample(0, i), inR = buffer.getNumChannels() > 1 ? buffer.getSample(1, i) : inL;
        ring.setSample(0, int(sampleCursor % ringCapacity), inL);
        ring.setSample(1, int(sampleCursor % ringCapacity), inR);
        float fragL = 0.0f, fragR = 0.0f;
        if (hasRecent && recentRead < recentLength)
        {
            const int pos = int((recentStart + recentRead++) % ringCapacity);
            fragL = ring.getSample(0, pos); fragR = ring.getSample(1, pos);
        }
        buffer.setSample(0, i, dry * inL + wet * fragL);
        if (buffer.getNumChannels() > 1) buffer.setSample(1, i, dry * inR + wet * fragR);
        ++sampleCursor;
    }
}

void AudioFragmenterAudioProcessor::getStateInformation(juce::MemoryBlock& dest)
{
    if (auto xml = parameters.copyState().createXml()) copyXmlToBinary(*xml, dest);
}

void AudioFragmenterAudioProcessor::setStateInformation(const void* data, int size)
{
    if (auto xml = getXmlFromBinary(data, size)) if (xml->hasTagName(parameters.state.getType())) parameters.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessorEditor* AudioFragmenterAudioProcessor::createEditor() { return new AudioFragmenterAudioProcessorEditor(*this); }
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new AudioFragmenterAudioProcessor(); }
