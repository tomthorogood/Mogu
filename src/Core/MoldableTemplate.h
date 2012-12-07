/*
 * MoldableTemplate.h
 *
 *  Created on: Sep 27, 2012
 *      Author: tom
 */

#ifndef MOLDABLETEMPLATE_H_
#define MOLDABLETEMPLATE_H_

#include <declarations.h>

namespace Goo {

struct MoldableTemplate
{
    uint16_t flags;
    uint16_t num_children;
    masked actionBlocking;
    mapped type;
    std::string location;
    std::string source;
    std::string content;
    std::string style;
    std::string name;
    std::string node;
    bool deleteable;
    uint16_t num_connected_widgets;

    MoldableTemplate()
    {
        flags = 0;
        actionBlocking = 0;
        type = 0;
        num_children = 0;
        num_connected_widgets = 0;
        location = EMPTY;
        source = EMPTY;
        content = EMPTY;
        style = EMPTY;
        name = EMPTY;
        deleteable = false;
    }

    inline void disconnect()
    {
        if (--num_connected_widgets <= 0) deleteable = true;
    }
};

}    //namespace Goo
#endif /* MOLDABLETEMPLATE_H_ */
