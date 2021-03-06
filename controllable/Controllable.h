/*
 ==============================================================================

 Controllable.h
 Created: 8 Mar 2016 1:08:56pm
 Author:  bkupe

 ==============================================================================
 */

#pragma once

class ControllableUI;
class ControllableContainer;
class DashboardItem;

class Controllable :
	public Inspectable,
	public ScriptTarget,
	public DashboardItemTarget
{
public:
	enum Type { //Add type here if creating new type of Controllable
		CUSTOM,
		TRIGGER,
		FLOAT,
		INT,
		BOOL,
		STRING,
		ENUM,
		POINT2D,
		POINT3D,
		TARGET,
		COLOR
	};

	
	Controllable(const Type &type, const String &niceName, const String &description, bool enabled = true);
	virtual ~Controllable();

	Type type;
	String niceName;
	String shortName;
	String description;
	String argumentsDescription;

	//For storing arbitraty data
	var customData;
	bool saveCustomData;
	
	//
	bool enabled;
	bool canBeDisabledByUser;
	bool descriptionIsEditable;
	bool hasCustomShortName;
	bool isControllableExposed;
	bool isControllableFeedbackOnly;
	bool hideInOutliner;
	bool includeInScriptObject;

	String controlAddress;

	//save & load
	bool isSavable;
	bool saveValueOnly;
	bool isLoadingData;

	//user control
	bool isCustomizableByUser;
	bool isRemovableByUser;

	bool replaceSlashesInShortName;

	WeakReference<ControllableContainer> parentContainer;

	UndoableAction * setUndoableNiceName(const String &_niceName, bool onlyReturnAction = false);
	void setNiceName(const String &_niceName);
	void setCustomShortName(const String &_shortName);
	void setAutoShortName();

	virtual void setEnabled(bool value, bool silentSet = false, bool force = false);
	virtual void setControllableFeedbackOnly(bool value);

	void setParentContainer(ControllableContainer * container);

	template<class T>
	T * getParentAs() { 
		if (parentContainer == nullptr || parentContainer.wasObjectDeleted()) return nullptr;
		return dynamic_cast<T *>(parentContainer.get());
	}
	void updateControlAddress();

	void remove(bool addToUndo = false); // called from external to make this object ask for remove

	virtual void updateLiveScriptObjectInternal(DynamicObject * parent = nullptr) override;

	virtual var getJSONData(ControllableContainer * relativeTo = nullptr);
	virtual var getJSONDataInternal() { return var(new DynamicObject()); } // to be overriden
	virtual void loadJSONData(var data);
	virtual void loadJSONDataInternal(var data) {} //to be overriden

	String getControlAddress(ControllableContainer * relativeTo = nullptr);

	// used for generating editor
	virtual ControllableUI * createDefaultUI(Controllable * targetControllable = nullptr) = 0;

	virtual DashboardItem * createDashboardItem() override;

	virtual void setParamAndValueFromScript(String param, var value);

	static var setValueFromScript(const juce::var::NativeFunctionArgs& a);
	static var checkIsParameterFromScript(const juce::var::NativeFunctionArgs& a);
	static var getParentFromScript(const juce::var::NativeFunctionArgs& a);
	static var setNameFromScript(const juce::var::NativeFunctionArgs& a);
	static var setParamFromScript(const juce::var::NativeFunctionArgs& a);

	virtual String getWarningTargetName() const override;
	
	virtual InspectableEditor * getEditor(bool /*isRootEditor*/) override;
	virtual String getTypeString() const { jassert(false); return ""; } //should be overriden


	class  Listener
	{
	public:
		/** Destructor. */
		virtual ~Listener() {}
		virtual void controllableStateChanged(Controllable *) {}
		virtual void controllableFeedbackStateChanged(Controllable *) {}
		virtual void controllableControlAddressChanged(Controllable *) {}
		virtual void controllableNameChanged(Controllable *) {}
		virtual void controllableRemoved(Controllable *) {}
		virtual void askForRemoveControllable(Controllable *, bool /*addToUndo*/ = false) {}
	};

	ListenerList<Listener> listeners;
	void addControllableListener(Listener* newListener) { listeners.add(newListener); }
	void removeControllableListener(Listener* listener) { listeners.remove(listener); }

																		// ASYNC
	class  ControllableEvent
	{
	public:
		enum Type { STATE_CHANGED, FEEDBACK_STATE_CHANGED, CONTROLADDRESS_CHANGED, NAME_CHANGED, CONTROLLABLE_REMOVED };

		ControllableEvent(Type t, Controllable * c) : type(t),controllable(c) {}

		Type type;
		Controllable * controllable;
	};

	QueuedNotifier<ControllableEvent> queuedNotifier;
	typedef QueuedNotifier<ControllableEvent>::Listener AsyncListener;


	void addAsyncControllableListener(AsyncListener* newListener) { queuedNotifier.addListener(newListener); }
	void addAsyncCoalescedControllableListener(AsyncListener* newListener) { queuedNotifier.addAsyncCoalescedListener(newListener); }
	void removeAsyncControllableListener(AsyncListener* listener) { queuedNotifier.removeListener(listener); }


private:
	WeakReference<Controllable>::Master masterReference;
	friend class WeakReference<Controllable>;


public:
	class ControllableAction :
		public UndoableAction
	{
	public:
		ControllableAction(Controllable * c) :
			controllableRef(c)
		{
			controlAddress = c->getControlAddress();
		}

		WeakReference<Controllable> controllableRef;
		String controlAddress;

		Controllable * getControllable();
	};

	class ControllableChangeNameAction :
		public ControllableAction
	{
	public:
		ControllableChangeNameAction(Controllable * c, String oldName, String newName) :
			ControllableAction(c),
			oldName(oldName),
			newName(newName)
		{
		}

		String oldName;
		String newName;

		bool perform() override;
		bool undo() override;
	};

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Controllable)
};
