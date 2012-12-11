/*
 * MoldableAbstractParent.h
 *
 *  Created on: Dec 11, 2012
 *      Author: tom
 */

#ifndef MOLDABLEABSTRACTPARENT_H_
#define MOLDABLEABSTRACTPARENT_H_

#include "Moldable.h"

class MoldableAbstractParent : public Moldable
{
protected:
    bool has_children;
    std::vector <Moldable*> __moldable_children;
public:
    MoldableAbstractParent (const std::string& node);
    virtual void load();

    inline const std::vector<Moldable*>& moldableChildren() {
        return __moldable_children;
    }

    inline Moldable* moldableChild(size_t index) {
        return __moldable_children[index];
    }
};


#endif /* MOLDABLEABSTRACTPARENT_H_ */
