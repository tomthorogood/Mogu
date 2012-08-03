/*
 * Moldable.cpp
 *
 *  Created on: Jun 4, 2012
 *      Author: tom
 */

#include <Core/Moldable.h>
#include <Core/Dynamic.h>
#include <Wt/WContainerWidget>
#include <Wt/WStackedWidget>
#include <Wt/WText>
#include <Wt/WImage>
#include <Wt/WAnchor>
#include <Wt/WLineEdit>

#include <Redis/RedisCore.h>
#include <Parsers/Parsers.h>
#include <Parsers/NodeValueParser.h>
#include <Parsers/StyleParser.h>
#include <Events/Bindery.h>
#include <Static.h>
#include <Mogu.h>
#include <hash.h>

// REMOVE WHEN IN PRODUCTION
#include <iostream>

namespace Goo
{
using std::string;
using namespace Enums::WidgetTypes;
using namespace Enums::BitMasks;

//REMOVE WHEN In PRODUCTION
using std::cout;
using std::endl;

GooVariables::GooVariables() : children()
{
    propertyFlags       =0;
    actionBlocking      =0;
    type				=0;
    link_is_internal    = false;

    content         = "";
    location        = "";
    source          = "";
}

Moldable::Moldable(
        const string& constructorNode,
        Wt::WContainerWidget* parent
)
: Wt::WContainerWidget (parent),
    children(),
    baseVariables(),
    __style_changed(this),
    typeFlags()

{
    nodes.add(constructorNode);
#ifdef TERM_ENABLED
    std::cout << constructorNode << std::endl;
#endif
    bindery =0;
    setContentVariables();
}

Moldable::~Moldable()
{
    if (bindery != 0)
        delete bindery;
}

Moldable* Moldable::child (int index)
{
    return children.at(index);
}

int Moldable::countMoldableChildren()
{
    children.trim();
    return children.size();
}

string Moldable::getMappedValue(string key)
{
    string nodeName = getNodeList()->at(0);
    Redis::command("hget", nodeName, key);
    return Redis::toString();
}

Wt::Signal<>& Moldable::styleChanged()
{
    return __style_changed;
}

void Moldable::setContentVariables()
{
    using namespace Enums::BitMasks;
    namespace Properties = Enums::SignalTypes;
    using namespace Parsers::StyleParser;

    typeFlags = Parsers::StyleParser::getWidgetType(this);
    //Todo why are we storing this value twice?
    baseVariables.type = typeFlags;

    /*Set the various property flags of this widget. */

    if (widgetHasAnimation(this))
    {
        baseVariables.propertyFlags |= Properties::has_animation;
    }
    if (widgetHasEvents(this))
    {
        baseVariables.propertyFlags |= Properties::has_events;
    }
    if (widgetBlocksActions(this))
    {
        baseVariables.propertyFlags |= Properties::blocks_actions;
    }
    if (widgetHasChildren(this))
    {
        baseVariables.propertyFlags |= Properties::has_children;
    }
    if (widgetIsNamed(this))
    {
    	baseVariables.propertyFlags |= Properties::is_named;
    	do_if_is_named();
    }
    if ( (typeFlags & stack) == stack)
    {
        baseVariables.propertyFlags |= Properties::is_stacked;
    }

    /* Then set the more specific variables. */

    if (widgetHasStyling(this))
    {
        Wt::WString styleClass(getWidgetStyleClass(this));
        setStyleClass(styleClass);
    }

    /* All non-container widgets should have some sort of text
     * content, even though it might not necessarily be a "block of text".
     */
    if ( (typeFlags & 0x3F /*Mask two HO bits */) > stack)
    {
        baseVariables.content = getWidgetText(this);
    }

    if (
    		(typeFlags & image) == image ||
    		((typeFlags & image_link) == image_link))
    {
        baseVariables.source = getWidgetImgSource(this);
    }
    if ( (typeFlags & Enums::WidgetTypes::link) == Enums::WidgetTypes::link ||
    		(typeFlags & image_link) == image_link)
    {
        baseVariables.location = getWidgetLinkLocation(this);
    }
}


void
Moldable::load()
{
    /* Don't create content until the widget is loaded and visible. */
    if (!loaded())
    {
        Wt::WContainerWidget::load();
        createContent();
        if (baseVariables.propertyFlags & Enums::SignalTypes::has_events)
        {
            do_if_has_events();
        }
    }
}

void Moldable::addGoo (const string& nodeName)
{
	Moldable* newGoo =0;
	if (Parsers::StyleParser::widgetIsDynamic(nodeName))
	{
		std::string token = AUTH_TOKEN;
		token = Hash::toHash(token);
		newGoo = new Dynamic(
				Application::encrypt(token),
				nodeName);
	}
	if (newGoo ==0)
	{
		newGoo = new Moldable(nodeName);
	}
    if (typeFlags == Enums::WidgetTypes::stack)
    {
        Wt::WStackedWidget* stack =
                (Wt::WStackedWidget*) widget(0);
        stack->addWidget(newGoo);
    }
    else
    {
        addWidget(newGoo);
    }
    /* Because the children vector receives a pointer to the newGoo widget,
     * it doesn't really matter whether or not it is contained directly as
     * a child of this widget, or as a stacked container child, because it
     * can be interacted with just the same from external widgets requesting
     * access to their content. Isn't that special?
     */
    children.add(newGoo);
}

void Moldable::setStyleClass(const Wt::WString& style)
{
    Wt::WContainerWidget::setStyleClass(style);
    __style_changed.emit();
}


void
Moldable::createContent()
{
    if (baseVariables.propertyFlags & Enums::SignalTypes::blocks_actions)
    {
        baseVariables.actionBlocking =
                Parsers::StyleParser::getActionBlock(this);
    }

    if ( (typeFlags & widget_usually_clicked) == widget_usually_clicked)
    {
        do_if_is_link();
    }
    else if ((typeFlags & image) == image)
    {
        do_if_is_image();
    }
    else
    {
        switch(typeFlags & 0x3F /* Mask the two HO bits */)
        {
        case text:{
            Wt::WText* _text = new Wt::WText(baseVariables.content);
            addWidget(_text);
            break;}

        case stack:{
            Wt::WStackedWidget* _stacked_container =
                    new Wt::WStackedWidget();

            if (baseVariables.propertyFlags &
                    Enums::SignalTypes::has_animation)
            {
                Wt::WAnimation::AnimationEffect effect =
                        Parsers::StyleParser::getWidgetAnimation(this);
                Wt::WAnimation transition(effect);
                _stacked_container->setTransitionAnimation(transition,true);
            }

            /*In order to prevent bizarre side effects, the hollow container
             * (this), must transfer its style class to the stacked container.
             */
            _stacked_container->setStyleClass(styleClass());
            setStyleClass("");
            addWidget(_stacked_container);
            break;}
        case input_text:{
        	Wt::WLineEdit* input_ = new Wt::WLineEdit(baseVariables.content);
        	addWidget(input_);
        	break;}
        }
    }
    if (baseVariables.propertyFlags & Enums::SignalTypes::has_children)
    {
        do_if_has_children();
    }
}

void Moldable::do_if_is_link()
{
    /* Deleted by Wt constructor */
    Wt::WAnchor* _anchor = new Wt::WAnchor(
            baseVariables.location,
            baseVariables.content
    );
    if (typeFlags == image_link)
    {
        Wt::WImage* _image = new Wt::WImage (
                baseVariables.source, baseVariables.content);
        _anchor->setImage(_image);
    }

    if (baseVariables.link_is_internal)
    {
        Wt::WString _path(baseVariables.location);
        Wt::WLink link;
        link.setInternalPath(_path);
        _anchor->setLink(link);
    }
    addWidget(_anchor);
}

void Moldable::do_if_is_named()
{
	using namespace Parsers::StyleParser;
	Mogu* app = Application::mogu();
	std::string my_name = getWidgetName(this);
	app->registerWidget(my_name, this);
}

void Moldable::do_if_has_events()
{
    bindery = new Events::EventBindery(this);
}

void Moldable::do_if_is_image()
{
    Wt::WImage* _image = new Wt::WImage(
            baseVariables.source, baseVariables.content);
    addWidget(_image);
}

void Moldable::do_if_has_children()
{
    Parsers::StyleParser::getWidgetChildren(this, baseVariables.children);
    int size = baseVariables.children.size();
    for (int i = 0; i < size; i++)
    {
        string kid_name = baseVariables.children.at(i);
        addGoo(kid_name);
    }
    baseVariables.children.trim();
}

bool Moldable::allowsAction(Enums::SignalActions::SignalAction action)
{
    /* If this action's bit is set in the block (1) and the incoming action
     * is also (1), return false -- we do not allow that action.
     */
    return !((baseVariables.actionBlocking & action) ||
            /* Or if the general BLOCK is in place, blocking all actions. */
            (baseVariables.actionBlocking & Enums::SignalActions::BLOCK));
}

bool Moldable::isNamed()
{
	uint8_t named = Enums::SignalTypes::is_named;
	return (baseVariables.propertyFlags & named) == named;
}

const uint8_t& Moldable::getType() const
{
	return baseVariables.type;
}

void Moldable::requestRemoval(Moldable* _child)
{
	removeChild(_child);
}

Redis::strvector* Moldable::getNodeList()
{
    return &nodes;
}

}//namespace Goo
