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
    int num_children;
    std::vector <Moldable*> __moldable_children;
    virtual void __init__() override;
public:
    MoldableAbstractParent (const std::string& node, const SyntaxDef& );
    virtual void load() override;

    virtual void appendChild(Moldable*) =0;
    inline const std::vector<Moldable*>& moldableChildren() {
        return __moldable_children;
    }

    inline virtual void reload()
    {
        clear();
        setFlag(MoldableFlags::allow_reload);
        Moldable::__init__();
        __init__();
        load();
        unsetFlag(MoldableFlags::allow_reload);
    }
};


#endif /* MOLDABLEABSTRACTPARENT_H_ */
