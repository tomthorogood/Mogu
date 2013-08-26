/*
 * Moldable_Image.h
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#ifndef MOLDABLEIMAGE_H_
#define MOLDABLEIMAGE_H_

#include "Moldable.h"
#include <Wt/WLink>
#include <Wt/WImage>

class Moldable_Image : public Moldable
{
    Wt::WImage* image {};
    std::string assembly_src {};
    std::string assembly_txt {};
protected:
    virtual void init(Widget_Assembly&) override;
    void initialize_image();

public:
    Moldable_Image(Widget_Assembly&);

    inline virtual std::string get_moldable_value()
    {
        return image->imageLink().url();
    }

    //Currently, this operation is unsupported by Wt.
    inline virtual void set_moldable_value(const std::string& str_src) {}

    inline virtual void reload()
    {
        set_flag(Moldable_Flags::allow_reload);
        clear();
        initialize_global_attributes();
        initialize_image();
        load();
        unset_flag(Moldable_Flags::allow_reload);;
    }
};


#endif /* MOLDABLEIMAGE_H_ */
