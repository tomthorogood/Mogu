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
protected:
    Wt::WLineEdit* __input;
    virtual void __init__() override;
    virtual void setWidgetType() {
        __widget_type = MoguSyntax::input;
    }
public:
    MoldableInput (const std::string& node);

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
        Moldable::__init__();
        __init__();
        load();
        force_reload = false;
    }
};


#endif /* MOLDABLEINPUT_H_ */
