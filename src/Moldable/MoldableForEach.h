/*
 * MoldableForEach.h
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#ifndef MOLDABLEFOREACH_H_
#define MOLDABLEFOREACH_H_

#include "MoldableContainer.h"

class MoldableForEach : public MoldableContainer
{
protected:
    virtual void __init__();
public:
    MoldableForEach (const std::string& node);
    inline virtual void reload()
    {
        clear();
        force_reload = true;
        Moldable::__init__();
        MoldableAbstractParent::__init__();
        __init__();
        load();
        force_reload = false;
    }

};


#endif /* MOLDABLEFOREACH_H_ */
