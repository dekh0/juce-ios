#include "MainComponent.h"


//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);
    
    addAndMakeVisible(slider1);
    slider1.addListener(this);
    slider1.setSkewFactor(0.3f);
    slider1.setValue(0);
    slider1.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    
    addAndMakeVisible(slider2);
    slider2.addListener(this);
    slider2.setSkewFactor(0.3f);
    slider2.setValue(0);
    slider2.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    
    addAndMakeVisible(slider3);
    slider3.addListener(this);
    slider3.setSkewFactor(0.3f);
    slider3.setValue(0);
    slider3.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    
    addAndMakeVisible(slider4);
    slider4.addListener(this);
    slider4.setValue(0);
    slider4.setRange(3, 200);
    slider4.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    
    
    
    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

static int numOfHarmonics = 6;
bool processFlag = false;


void MainComponent::sliderValueChanged(Slider *slider)
{
    if(slider == &slider4){
        while(!processFlag)
        {
            numOfHarmonics = roundToInt(slider4.getValue());
            osc1.initialise({[&] (float x)
                {
                    double sum = 0;
                    for (int i = 1; i < numOfHarmonics; i++)
                        sum += sin ((2 * i - 1) * x) / ((2 * i - 1));
                    return 4.0f / MathConstants<float>::pi * sum;
                }});
            break;
        }
    }
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlockExpected;
    spec.sampleRate = sampleRate;
    spec.numChannels = 2;
    
    
    osc1={[&] (float x)
        {
            double sum = 0;
            for (int i = 1; i < numOfHarmonics; i++)
                sum += sin ((2 * i - 1) * x) / ((2 * i - 1));
            return 4.0f / MathConstants<float>::pi * sum;
        }};
    
    osc1.prepare(spec);
    osc2.prepare(spec);
    osc3.prepare(spec);
}


void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    processFlag = true;
    juce::dsp::AudioBlock<float> block(*bufferToFill.buffer);
    osc1.process(juce::dsp::ProcessContextReplacing<float>(block));
    osc2.process(juce::dsp::ProcessContextReplacing<float>(block));
    osc3.process(juce::dsp::ProcessContextReplacing<float>(block));
    osc1.setFrequency(slider1.getValue()*2000);
    osc2.setFrequency(slider2.getValue()*2000);
    osc3.setFrequency(slider3.getValue()*2000);
    processFlag = false;
}


void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.
    
    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    
    // You can add your drawing code here!
}

void MainComponent::resized()
{
    slider1.setBounds(0, 50, getWidth(), 50);
    slider2.setBounds(0, 100, getWidth(), 50);
    slider3.setBounds(0, 150, getWidth(), 50);
    slider4.setBounds(0, 200, getWidth(), 50);
}
