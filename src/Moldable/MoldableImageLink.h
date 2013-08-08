/*
 * Moldable_Image_Link.h
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#ifndef MOLDABLEIMAGELINK_H_
#define MOLDABLEIMAGELINK_H_

#include "MoldableLink.h"

class Moldable_Image_Link : public Moldable_Link
{
    Wt::WImage* image {};
    std::string assembly_src {}

protected:
    virtual void init(Widget_Assembly* assembly);
    void initialize_image();

public:
    Moldable_Image_Link(Widget_Assembly*);
    inline virtual void reload()
    {
        set_flag(Moldable_Flags::allow_reload);
        initialize_global_attributes();
        initialize_link();
        initialize_image();
        load();
        unset_flag(Moldable_Flags::allow_reload);
    }
};

#endif /* MOLDABLEIMAGELINK_H_ */
