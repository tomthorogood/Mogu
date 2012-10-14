/*
 * ModdableGoo.h
 *
 *  Created on: Jun 4, 2012
 *      Author: tom#include "config.h"
 *
 */

/*!\file MoldableGoo.h
 * \brief The base class for Mogucentric widgets.
 *
 * Goo::Moldable is the base widget for Mogu. Moldable widgets are inherently
 * lazy, and will do very little on their own, instead subjecting themselves
 * to other forces that , well, mold them. As such, almost everything in a
 * Moldable widget is public and variable.
 *
 * This also contains the declaration for the GooVariables struct, which holds
 * a Moldable widget's properties and can be passed around separately.
 */

#ifndef MODDABLEGOO_H_
#define MODDABLEGOO_H_

#include <declarations.h>
#include <Wt/WContainerWidget>
#include <Events/Bindery.h>
#include <Redis/RedisCore.h>
#include <Wt/WLineEdit>
#include <Core/MoldableTemplate.h>
#include <Wt/WApplication>
#include <Wt/WSignal> // Templated Type

//!\brief The namespace in which the Mogu Core is located.
namespace Goo
{
/////////////////////////////////////////////////////////////
/* Begin Moldable Class */
/////////////////////////////////////////////////////////////

/*! \brief The semi-base class for widgets that automatically add their own
 * children. Upon instantiation, ModdableGoo widgets connect to their
 * database node and determine what children they have, adding them in
 * dynamically. Each of the children will also be a moddable goo widget
 * (or derived from one).
 */

class Moldable : public Wt::WContainerWidget
{
private:
#ifdef DEBUG
	const char* __NODE_NAME; //easier to see in the Eclipse debug screen.
#endif
	MoldableTemplate* __tmpl;
	std::string __node; //!< This widget's location in the database

	Mogu* __app;

    Events::EventBindery* bindery; //!< The widget's personal event bindery
    bool __reload; //!< Allows the widget to be reloaded

    /*!\brief A container with pointers to child widgets that are also of
     * the Moldable class (or descendant classes). Necessary since the native
     * widget() method will return WWidgets.
     * \sa addGoo()
     */
    std::vector <Moldable*> mchildren;

    /*!\brief Signal emitted when the style is changed. */
    Wt::Signal <> __style_changed;

    /*!\brief Signal emitted if the widget contains a WStackedWidget and the
     * stack index is manipulated by Mogu.
     */
    Wt::Signal <> __index_changed;

    /*!\brief Signal emitted if the widget is given a test which it fails. */
    Wt::Signal <> __failed_test;

    /*!\brief Signal emitted if the widget is given a test which it passes. */
    Wt::Signal <> __succeeded_test;

    Wt::Signal <bool> __hidden_changed;


    /*!\brief Parses the values in the events node and binds
     * these events to user activity.
     */
    inline void do_if_has_events()
    {
        bindery = new Events::EventBindery(this);
    }

    std::string (*__value_callback)(Moldable&);
    void (*__setvalue_callback)(Moldable&, std::string&);

public:
    Moldable();
    Moldable(MoldableTemplate*);

    virtual ~Moldable();

    /*!\brief Implementation of the virtual load() method of WContainerWidget,
     * which will be used to ensure that nothing actually gets created until
     * the widget is needed. In this fashion, upon instantiation, the widgets
     * attributes are parsed, but not loaded until necessary, which is the best
     * balance between performance and memory.
     */
    virtual void load();
    virtual void setHidden(
    		bool hidden, const Wt::WAnimation& animation= Wt::WAnimation());


    /*!\brief An implementation of the setStyleClass method which also
     * emits the new style as part of the styleChanged signal.
     * @param style
     */
    inline virtual void setStyleClass(const Wt::WString& style)
    {
        Wt::WContainerWidget::setStyleClass(style);
        __style_changed.emit();
    }

    inline void addMoldableChild(Moldable* __child)
    {
    	mchildren.push_back(__child);
    }

    /*!\brief Retrieves a Moldable widget from this widget.
     *
     * @param index The widget to be retrieved.
     * @return The *MOLDABLE* widget within the index of moldable children.
     */
    inline Moldable* child (int index)
    {
    	return mchildren.at(index);
    }

    /*!\brief Returns the number of moldable children this widget contains.
     *
     * @return The number of moldable children this widget contains.
     */
    inline int countMoldableChildren()
    {
    	return mchildren.size();
    }

    /*!\brief Provides an accessor for the styleChanged signal. */
    Wt::Signal<>& styleChanged()
	{
    	return __style_changed;
    }

    /*!\brief Use instead of getNodeList to retrieve this widget's location
     * in the database.
     * @return
     */
    inline std::string getNode()
    {
    	return __tmpl->node;
    }

    /*!\brief Alerts the caller to whether or not this widget allows a
     * certain action.
     * @param action The action requested
     * @return Whether or not it is a valid action for this widget
     */
    inline bool allowsAction(Enums::SignalActions::SignalAction action)
    {
    	return !(
			(__tmpl->actionBlocking & action)
			|| (__tmpl->actionBlocking == Enums::SignalActions::BLOCK));
    }

    /*!\brief Allows another widget to remove a specific child from this
     * widget's family tree. The other widget must already have the child's
     * pointer handy
     * @param child The pointer to the Moldable* widget that is to be
     * deleted.
     */
    inline void requestRemoval(Moldable* child)
    {
    	removeChild(child);
    }

    /*!\brief Provides an accessor to the widget's properties */
    inline MoldableTemplate* getProperties()
    {
    	return __tmpl;
    }

    /*\brief Returns this widget's type. */
    inline const mapped& getType() const
    {
    	return __tmpl->type;
    }

    /*!\brief If this widget is provided a validator, its first child is
     * assigned said validator.
     * @param validator
     */
    inline void validate (Wt::WValidator* validator)
    {
    	Wt::WLineEdit* input = (Wt::WLineEdit*) widget(0);
    	input->setValidator(validator);
    }

    /*!\brief Provides an accessor to the fail signal. */
    inline Wt::Signal<>& fail()
	{
    	return __failed_test;
	}

    /*!\brief Provides an accessor to the index changed signal. */
    inline Wt::Signal<>& stackIndexChanged()
	{
    	return __index_changed;
	}

    /*!\brief Provides an accessor for the success signal. */
    inline Wt::Signal<>& succeed()
	{
    	return __succeeded_test;
	}

    inline Wt::Signal<bool>& hiddenChanged()
	{
    	return __hidden_changed;
	}

    /*!\brief Validates this widget's first child, which will emit either the
     * fail() or succeed() signals.
     */
    void __validate();

    /*!\brief Allows this widget to be reloaded, returning a reference to itself
     * for easy method chanining
     * @return
     */
    inline Moldable& setReload()
    {
    	__reload = true;
    	return *this;
    }

    /*!\brief Returns whether or not this widget may be reloaded */
    inline bool reload() { return __reload; }

    inline Mogu* app() { return __app;}

    inline void setValueCallback(std::string(*cb)(Moldable&))
    {
    	__value_callback = cb;
    }

    inline void setSetValueCallback(void(*cb)(Moldable&, std::string&))
    {
    	__setvalue_callback = cb;
    }

    inline std::string valueCallback()
    {
    	return __value_callback(*this);
    }

    inline void setValueCallback(std::string& newtxt)
    {
    	__setvalue_callback(*this, newtxt);
    }


    void getState(	Enums::WidgetTypes::States state, Nodes::NodeValue& val);

};

} //namespace Goo
#endif /* MODDABLEGOO_H_ */
