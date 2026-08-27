#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
constexpr float minLength = 20.0f, maxLength = 2000.0f, defaultLength = 222.0f;
constexpr int defaultSyncDivision = 6;

juce::StringArray getSyncDivisionLabels()
{
    return { "1/1", "1/1 D", "1/1 T", "1/2", "1/2 D", "1/2 T",
             "1/4", "1/4 D", "1/4 T", "1/8", "1/8 D", "1/8 T",
             "1/16", "1/16 D", "1/16 T" };
}
}

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
    p.push_back(std::make_unique<juce::AudioParameterInt>("recentSlices", "Recent Slices N", 1, 8, 4));
    p.push_back(std::make_unique<juce::AudioParameterBool>("fadeEnabled", "Fade", true));
    p.push_back(std::make_unique<juce::AudioParameterChoice>("sliceLengthMode", "Slice Length Mode",
        juce::StringArray { "Milliseconds", "Tempo Sync" }, 0));
    p.push_back(std::make_unique<juce::AudioParameterChoice>("syncDivision", "Tempo Division",
        getSyncDivisionLabels(), defaultSyncDivision));
    return { p.begin(), p.end() };
}

void AudioFragmenterAudioProcessor::prepareToPlay(double sr, int block)
{
    currentSampleRate = sr; maxBlockSize = juce::jmax(1, block);
    const int maxSamples = juce::jmax(1, juce::roundToInt(maxLength * float(sr) / 1000.0f));
    ringCapacity = (maxRecentSlices + 1) * maxSamples + maxBlockSize;
    ring.setSize(2, ringCapacity, false, true, true);
    ring.clear();
    latchedLengthMode = juce::jlimit(0, 1,
        juce::roundToInt(parameters.getRawParameterValue("sliceLengthMode")->load()));
    latchedSyncDivision = juce::jlimit(0, 14,
        juce::roundToInt(parameters.getRawParameterValue("syncDivision")->load()));
    lastValidBpm = pendingBpm = latchedBpm = 120.0;
    activeFragmentSamples = calculateFragmentSamples(latchedLengthMode, latchedSyncDivision, latchedBpm);
    sampleCursor = fragmentStart = 0;
    recentRead = historyCount = historyWrite = 0;
    previousSelection = -1;
    previousSelectionStart = -1;
    latchedRecentSlices = juce::jlimit(1, maxRecentSlices,
        juce::roundToInt(parameters.getRawParameterValue("recentSlices")->load()));
    latchedFadeEnabled = parameters.getRawParameterValue("fadeEnabled")->load() >= 0.5f;
    randomState = 0x13579bdfu;
    for (auto& entry : history) entry = {};
}

void AudioFragmenterAudioProcessor::releaseResources() {}

