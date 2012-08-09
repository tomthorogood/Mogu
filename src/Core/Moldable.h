/*
 * ModdableGoo.h
 *
 *  Created on: Jun 4, 2012
 *      Author: tom#include "config.h"
 *
 */

#ifndef MODDABLEGOO_H_
#define MODDABLEGOO_H_

#include <declarations.h>
#include <Wt/WContainerWidget>
#include <Events/Bindery.h>
#include <Wt/WSignal> // Templated Type

namespace Goo
{

////////////////////////////////////////////////////////
/*!\brief Holds the information about the widget retrieved from the database
 * so that the widget doesn't actually build the information until it is
 * necessary.
 */
struct GooVariables
{
    Redis::strvector children;
    uint8_t flags; //!<\sa Enums::SignalTypes::Properties
    uint8_t actionBlocking;
    uint8_t type;

    std::string location;    //!< Used for external links only.
    std::string source;      //!< URI for image file, if applicable.
    std::string content;

    GooVariables();
};

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
    Redis::strvector nodes;

    Events::EventBindery* bindery;

    /*!\brief A container with pointers to child widgets that are also of
     * the Moldable class (or descendant classes). Necessary since the native
     * widget() method will return WWidgets.
     * \sa addGoo()
     */
    TurnLeft::Utils::HungryVector <Moldable*> children;

    /*!\brief A simple struct holding the values for the possible
     * content variables.
     * \sa GooVariables
     */
    GooVariables* baseVariables;

    /*!\brief Signal emitted when the style is changed. */
    Wt::Signal <> __style_changed;



    Enums::WidgetTypes::WidgetTypes typeFlags;

    /*!\brief Parses the values in the events node and binds
     * these events to user activity.
     */
    inline void Moldable::do_if_has_events()
    {
        bindery = new Events::EventBindery(this);
    }

public:
    /*!\brief The standard (and only!) constrcutor for a ModdableGoo instance.
     *
     * @param constructorNode The node in the database that contains the
     * information about this widget.
     * @param source_file The file contains the template information that
     * this widget uses.
     * @param parent (optional) The parent widget of this widget.
     */
    Moldable (
            const std::string& constructorNode,
            Wt::WContainerWidget* parent =0);

    virtual ~Moldable();

    /*!\brief Implementation of the virtual load() method of WContainerWidget,
     * which will be used to ensure that nothing actually gets created until
     * the widget is needed. In this fashion, upon instantiation, the widgets
     * attributes are parsed, but not loaded until necessary, which is the best
     * balance between performance and memory.
     */
    void load();

    /*!\brief Adds a child ModdableGoo widget and, if that widget is set to
     * propogate its destrution signal upwards, listens for the melt signal.
     * @param nodeName The node in the database where information about the child
     * widget is stored.
     */
    void addGoo (const std::string& nodeName);

    /*!\brief An implementation of the setStyleClass method which also
     * emits the new style as part of the styleChanged signal.
     * @param style
     */
    virtual void setStyleClass(const Wt::WString& styleClass);

    /*!\brief Retrieves a Moldable widget from this widget.
     *
     * @param index The widget to be retrieved.
     * @return The *MOLDABLE* widget within the index of moldable children.
     */
    inline Moldable* child (int index)
    {
    	return children.at(index);
    }

    /*!\brief Returns the number of moldable children this widget contains.
     *
     * @return The number of moldable children this widget contains.
     */
    inline int countMoldableChildren()
    {
    	return children.size();
    }

    /*!\brief Retrieves a node property from the database.
     * @param key The key that would have been parsed from the database.
     * @return The value attached to the key.
     */
    inline std::string getMappedValue(std::string key)
    {
        std::string nodeName = Helpers::getTemplateNode(this);
        Redis::command("hget", nodeName, key);
        return Redis::toString();
    }

    Wt::Signal<>& styleChanged()
	{
    	return __style_changed;
    }

    inline Redis::strvector* getNodeList()
    {
    	return &nodes;
    }

    inline bool allowsAction(Enums::SignalActions::SignalAction action)
    {
    	return !(
			(baseVariables->actionBlocking & action)
			|| (baseVariables->actionBlocking & Enums::SignalActions::BLOCK));
    }

    inline bool isNamed()
    {
		uint8_t named = Enums::SignalTypes::is_named;
		return baseVariables->flags & named;
    }

    inline void requestRemoval(Moldable* child)
    {
    	removeChild(child);
    }

    inline GooVariables* getProperties()
    {
    	return baseVariables;
    }

    inline const uint8_t& getType() const
    {
    	return baseVariables->type;
    }

};

} //namespace Goo
#endif /* MODDABLEGOO_H_ */
