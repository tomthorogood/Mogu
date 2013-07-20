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
    Wt::WText* text;
    void initializeText();
    std::string assembly_text;
protected:
    virtual void init(WidgetAssembly*) override;
public:
    MoldableText(WidgetAssembly*);

    inline virtual std::string moldableValue() {
        return text->text().toUTF8();
    }

    inline virtual void setMoldableValue(const std::string& value) {
        text->setText(value);
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
