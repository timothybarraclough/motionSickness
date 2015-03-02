/*
 *  LabelSlider.h
 *  Motion Sickness
 *
 *  Created by Timothy J on 5/07/13.
 *  Copyright 2013 Victoria University. All rights reserved.
 *
 */


#include "../JuceLibraryCode/JuceHeader.h"

#ifndef MotionSickness_LabelSlider_h
#define MotionSickness_LabelSlider_h

class LabelSlider : public Slider
{

public:
	
    LabelSlider(String name) : Slider(name)
    {
        setPopupMenuEnabled(true);
        setPopupDisplayEnabled(true, 0);
        setSliderStyle(SliderStyle::LinearBar);
        setColour(0, Colours::green);
        setTextBoxStyle(TextEntryBoxPosition::NoTextBox, true, 0, 0);
        
        _name = name;
    }
    
    void paint(Graphics& g)
    {
        Slider::paint(g);
        g.setColour(Colours::black);
        g.drawFittedText(_name,
                         0, 0,
                         getWidth(), getHeight(),
                         Justification(Justification::Flags::left & Justification::Flags::verticallyCentred),
                         1);
    }
    
private:
    String _name;
};


#endif
