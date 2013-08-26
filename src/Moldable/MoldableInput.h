/*
 * Moldable_Input.h
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#ifndef MOLDABLEINPUT_H_
#define MOLDABLEINPUT_H_

#include "Moldable.h"
#include <Wt/WValidator>
#include <Wt/WLineEdit>

class Moldable_Input : public Moldable
{
    std::string assembly_validator {};
    std::string assembly_txt {};
    Wt::WValidator* validator_ptr {};

protected:
    Wt::WLineEdit* input {};
    virtual void init(Widget_Assembly& assembly) override;
    void initialize_input();

public:
    Moldable_Input (Widget_Assembly&);

    inline virtual std::string get_moldable_value() {
        std::string val = input->text().toUTF8();
        return val;
        //return input->valueText().toUTF8();
    }

    inline virtual void set_moldable_value(const std::string& txt) {
        input->setEmptyText(txt);
    }

    inline virtual void validate()
    {
        Wt::WValidator::State result = input->validate();
        if (result == Wt::WValidator::Valid) {
            succeed().emit();
            return;
        }
        fail().emit();
    }

    inline virtual void reload()
    {
        set_flag(Moldable_Flags::allow_reload);
        initialize_global_attributes();
        initialize_input();
        load();
        unset_flag(Moldable_Flags::allow_reload);;
    }
};


#endif /* MOLDABLEINPUT_H_ */
