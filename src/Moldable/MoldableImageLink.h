/*
 * MoldableImageLink.h
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#ifndef MOLDABLEIMAGELINK_H_
#define MOLDABLEIMAGELINK_H_

#include "MoldableLink.h"

class MoldableImageLink : public MoldableLink
{
    Wt::WImage* __image;
protected:
    virtual void __init__();
public:
    MoldableImageLink(const std::string& node);
    inline virtual void reload()
    {
        Moldable::__init__();
        MoldableLink::__init__();
        __init__();
        force_reload = true;
        load();
        force_reload = false;
    }
};

#endif /* MOLDABLEIMAGELINK_H_ */
