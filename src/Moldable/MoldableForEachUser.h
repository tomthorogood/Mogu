/*
 * MoldableForEachUser.h
 *
 *  Created on: Dec 11, 2012
 *      Author: tom
 */

#ifndef MOLDABLEFOREACHUSER_H_
#define MOLDABLEFOREACHUSER_H_

#include "MoldableContainer.h"

class MoldableForEachUser : public MoldableContainer
{
protected:
    virtual void __init__();
public:
    MoldableForEachUser(const std::string& node);
    inline virtual void reload()
    {
        clear();
        Moldable::__init__();
        MoldableAbstractParent::__init__();
        __init__();
        force_reload = true;
        load();
        force_reload = false;
    }
};



#endif /* MOLDABLEFOREACHUSER_H_ */
