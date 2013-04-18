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
protected:
    virtual void __init__() override;
public:
    MoldableText(const std::string& node);

    inline virtual std::string moldableValue() {
        return __text->text().toUTF8();
    }

    inline virtual void setMoldableValue(const std::string& value) {
        __text->setText(value);
    }

    inline virtual void reload()
    {
        clear();
        force_reload = true;
        __init__();
        load();
        force_reload = false;
    }

};


#endif /* MOLDABLETEXT_H_ */
