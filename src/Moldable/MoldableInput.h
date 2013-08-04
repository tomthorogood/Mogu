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
    std::string assembly_validator;
    std::string assembly_txt;
    Wt::WValidator* validator_ptr = nullptr;
protected:
    Wt::WLineEdit* input = nullptr;
    virtual void init(WidgetAssembly* assembly) override;
    void initializeInput();

public:
    MoldableInput (WidgetAssembly*);

    inline virtual std::string moldableValue() {
        std::string val = input->text().toUTF8();
        return val;
        //return input->valueText().toUTF8();
    }

    inline virtual void setMoldableValue(const std::string& txt) {
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
        setFlag(MoldableFlags::allow_reload);
        initializeGlobalAttributes();
        initializeInput();
        load();
        unsetFlag(MoldableFlags::allow_reload);;
    }
};


#endif /* MOLDABLEINPUT_H_ */
