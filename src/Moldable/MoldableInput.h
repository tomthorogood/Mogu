/*
 * MoldableInput.h
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#ifndef MOLDABLEINPUT_H_
#define MOLDABLEINPUT_H_

#include "Moldable.h"
#include <Wt/WValidator>
#include <Wt/WLineEdit>

class MoldableInput : public Moldable
{
    std::string __assembly_validator;
    std::string __assembly_txt;
    Wt::WValidator* __validator = nullptr;
protected:
    Wt::WLineEdit* __input = nullptr;
    virtual void __init__(WidgetAssembly* assembly) override;
    void initializeInput();

public:
    MoldableInput (WidgetAssembly*);

    inline virtual std::string moldableValue() {
        return __input->valueText().toUTF8();
    }

    inline virtual void setMoldableValue(const std::string& txt) {
        __input->setEmptyText(txt);
    }

    inline virtual void validate()
    {
        Wt::WValidator::State result = __input->validate();
        if (result == Wt::WValidator::Valid) {
            succeed().emit();
            return;
        }
        fail().emit();
    }

    inline virtual void reload()
    {
        setFlag(MoldableFlags::allow_reload);
        initializeGlobalAttributes();
        initializeInput();
        load();
        unsetFlag(MoldableFlags::allow_reload);;
    }
};


#endif /* MOLDABLEINPUT_H_ */
