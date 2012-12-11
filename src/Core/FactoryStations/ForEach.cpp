/*
 * ForEach.cpp
 *
 *  Created on: Nov 20, 2012
 *      Author: tom
 */

#include <Core/FactoryStations/ForEach.h>
#include <Parsers/MoguScript_Tokenizer.h>
#include <Core/MoldableFactory.h>
#include <Core/FactoryStations/PropertyCallbacks.h>
#include <Parsers/StyleParser.h>

namespace Goo {
namespace MoldableFactory {
namespace Sculptory {

using namespace Enums;

AbstractForEach::AbstractForEach(
    MoldableTemplate& tmpl, Moldable& parent)
    : __values(), __tmpl(tmpl)
{
    mApp;
    this->app = app;
    receiver = &parent;
    if (__tmpl.flags & WidgetTypes::is_stacked) {
        receiver = (Wt::WContainerWidget*) parent.widget(0);
    }

}

void AbstractForEach::execute(
    Moldable& parent)
{
    using namespace Parsers::StyleParser;

    getNumChildren();
    populateValues();
    mapped inner_type = (mapped) getWidgetType(
        "templates." + getWidgetProperty(__tmpl.node, "template"));
    MoldableTemplate tplcpy(__tmpl);
    tplcpy.num_children = 0;
    tplcpy.type = inner_type;

    for (int i = 0; i < __tmpl.num_children; i++) {
        MoldableTemplate* cpy = new MoldableTemplate(tplcpy);
        cpy->content = __values[i];
        receiver->addWidget(sculpt(cpy));
    }
    parent.setValueCallback(&Callbacks::__value_container);
}

ForEachData::ForEachData(
    MoldableTemplate& tmpl, Moldable& parent)
    : AbstractForEach(tmpl, parent)
{
    execute(parent);
}

void ForEachData::getNumChildren()
{
    Parsers::MoguScript_Tokenizer tokenizer(__tmpl.node);
    std::string node = tokenizer.next();
    app->redisExec(Mogu::Keep, "llen %s", node.c_str());
    __tmpl.num_children = (size_t) redisReply_INT;
}

void ForEachData::populateValues()
{
    mApp;
    Parsers::MoguScript_Tokenizer tokenizer(__tmpl.node);
    std::string node = tokenizer.next();

    for (size_t i = 0; i < __tmpl.num_children; ++i) {
        app->redisExec(Mogu::Keep, "lindex %s %d", node.c_str(), i);
        __values.push_back(redisReply_STRING);
    }
}

ForEachGroupMember::ForEachGroupMember(
    MoldableTemplate& tmpl, Moldable& parent)
    : AbstractForEach(tmpl, parent), memberSessions(), groupName(
        app->getGroup()), groupManager(groupName)
{
    execute(parent);
}

void ForEachGroupMember::getNumChildren()
{
    const std::string& mogu_id = app->getUserManager().getMoguID();
    GroupManager groupManager;
    std::vector<std::string>& accessible = groupManager.getMembership(
        groupManager.getMemberRank(mogu_id), 0);
    __tmpl.num_children = accessible.size();
    for (size_t i = 0; i < __tmpl.num_children; ++i) {
        app->redisExec(Mogu::Keep, "hget %s %s", __NODE_SESSION_LOOKUP,
            accessible[i].c_str());
        memberSessions.push_back(redisReply_STRING);
    }
}

void ForEachGroupMember::populateValues()
{
    std::string session_reset = app->sessionID();
    for (size_t i = 0; i < __tmpl.num_children; ++i) {
        app->setSessionID(memberSessions[i]);
        NodeValue v;
        app->interpreter().giveInput(__tmpl.content, v);
        __values.push_back(v.getString());
        app->setSessionID(session_reset);
    }
}

}    //namespace Sculptory
}    //namespace MoldableFactory
}    //namespace Goo
