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
    std::string __assembly_src;
protected:
    virtual void __init__(WidgetAssembly* assembly);
    void initializeImage();

public:
    MoldableImageLink(WidgetAssembly*);
    inline virtual void reload()
    {
        setFlag(MoldableFlags::allow_reload);
        initializeGlobalAttributes();
        initializeLink();
        initializeImage();
        load();
        unsetFlag(MoldableFlags::allow_reload);;
    }
};

#endif /* MOLDABLEIMAGELINK_H_ */