bool AudioFragmenterAudioProcessor::isBusesLayoutSupported(const BusesLayout& l) const
{
    return l.getMainInputChannelSet() == juce::AudioChannelSet::stereo()
        && l.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void AudioFragmenterAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    pendingBpm = lastValidBpm;
    if (auto* playHead = getPlayHead())
        if (auto position = playHead->getPosition())
            if (auto bpm = position->getBpm())
                if (std::isfinite(*bpm) && *bpm > 0.0)
                    pendingBpm = lastValidBpm = *bpm;

    const float wet = juce::jlimit(0.0f, 1.0f, parameters.getRawParameterValue("dryWet")->load());
    const float dry = 1.0f - wet;
    const int n = buffer.getNumSamples();
    for (int i = 0; i < n; ++i)
    {
        if (sampleCursor > fragmentStart && sampleCursor - fragmentStart >= activeFragmentSamples)
        {
            history[historyWrite] = { fragmentStart, activeFragmentSamples };
            historyWrite = (historyWrite + 1) % maxRecentSlices;
            historyCount = juce::jmin(maxRecentSlices, historyCount + 1);
            fragmentStart = sampleCursor;
            latchedLengthMode = juce::jlimit(0, 1,
                juce::roundToInt(parameters.getRawParameterValue("sliceLengthMode")->load()));
            latchedSyncDivision = juce::jlimit(0, 14,
                juce::roundToInt(parameters.getRawParameterValue("syncDivision")->load()));
            latchedBpm = pendingBpm;
            activeFragmentSamples = calculateFragmentSamples(latchedLengthMode, latchedSyncDivision, latchedBpm);
            latchedRecentSlices = juce::jlimit(1, maxRecentSlices,
                juce::roundToInt(parameters.getRawParameterValue("recentSlices")->load()));
            latchedFadeEnabled = parameters.getRawParameterValue("fadeEnabled")->load() >= 0.5f;

            const int candidateCount = juce::jmin(historyCount, latchedRecentSlices);
            int selected = -1;
            if (candidateCount == 1)
                selected = (historyWrite - 1 + maxRecentSlices) % maxRecentSlices;
            else if (candidateCount > 1)
            {
                int available = candidateCount;
                for (int offset = 0; offset < candidateCount; ++offset)
                {
                    const int index = (historyWrite - 1 - offset + maxRecentSlices * 2) % maxRecentSlices;
                    if (previousSelectionStart >= 0 && history[index].start == previousSelectionStart)
                        --available;
                }
                int choice = int(nextRandom() % uint32_t(juce::jmax(1, available)));
                for (int offset = 0; offset < candidateCount; ++offset)
                {
                    const int index = (historyWrite - 1 - offset + maxRecentSlices * 2) % maxRecentSlices;
                    if (previousSelectionStart >= 0 && history[index].start == previousSelectionStart) continue;
                    if (choice-- == 0) { selected = index; break; }
                }
            }
            if (selected >= 0)
            {
                previousSelection = selected;
                previousSelectionStart = history[selected].start;
                recentRead = 0;
            }
        }
        const float inL = buffer.getSample(0, i), inR = buffer.getNumChannels() > 1 ? buffer.getSample(1, i) : inL;
        ring.setSample(0, int(sampleCursor % ringCapacity), inL);
        ring.setSample(1, int(sampleCursor % ringCapacity), inR);
        float fragL = 0.0f, fragR = 0.0f;
        if (previousSelection >= 0 && recentRead < history[previousSelection].length)
        {
            const int position = recentRead++;
            const int pos = int((history[previousSelection].start + position) % ringCapacity);
            const float envelope = latchedFadeEnabled
                ? getWetEnvelopeGain(position, history[previousSelection].length)
                : 1.0f;
            fragL = envelope * ring.getSample(0, pos); fragR = envelope * ring.getSample(1, pos);
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
    if (auto xml = getXmlFromBinary(data, size))
        if (xml->hasTagName(parameters.state.getType()))
        {
            auto state = juce::ValueTree::fromXml(*xml);
            if (!state.hasProperty("recentSlices")) state.setProperty("recentSlices", 4, nullptr);
            if (!state.hasProperty("fadeEnabled")) state.setProperty("fadeEnabled", true, nullptr);
            if (!state.hasProperty("sliceLengthMode")) state.setProperty("sliceLengthMode", 0, nullptr);
            if (!state.hasProperty("syncDivision")) state.setProperty("syncDivision", defaultSyncDivision, nullptr);
            parameters.replaceState(state);
        }
}

int AudioFragmenterAudioProcessor::calculateFragmentSamples(int mode, int division, double bpm) const noexcept
{
    if (mode == 0)
    {
        const float requested = parameters.getRawParameterValue("fragmentLengthMs")->load();
        return juce::jlimit(1, ringCapacity - maxBlockSize,
            juce::roundToInt(requested * float(currentSampleRate) / 1000.0f));
    }

    const double safeBpm = (std::isfinite(bpm) && bpm > 0.0) ? bpm : 120.0;
    const double seconds = (60.0 / safeBpm) * double(getSyncBeatFactor(division));
    return juce::jlimit(1, ringCapacity - maxBlockSize,
        juce::roundToInt(seconds * currentSampleRate));
}

float AudioFragmenterAudioProcessor::getSyncBeatFactor(int division) noexcept
{
    static constexpr float factors[] = { 4.0f, 6.0f, 8.0f / 3.0f,
                                          2.0f, 3.0f, 4.0f / 3.0f,
                                          1.0f, 1.5f, 2.0f / 3.0f,
                                          0.5f, 0.75f, 1.0f / 3.0f,
                                          0.25f, 0.375f, 1.0f / 6.0f };
    return factors[juce::jlimit(0, 14, division)];
}

uint32_t AudioFragmenterAudioProcessor::nextRandom() noexcept
{
    randomState ^= randomState << 13;
    randomState ^= randomState >> 17;
    randomState ^= randomState << 5;
    return randomState;
}

float AudioFragmenterAudioProcessor::getWetEnvelopeGain(int position, int length) const noexcept
{
    const int nominalFadeSamples = juce::jmax(0, juce::roundToInt(0.020 * currentSampleRate));
    const int fadeSamples = juce::jmin(nominalFadeSamples, length / 8);
    if (fadeSamples <= 1 || length <= 1)
        return 1.0f;

    const auto pi = juce::MathConstants<float>::pi;
    if (position < fadeSamples)
    {
        const float x = float(position) / float(fadeSamples - 1);
        return 0.5f - 0.5f * std::cos(pi * x);
    }

    if (position >= length - fadeSamples)
    {
        const float x = float(position - (length - fadeSamples)) / float(fadeSamples - 1);
        return 0.5f + 0.5f * std::cos(pi * x);
    }

    return 1.0f;
}

juce::AudioProcessorEditor* AudioFragmenterAudioProcessor::createEditor() { return new AudioFragmenterAudioProcessorEditor(*this); }
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new AudioFragmenterAudioProcessor(); }
