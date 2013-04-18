/*
 * MoldableAbstractParent.cpp
 *
 *  Created on: Dec 11, 2012
 *      Author: tom
 */

#include "MoldableAbstractParent.h"
#include <Redis/ContextQuery.h>
#include <Mogu.h>

MoldableAbstractParent::MoldableAbstractParent (const std::string& node)
: Moldable(node)
{
    __init__();
}

void MoldableAbstractParent::__init__()
{
    Moldable::__init__();
    const char* c_node = __node.c_str();
    Redis::ContextQuery db(Prefix::widgets);
    CreateQuery(db,
        new Redis::Query("llen widgets.%s.children", c_node));
    num_children = db.yieldResponse <int>();
}

void MoldableAbstractParent::load(){
    if (loaded() && !force_reload) return;
    Moldable::load();
    if (num_children > 0) {
#ifdef DEBUG
        std::cout << getNode() << " " << "has children!" << std::endl;
#endif
        mApp;
        const MoldableFactory& factory = app->getFactory();
        Redis::ContextQuery db(Prefix::widgets);
        std::shared_ptr<Redis::Query> get_children;
        get_children = std::make_shared <Redis::Query>(
            new Redis::Query(
                "lrange widgets.%s.children 0 %d",getNode().c_str(),num_children)
        );

        db.appendQuery(get_children);
        std::vector <std::string>& children_v =
            db.yieldResponse <std::vector <std::string>&>();



        for (int i = 0; i < num_children; ++i)
        {
           std::string s_child = children[i];
           Moldable* m_child = factory.createMoldableWidget(s_child);
           __moldable_children.push_back(m_child);
           addWidget(m_child);
       }
   }
}

