/*
 * MoldableText.h
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#ifndef MOLDABLETEXT_H_
#define MOLDABLETEXT_H_

#include "Moldable.h"
#include <Wt/WText>

class MoldableText : public Moldable
{
    Wt::WText* __text;
    void initializeText();
    std::string __assembly_text;
protected:
    virtual void __init__(WidgetAssembly*) override;
public:
    MoldableText(WidgetAssembly*);

    inline virtual std::string moldableValue() {
        return __text->text().toUTF8();
    }

    inline virtual void setMoldableValue(const std::string& value) {
        __text->setText(value);
    }

    inline virtual void reload()
    {
        clear();
        testFlag(MoldableFlags::allow_reload);

        load();
        testFlag(MoldableFlags::allow_reload);
    }
};


#endif /* MOLDABLETEXT_H_ */
