#include "EnumParameter.h"
/*
  ==============================================================================

    EnumParameter.cpp
    Created: 29 Sep 2016 5:34:59pm
    Author:  bkupe

  ==============================================================================
*/


EnumParameter::EnumParameter(const String & niceName, const String &description, bool enabled) :
	Parameter(Type::ENUM, niceName, description, "" ,var(),var(), enabled)
{
	lockManualControlMode = true;

	scriptObject.setMethod("getData", EnumParameter::getValueDataFromScript);
	scriptObject.setMethod("addOption", EnumParameter::addOptionFromScript);
}

EnumParameter * EnumParameter::addOption(String key, var data, bool selectIfFirstOption)
{
	enumValues.add(new EnumValue(key, data));
	if (enumValues.size() == 1 && selectIfFirstOption)
	{
		defaultValue = key;
		setValue(key, true, false,false);
	}

	enumListeners.call(&Listener::enumOptionAdded, this, key);
	updateArgDescription();
	return this;
}

void EnumParameter::removeOption(String key)
{
	enumValues.remove(getIndexForKey(key));
	enumListeners.call(&Listener::enumOptionRemoved, this, key);
	updateArgDescription();
}

void EnumParameter::clearOptions()
{
	StringArray keysToRemove;
	for (auto &ev : enumValues) keysToRemove.add(ev->key);
	for (auto &k : keysToRemove) removeOption(k);
}

void EnumParameter::updateArgDescription()
{
	argumentsDescription = "";
	for (int i = 0; i < enumValues.size(); i++)
	{
		argumentsDescription += enumValues[i]->key;
		if(i < enumValues.size()-1) argumentsDescription += " | ";
	}
}

var EnumParameter::getValue()
{
	return getValueData();
}

var EnumParameter::getValueData() {
	EnumValue * ev = getEntryForKey(value.toString());
	if (ev == nullptr) return var();
	return ev->value;
}

String EnumParameter::getValueKey() {
	return value.toString();
}

int EnumParameter::getIndexForKey(StringRef key)
{
	int numValues = enumValues.size();
	for (int i = 0; i < numValues; i++) if (enumValues[i]->key == key) return i;
	return -1;
}

EnumParameter::EnumValue * EnumParameter::getEntryForKey(StringRef key)
{
	int index = getIndexForKey(key);
	if (index == -1) return nullptr;
	return enumValues[index];
}

StringArray EnumParameter::getAllKeys()
{
	StringArray result;
	for(auto &ev: enumValues) result.add(ev->key);
	return result;
}

void EnumParameter::setValueWithData(var data)
{
	for (auto &ev : enumValues)
	{
		if (ev->value == data)
		{
			setValueWithKey(ev->key);
			break;
		}
	}
}

void EnumParameter::setValueWithKey(String key)
{
	setValue(key);
}

void EnumParameter::setNext(bool loop, bool addToUndo)
{

	int targetIndex = getIndexForKey(value.toString()) + 1;
	if (targetIndex >= enumValues.size())
	{
		if (loop) targetIndex = 0;
		else return;
	}

	String newValue = enumValues[targetIndex]->key;

	if (addToUndo) setUndoableValue(value, newValue);
	else setValueWithKey(newValue);
}

bool EnumParameter::checkValueIsTheSame(var oldValue, var newValue)
{
	return oldValue.toString() == newValue.toString();
}

var EnumParameter::getValueDataFromScript(const juce::var::NativeFunctionArgs & a)
{
	WeakReference<Parameter> c = getObjectFromJS<Parameter>(a);
	if (c == nullptr || c.wasObjectDeleted()) return var();
	EnumParameter * ep = dynamic_cast<EnumParameter *>(c.get());
	return ep->getValueData();
}

var EnumParameter::addOptionFromScript(const juce::var::NativeFunctionArgs& a)
{
	WeakReference<Parameter> c = getObjectFromJS<Parameter>(a);
	if (c == nullptr || c.wasObjectDeleted()) return var();
	EnumParameter* ep = dynamic_cast<EnumParameter*>(c.get());
	
	if (a.numArguments < 2)
	{
		NLOGWARNING("Script", "EnumParameter addOption should at least have 2 arguments");
		return var();
	}

	ep->addOption(a.arguments[0].toString(), a.arguments[1]);

	return var();
}

EnumParameterUI * EnumParameter::createUI(EnumParameter * target)
{
	if (target == nullptr) target = this;
	return new EnumParameterUI(target);
}

ControllableUI * EnumParameter::createDefaultUI(Controllable * targetControllable) {
	return createUI(dynamic_cast<EnumParameter *>(targetControllable));
}
