/*
 * Moldable_Abstract_Parent.h
 *
 *  Created on: Dec 11, 2012
 *      Author: tom
 */

#ifndef MOLDABLEABSTRACTPARENT_H_
#define MOLDABLEABSTRACTPARENT_H_

#include "Moldable.h"

class Moldable_Abstract_Parent : public Moldable
{
protected:
    int num_children;
    std::vector <Moldable*> moldable_children;
    virtual void init(Widget_Assembly&) override;
    std::vector <std::string> child_nodes;
public:
    Moldable_Abstract_Parent (Widget_Assembly&, const Syntax_Def&);
    virtual void load() override;

    virtual void append_child(Moldable*) =0;

    inline virtual void reload()
    {
        clear();
        set_flag(Moldable_Flags::allow_reload);
        initialize_global_attributes();
        load();
        unset_flag(Moldable_Flags::allow_reload);
    }
};


#endif /* MOLDABLEABSTRACTPARENT_H_ */
