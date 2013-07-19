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
    std::string __assembly_src;
    std::string __assembly_txt;
protected:
    virtual void __init__(WidgetAssembly*) override;
    void initializeImage();

public:
    MoldableImage(WidgetAssembly*);

    inline virtual std::string moldableValue()
    {
        return __image->imageLink().url();
    }

    //Currently, this operation is unsupported by Wt.
    inline virtual void setMoldableValue(const std::string& str_src) {}

    inline virtual void reload()
    {
        setFlag(MoldableFlags::allow_reload);;
        clear();
        initializeGlobalAttributes();
        initializeImage();
        load();
        unsetFlag(MoldableFlags::allow_reload);;
    }
};


#endif /* MOLDABLEIMAGE_H_ */
