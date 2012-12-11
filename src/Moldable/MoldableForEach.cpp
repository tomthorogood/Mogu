/*
 * MoldableForEach.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#include "MoldableForEach.h"
#include <Redis/RedisCore.h>
#include <Mogu.h>




MoldableForEach::MoldableForEach(const std::string& node)
: MoldableContainer(node)
{
    mApp;
    const MoldableFactory& factory = app->getFactory();

    //Clear styling which should instead be applied to the children.
    setStyleClass(" ");

    //ForEach widgets must have a template
    app->redisExec(Mogu::Keep, "hget %s template", node.c_str());
    MoguNode n_template(redisReply_STRING);
    std::string s_template = n_template.addPrefix("templates");

    /* Get the data that will be used to populate the widgets
     * n = node
     * i = int
     * s = string
     * v = vector
     */
    app->redisExec(Mogu::Keep, "hget %s content", node.c_str());
    Parsers::MoguScript_Tokenizer t(redisReply_STRING);
    MoguNode n_data(t.next());
    std::string s_data = n_data.addPrefix("data");
    app->redisExec(Mogu::Keep, "llen %s", s_data.c_str());
    int i_data = redisReply_INT;
    std::vector <std::string> v_data;
    app->redisExec(Mogu::Keep, "lrange %s 0 %d", s_data.c_str(), i_data);
    Redis::toVector(app->reply(), v_data);

    for (size_t i = 0; i < (size_t) i_data; ++i)
    {
        // Create the right kind of widget based on the template.
        Moldable* moldable_child = factory.createMoldableWidget(s_template);
        std::string current_data = v_data[i];
        NodeValue v;
        app->interpreter().giveInput(current_data, v);
        moldable_child->setMoldableValue(v.getString());
        addWidget(moldable_child);
    }
}


