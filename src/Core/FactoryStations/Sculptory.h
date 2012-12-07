/*
 * Sculptory.h
 *
 *  Created on: Sep 28, 2012
 *      Author: tom
 */

#ifndef SCULPTORY_H_
#define SCULPTORY_H_

#include <declarations.h>
#include <Core/MoldableTemplate.h>
#include <Core/MoldableFactory.h>
#include <Core/FactoryStations/PropertyCallbacks.h>
#include <Redis/RedisCore.h>
#include <Wt/WString>
#include <Wt/WWidget>
#include <Static.h>
#include <Parsers/StyleParser.h>
#include <Types/ListNodeGenerator.h>
#include <Core/FactoryStations/ForEach.h>
#include <Wt/WContainerWidget>
#include <Validators/Validators.h>
#include <Wt/WAnchor>
#include <Wt/WImage>
#include <Wt/WText>
#include <Core/Moldable.h>

namespace Goo {
namespace MoldableFactory {
namespace Sculptory {

using namespace Parsers::StyleParser;
using namespace Application;
using namespace Enums::WidgetTypes;

void __sculpt_stack(
    MoldableTemplate* __tmpl, Moldable* m);
void addChildren(
    MoldableTemplate* __tmpl, Wt::WContainerWidget* c, Moldable* m = 0);
void __sculpt_text(
    MoldableTemplate* __tmpl, Moldable *m);

inline void setStyle(
    const std::string& _style, Wt::WWidget* m)
{
    mApp;
    NodeValue v;
    app->interpreter().giveInput(_style, v);
    Wt::WString style(v.getString());
    m->setStyleClass(style);
}

inline void getNumChildren(
    MoldableTemplate* __tmpl)
{
    std::string node;
    if (__tmpl->type == foreach) {
        node = __tmpl->content;
        /*Currently, a foreach loop's content field
         * will have '@data.source.blah.@'
         */
        node = node.substr(1, node.length() - 2);
    }
    else {
        node = __tmpl->node + ".children";
    }
    const char* cnode = node.c_str();
    mApp;
    app->redisCommand("llen %s", cnode);
    __tmpl->num_children = Redis::getInt(app->reply());
}

inline void __sculpt_container(
    MoldableTemplate* __tmpl, Moldable *m)
{
    if (__tmpl->style != EMPTY) setStyle(__tmpl->style, m);
    if (__tmpl->num_children > 0) {
        addChildren(__tmpl, m);
    }
    m->setValueCallback(&Callbacks::__value_container);
}

inline void __sculpt_foreach(
    MoldableTemplate* __tmpl, Moldable*m)
{
    ForEachData fed(*__tmpl, *m);
}

inline void __sculpt_memberlist(
    MoldableTemplate* __tmpl, Moldable* m)
{
    ForEachGroupMember fegm(*__tmpl, *m);
}

inline void __sculpt_link(
    MoldableTemplate* __tmpl, Moldable* m)
{
    mApp;
    if (__tmpl->style != EMPTY) setStyle(__tmpl->style, m);
    NodeValue v;
    app->interpreter().giveInput(__tmpl->content, v, m);
    Wt::WAnchor* anchor = new Wt::WAnchor(__tmpl->location, v.getString());
    anchor->setTarget(Wt::TargetNewWindow);
    m->addWidget(anchor);
}
inline void __sculpt_image(
    MoldableTemplate* __tmpl, Moldable* m)
{
    mApp;
    if (__tmpl->style != EMPTY) setStyle(__tmpl->style, m);
    NodeValue v;
    app->interpreter().giveInput(__tmpl->content, v, m);
    Wt::WImage* img = new Wt::WImage(__tmpl->source, v.getString());
    m->addWidget(img);

    if (widgetHasBooleanProperty(m->getNode(), "scaled"))
        img->addStyleClass("mogu_SCALED_IMAGE");
}

inline void __sculpt_image_link(
    MoldableTemplate* __tmpl, Moldable* m)
{
    mApp;
    if (__tmpl->style != EMPTY) setStyle(__tmpl->style, m);
    NodeValue v;
    app->interpreter().giveInput(__tmpl->content, v, m);
    Wt::WImage* img = new Wt::WImage(__tmpl->source, __tmpl->content);
    Wt::WAnchor* a = new Wt::WAnchor(__tmpl->location, __tmpl->content);
    a->setImage(img);
    a->setTarget(Wt::TargetNewWindow);
    m->addWidget(a);
}

inline void __sculpt_input_txt(
    MoldableTemplate* __tmpl, Moldable* m)
{
    mApp;
    if (__tmpl->style != EMPTY) setStyle(__tmpl->style, m);
    NodeValue v;
    app->interpreter().giveInput(__tmpl->content, v, m);
    Wt::WLineEdit* in = new Wt::WLineEdit();
    Wt::WString empty(v.getString());
    in->setEmptyText(empty);
    if (__tmpl->flags & is_validated) {
        std::string val_name = getWidgetProperty(__tmpl->node, "validator");
        Wt::WValidator* v = Validators::createValidator(val_name);
        in->setValidator(v);
        in->keyWentUp().connect(m, &Moldable::__validate);
    }
    m->addWidget(in);
    m->setValueCallback(&Callbacks::__value_wlineedit);
    m->setSetValueCallback(&Callbacks::__setvalue_wlineedit);

}
inline void __sculpt_password_field(
    MoldableTemplate* __tmpl, Moldable* m)
{
    __sculpt_input_txt(__tmpl, m);
    Wt::WLineEdit* p = (Wt::WLineEdit*) m->widget(0);
    p->setEchoMode(Wt::WLineEdit::Password);
}

inline void __sculpt_textarea(
    MoldableTemplate* __tmpl, Moldable* m)
{
}
inline void __sculpt_dropdown(
    MoldableTemplate* __tmpl, Moldable* m)
{
}
inline void __sculpt_radio(
    MoldableTemplate* __tmpl, Moldable* m)
{
}
inline void __sculpt_checkbox(
    MoldableTemplate* __tmpl, Moldable* m)
{
}
inline void __sculpt_multi_select(
    MoldableTemplate* __tmpl, Moldable* m)
{
}
inline void __sculpt_button(
    MoldableTemplate* __tmpl, Moldable* m)
{
}

}    //namespace Sculptory
}    //namespace MoldableFactory
}    //namespace Goo

#endif /* SCULPTORY_H_ */
