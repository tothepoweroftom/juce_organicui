/*
  ==============================================================================

    AutomationEditor.h
    Created: 5 Feb 2017 2:58:27pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class AutomationEditor :
	public GenericControllableContainerEditor
{
public:
	AutomationEditor(Automation * automation, bool isRoot);
	~AutomationEditor();

	Automation * automation;
	ScopedPointer<AutomationUI> automationUI;

	void resizedInternalContent(Rectangle<int> &r) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AutomationEditor)
};



#pragma once