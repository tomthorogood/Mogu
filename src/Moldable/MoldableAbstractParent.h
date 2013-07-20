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
    std::vector <Moldable*> moldable_children;
    virtual void init(WidgetAssembly*) override;
    std::vector <std::string> child_nodes;
public:
    MoldableAbstractParent (WidgetAssembly*, const SyntaxDef&);
    virtual void load() override;

    virtual void appendChild(Moldable*) =0;
    inline const std::vector<Moldable*>& moldableChildren() {
        return moldable_children;
    }

    inline virtual void reload()
    {
        clear();
        setFlag(MoldableFlags::allow_reload);
        initializeGlobalAttributes();
        load();
        unsetFlag(MoldableFlags::allow_reload);
    }
};


#endif /* MOLDABLEABSTRACTPARENT_H_ */
