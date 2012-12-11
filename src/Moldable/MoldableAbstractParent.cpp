/*
 * MoldableAbstractParent.cpp
 *
 *  Created on: Dec 11, 2012
 *      Author: tom
 */

#include "MoldableAbstractParent.h"
#include <Mogu.h>

MoldableAbstractParent::MoldableAbstractParent (const std::string& node)
: Moldable(node)
{
    mApp;
    std::string children = getNode() + ".children";
    app->redisExec(Mogu::Keep, "exists %s", children.c_str());
    has_children = redisReply_TRUE ? true : false;
}

void MoldableAbstractParent::load(){
    if (loaded()) return;
    Moldable::load();
    if (has_children) {
       mApp;
       const MoldableFactory& factory = app->getFactory();
       std::string n_children = __node.addPrefix("widgets") + ".children";
       app->redisExec(Mogu::Keep, "llen %s", n_children.c_str());
       size_t i_children = (size_t) redisReply_INT;
       for (size_t i = 0; i < i_children; ++i)
       {
           app->redisExec(Mogu::Keep, "lindex %s %d", n_children.c_str(), i);
           std::string s_child = redisReply_STRING;
           Moldable* m_child = factory.createMoldableWidget(s_child);
           __moldable_children.push_back(m_child);
           addWidget(m_child);
       }
   }
}

