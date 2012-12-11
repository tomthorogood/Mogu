/*
 * MoldableForEachUser.cpp
 *
 *  Created on: Dec 11, 2012
 *      Author: tom
 */

#include "MoldableForEachUser.h"
#include <Mogu.h>
#include <Types/GroupManager.h>

MoldableForEachUser::MoldableForEachUser(const std::string& node)
: MoldableContainer(node)
{
    mApp;
    std::vector <std::string> memberSessions;
    std::vector <std::string> values;
    const std::string& mogu_id = app->getUserManager().getMoguID();
    GroupManager groupManager(app->getGroup());
    const MoldableFactory& factory = app->getFactory();


    // Get the content we'll be parsing for each user:
    app->redisExec(Mogu::Keep, "hget %s content", node.c_str());
    std::string contents = redisReply_STRING;

    // Convert reference into string object ON PURPOSE! We need the original.
    std::string session_reset = app->sessionID();

    // Get a list of all accessible user ids
    std::vector<std::string>& accessible = groupManager.getMembership(
        groupManager.getMemberRank(mogu_id), 0);
    size_t num_children = accessible.size();

    // Populate a vector of all of the session end points
    // we need to use to start examining user data
    for (size_t i = 0; i < num_children; ++i) {
        app->redisExec(Mogu::Keep, "hget %s %s", __NODE_SESSION_LOOKUP,
            accessible[i].c_str());
        memberSessions.push_back(redisReply_STRING);
    }

    // Temporarily change the session id into the user's and retrieve the
    // requested data. Then, change the session id back.
    for (size_t i = 0; i < num_children; ++i) {
        app->setSessionID(memberSessions[i]);
        NodeValue v;
        app->interpreter().giveInput(contents, v);
        values.push_back(v.getString());
        app->setSessionID(session_reset);
    }

    // Now that we have all the right content, we must create the widgets that
    // utilize this content. This is similar to the MoldableForEach algorithm,
    // but does not require going back to the database anymore. We're done with
    // the database for now.

    setStyleClass(" ");

    app->redisExec(Mogu::Keep, "hget %s template", node.c_str());
    MoguNode n_template = redisReply_STRING;
    std::string s_template = n_template.addPrefix("templates");

    for (size_t i = 0; i < num_children; ++i)
    {
        Moldable* moldable_child = factory.createMoldableWidget(s_template);
        moldable_child->setMoldableValue(values[i]);
        addWidget(moldable_child);
    }

}

