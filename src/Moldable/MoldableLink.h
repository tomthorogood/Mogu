/*
 * Moldable_Link.h
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#ifndef MOLDABLELINK_H_
#define MOLDABLELINK_H_

#include "Moldable.h"
#include <Wt/WAnchor>

class Moldable_Link : public Moldable
{
private:
    std::string assembly_location {};
    std::string assembly_text {};

protected:
    Wt::WAnchor* link {};
    virtual void init(Widget_Assembly&) override;
    void initialize_link();

public:
    Moldable_Link (Widget_Assembly& assembly);

    inline virtual std::string get_moldable_value()
    {
        return link->text().toUTF8();
    }

    // Not currently supported in Wt
    inline virtual void set_moldable_value(const std::string& str) {}

    inline virtual void reload()
    {
        test_flag(Moldable_Flags::allow_reload);;
        initialize_link();
        load();
        test_flag(Moldable_Flags::allow_reload);;
    }
};

#endif /* MOLDABLELINK_H_ */
