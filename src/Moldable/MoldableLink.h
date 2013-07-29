/*
 * MoldableLink.h
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#ifndef MOLDABLELINK_H_
#define MOLDABLELINK_H_

#include "Moldable.h"
#include <Wt/WAnchor>

class MoldableLink : public Moldable
{
private:
    std::string assembly_location;
    std::string assembly_text;
protected:
    Wt::WAnchor* link = nullptr;
    virtual void init(WidgetAssembly*) override;
    void initializeLink();
public:
    MoldableLink (WidgetAssembly* assembly);

    inline virtual std::string moldableValue()
    {
        return link->text().toUTF8();
    }

    // Not currently supported in Wt
    inline virtual void setMoldableValue(const std::string& str) {}

    inline virtual void reload()
    {
        testFlag(MoldableFlags::allow_reload);;
        initializeLink();
        load();
        testFlag(MoldableFlags::allow_reload);;
    }
};

#endif /* MOLDABLELINK_H_ */
