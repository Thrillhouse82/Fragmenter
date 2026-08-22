#include "PluginEditor.h"

AudioFragmenterAudioProcessorEditor::AudioFragmenterAudioProcessorEditor(AudioFragmenterAudioProcessor& p) : AudioProcessorEditor(p), processor(p)
{
    setSize(320, 180);
    for (auto* s : { &length, &wet }) { s->setSliderStyle(juce::Slider::LinearHorizontal); s->setTextBoxStyle(juce::Slider::TextBoxRight, false, 70, 22); addAndMakeVisible(s); }
    lengthLabel.setText("Fragment Length", juce::dontSendNotification); wetLabel.setText("Dry/Wet", juce::dontSendNotification);
    addAndMakeVisible(lengthLabel); addAndMakeVisible(wetLabel);
    lengthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "fragmentLengthMs", length);
    wetAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "dryWet", wet);
}
void AudioFragmenterAudioProcessorEditor::paint(juce::Graphics& g) { g.fillAll(juce::Colours::darkgrey); g.setColour(juce::Colours::white); g.setFont(18.0f); g.drawText("AudioFragmenter", 16, 12, 280, 24, juce::Justification::left); }
void AudioFragmenterAudioProcessorEditor::resized() { lengthLabel.setBounds(16, 55, 120, 24); length.setBounds(140, 55, 160, 24); wetLabel.setBounds(16, 105, 120, 24); wet.setBounds(140, 105, 160, 24); }
