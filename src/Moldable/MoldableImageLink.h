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
    Wt::WImage* image;
    std::string assembly_src;
protected:
    virtual void init(WidgetAssembly* assembly);
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
