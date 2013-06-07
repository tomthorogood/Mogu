/*
 * MoldableImage.h
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#ifndef MOLDABLEIMAGE_H_
#define MOLDABLEIMAGE_H_

#include "Moldable.h"
#include <Wt/WLink>
#include <Wt/WImage>

class MoldableImage : public Moldable
{
    Wt::WImage* __image;

protected:
    virtual void __init__() override;
    inline virtual void setWidgetType() {
        __widget_type = MoguSyntax::image;
    }

public:
    MoldableImage(const std::string& node);

    inline virtual std::string moldableValue()
    {
        return __image->imageLink().url();
    }

    //Currently, this operation is unsupported by Wt.
    inline virtual void setMoldableValue(const std::string& str_src) {}

    inline virtual void reload()
    {
        force_reload = true;
        clear();
        Moldable::__init__();
        __init__();
        load();
        force_reload = false;
    }
};


#endif /* MOLDABLEIMAGE_H_ */
