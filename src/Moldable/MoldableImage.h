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
        __init__();
        load();
        force_reload = false;
    }
};


#endif /* MOLDABLEIMAGE_H_ */
