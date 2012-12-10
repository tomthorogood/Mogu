/*
 * MoldableLink.h
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#ifndef MOLDABLELINK_H_
#define MOLDABLELINK_H_

#include "Moldable.h"

class MoldableLink : public Moldable
{
protected:
    Wt::WAnchor* __link;
public:
    MoldableLink (const std::string& node);

    inline virtual std::string moldableValue()
    {
        return __link->text().toUTF8();
    }

    // Not currently supported in Wt
    inline virtual void setMoldableValue(const std::string& str) {}
};

#endif /* MOLDABLELINK_H_ */
