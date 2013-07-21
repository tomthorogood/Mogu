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
    Wt::WImage* image   =nullptr;
    std::string assembly_src;
    std::string assembly_txt;
protected:
    virtual void init(WidgetAssembly*) override;
    void initializeImage();

public:
    MoldableImage(WidgetAssembly*);

    inline virtual std::string moldableValue()
    {
        return image->imageLink().url();
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
