/*
  ==============================================================================

    FileParameter.h
    Created: 9 Mar 2016 12:29:30am
    Author:  bkupe

  ==============================================================================
*/
#pragma once

class FileParameter : 
	public StringParameter,
	public EngineListener
{
public:
    FileParameter(const String &niceName, const String &description, const String &initialValue, bool enabled=true);
	virtual ~FileParameter();

	String customBasePath;

	String absolutePath;
	String fileTypeFilter;

	bool forceAbsolutePath;

    // need to override this function because var Strings comparison  is based on pointer (we need full string comp)
    virtual void setValueInternal(var&) override;
	bool forceRelativePath;
	
	void setForceRelativePath(bool value);

	bool isRelativePath(const String &p);
	String getAbsolutePath() const;
	File getBasePath() const;
	File getFile();

	var getJSONDataInternal() override;
	void loadJSONDataInternal(var data) override;

	void fileSaved(bool savedAs) override;


	static StringParameter * create() { return new FileParameter("New FileParameter", "",""); }
	virtual String getTypeString() const override { return getTypeStringStatic(); }
	static String getTypeStringStatic() { return "File"; }

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FileParameter)
};
