/*
 * MoldableLink.h
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#ifndef MOLDABLELINK_H_
#define MOLDABLELINK_H_

#include "Moldable.h"
#include <Wt/WAnchor>

class MoldableLink : public Moldable
{
protected:
    Wt::WAnchor* __link;
    virtual void __init__() override;

public:
    MoldableLink (const std::string& node);

    inline virtual std::string moldableValue()
    {
        return __link->text().toUTF8();
    }

    // Not currently supported in Wt
    inline virtual void setMoldableValue(const std::string& str) {}

    inline virtual void reload()
    {
        force_reload = true;
        Moldable::__init__();
        __init__();
        load();
        force_reload = false;
    }
};

#endif /* MOLDABLELINK_H_ */
