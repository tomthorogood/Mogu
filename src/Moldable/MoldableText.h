/*
 * Moldable_Text.h
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#ifndef MOLDABLETEXT_H_
#define MOLDABLETEXT_H_

#include "Moldable.h"
#include <Wt/WText>

class Moldable_Text : public Moldable
{
    Wt::WText* text {};
    std::string assembly_text {};
    void initialize_text();

protected:
    virtual void init(Widget_Assembly&) override;

public:
    Moldable_Text(Widget_Assembly&);

    inline virtual std::string get_moldable_value()
        { return text->text().toUTF8(); }

    inline virtual void set_moldable_value(const std::string& value)
        { text->setText(value); }

    inline virtual void reload()
    {
        clear();
        test_flag(Moldable_Flags::allow_reload);
        load();
        test_flag(Moldable_Flags::allow_reload);
    }
};

#endif /* MOLDABLETEXT_H_ */
