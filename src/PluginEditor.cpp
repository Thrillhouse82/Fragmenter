#include "PluginEditor.h"

namespace
{
juce::StringArray syncDivisionLabels()
{
    return { "1/1", "1/1 D", "1/1 T", "1/2", "1/2 D", "1/2 T",
             "1/4", "1/4 D", "1/4 T", "1/8", "1/8 D", "1/8 T",
             "1/16", "1/16 D", "1/16 T" };
}
}

AudioFragmenterAudioProcessorEditor::AudioFragmenterAudioProcessorEditor(AudioFragmenterAudioProcessor& p) : AudioProcessorEditor(p), processor(p)
{
    setSize(620, 280);
    for (auto* s : { &length, &syncDivision, &wet, &recent })
    {
        s->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 86, 22);
        addAndMakeVisible(s);
    }
    length.textFromValueFunction = [](double v) { return juce::String(juce::roundToInt(v)) + " ms"; };
    syncDivision.setRange(0.0, 14.0, 1.0);
    syncDivision.textFromValueFunction = [](double v)
    {
        const auto labels = syncDivisionLabels();
        return labels[juce::jlimit(0, labels.size() - 1, juce::roundToInt(v))];
    };
    wet.textFromValueFunction = [](double v) { return juce::String(juce::roundToInt(v * 100.0)) + "%"; };
    recent.textFromValueFunction = [](double v) { return juce::String(juce::roundToInt(v)); };
    lengthMode.addItem("Milliseconds", 1);
    lengthMode.addItem("Tempo Sync", 2);
    lengthModeLabel.setText("Length Mode", juce::dontSendNotification);
    addAndMakeVisible(lengthModeLabel);
    addAndMakeVisible(lengthMode);
    lengthMode.onChange = [this] { updateLengthModeUi(); };
    fade.setClickingTogglesState(true);
    fade.onStateChange = [this]
    {
        fade.setButtonText(fade.getToggleState() ? "Fade: On" : "Fade: Off");
    };
    fade.setButtonText("Fade: On");
    addAndMakeVisible(fade);
    lengthLabel.setText("Fragment Length", juce::dontSendNotification);
    syncDivisionLabel.setText("Tempo Division", juce::dontSendNotification);
    wetLabel.setText("Dry/Wet", juce::dontSendNotification);
    recentLabel.setText("Recent Slices N", juce::dontSendNotification);
    addAndMakeVisible(lengthLabel); addAndMakeVisible(syncDivisionLabel); addAndMakeVisible(wetLabel); addAndMakeVisible(recentLabel);
    lengthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "fragmentLengthMs", length);
    syncDivisionAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "syncDivision", syncDivision);
    wetAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "dryWet", wet);
    recentAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "recentSlices", recent);
    lengthModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(processor.parameters, "sliceLengthMode", lengthMode);
    fadeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processor.parameters, "fadeEnabled", fade);
    updateLengthModeUi();
}
void AudioFragmenterAudioProcessorEditor::paint(juce::Graphics& g) { g.fillAll(juce::Colours::darkgrey); g.setColour(juce::Colours::white); g.setFont(18.0f); g.drawText("AudioFragmenter", 16, 12, 280, 24, juce::Justification::left); }
void AudioFragmenterAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().withTrimmedTop(82).reduced(12);
    const int column = area.getWidth() / 5;
    lengthLabel.setBounds(column * 0, area.getY(), column, 24); length.setBounds(column * 0, area.getY() + 24, column, area.getHeight() - 24);
    syncDivisionLabel.setBounds(column * 1, area.getY(), column, 24); syncDivision.setBounds(column * 1, area.getY() + 24, column, area.getHeight() - 24);
    wetLabel.setBounds(column * 2, area.getY(), column, 24); wet.setBounds(column * 2, area.getY() + 24, column, area.getHeight() - 24);
    recentLabel.setBounds(column * 3, area.getY(), column, 24); recent.setBounds(column * 3, area.getY() + 24, column, area.getHeight() - 24);
    fade.setBounds(column * 4, area.getY() + 24, column, area.getHeight() - 24);
    lengthModeLabel.setBounds(12, 42, 110, 24);
    lengthMode.setBounds(126, 42, 180, 24);
}

void AudioFragmenterAudioProcessorEditor::updateLengthModeUi()
{
    const bool tempoSync = lengthMode.getSelectedId() == 2;
    length.setEnabled(!tempoSync);
    syncDivision.setEnabled(tempoSync);
}
