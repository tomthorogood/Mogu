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
    uint8_t propertyFlags; //!<\sa Enums::SignalTypes::Properties
    uint8_t actionBlocking;
    uint8_t type;
    bool link_is_internal;

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
    GooVariables baseVariables;

    /*!\brief Signal emitted when the style is changed. */
    Wt::Signal <> __style_changed;



    Enums::WidgetTypes::WidgetTypes typeFlags;
    
    /*!\brief Parses the stylesheet and the node, validating content
     * templates and setting the proper flags.
     */
    void setContentVariables ();

    /*!\brief Performs actions based on the typeFlags that are set,
     * logically deducing how best to implement the content.
     */
    void createContent ();

    /*!\brief Parses the variables and creates content according to
     * the rules for creating a link.
     */
    void do_if_is_link();

    /*!\brief Parses the variables and creates content according to
     * the rules of creating a static image.
     */
    void do_if_is_image();

    /*!\brief Parses the values in the events node and binds
     * these events to user activity.
     */
    void do_if_has_events();

    /*!\brief Iterates through the list of children and adds them to the
     * current widget.
     */
    void do_if_has_children();

    /*!\brief Registers the widget as a named widget. */
    void do_if_is_named();


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

    virtual std::string __TEST__();

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
    Moldable* child (int index);

    /*!\brief Returns the number of moldable children this widget contains.
     *
     * @return The number of moldable children this widget contains.
     */
    int countMoldableChildren();

    /*!\brief Retrieves a node property from the database.
     * @param key The key that would have been parsed from the database.
     * @return The value attached to the key.
     */
    std::string getMappedValue(std::string key);

    Wt::Signal<>& styleChanged();

    Redis::strvector* getNodeList();

    bool allowsAction(Enums::SignalActions::SignalAction action);

    bool isNamed();

    void requestRemoval(Moldable* child);

    const uint8_t& getType() const;

};

} //namespace Goo
#endif /* MODDABLEGOO_H_ */
