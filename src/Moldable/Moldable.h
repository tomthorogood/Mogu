/*
 * Moldable.h
 *
 *  Created on: Dec 9, 2012
 *      Author: tom
 */

#ifndef MOLDABLE_H_
#define MOLDABLE_H_

#include <declarations.h>
#include <Wt/WContainerWidget>
#include <Wt/WSignal>
#include <Types/NodeValue.h>
#include <Types/syntax.h>
#include <Redis/NodeEditor.h>
#include <Redis/ContextQuery.h>

class Moldable;

enum class MoldableFlags
{
    is_templated        =1
    , has_children      =2
    , template_children =4
    , has_events        =8
    , template_events   =16
    , is_cached         =32
    , allow_reload      =64
    , shun              =128
};

/* Simply an interface that can be implemented to expose widgets to 
 * manipulation by external forces.
 */
class AllowsExternalManipulation : public Wt::WContainerWidget
{
public:
    /* Overload of the parent class 'addChild' widget to provide public access
     * to manipulate widget tree for Moldable widgets.
     */
    inline virtual void addChild(Moldable* child) {
        Wt::WContainerWidget::addChild(reinterpret_cast<Wt::WWidget*>(child));
    }

    /* Overload of the parent class 'removeChild' widget to provide public
     * access to manipulate widget tree for Moldable widgets.
     */
    inline virtual void removeChild(Moldable* child) {
        Wt::WContainerWidget::removeChild(reinterpret_cast<Wt::WWidget*>(child));
    }
};


class Moldable : 
    public AllowsExternalManipulation
{
    EventHandler* __bindery = nullptr;

    Wt::Signal <> __style_changed;
    Wt::Signal <> __failed_test;
    Wt::Signal <> __succeeded_test;
    Wt::Signal <> __loaded;
    Wt::Signal <> __hidden_changed;
    Wt::Signal <> __index_changed;

    const SyntaxDef&  __widget_type;

    size_t num_triggers;

    bool updateStackIndex(size_t  index);
    void setFlags(Redis::NodeEditor&);

    std::string __assembly_style;
    std::string __assembly_tooltip;

protected:
    uint8_t __flags     = 0;
    std::string __node;
    std::string __template_name;

    virtual void __init__(WidgetAssembly*);
    void initializeGlobalAttributes();

public:

    Moldable(WidgetAssembly*, const SyntaxDef& widget_type);

    virtual ~Moldable();

    virtual std::string moldableValue()                 =0;

    virtual void setMoldableValue(const std::string&)   =0;
    virtual void appendChild(Moldable*){}

    inline size_t getNumTriggers() { return num_triggers;}

    virtual void getAttribute(const SyntaxDef& state, NodeValue& val);
    virtual bool setAttribute(const SyntaxDef& state, NodeValue& val);
    inline virtual bool setAttribute(const SyntaxDef& state, NodeValue&& val)
        { return setAttribute(state, val);}

    inline virtual void setStyleClass (const Wt::WString& style)
    {
        Wt::WContainerWidget::setStyleClass(style);
        __style_changed.emit();
    }

    inline std::string getNode()
    {
        return __node;
    }

    inline virtual void load() override
    {
        if (loaded() && !testFlag(MoldableFlags::allow_reload)) return;
        Wt::WContainerWidget::load();
    }

    inline Wt::Signal <>& styleChanged()    { return __style_changed; }
    inline Wt::Signal <>& fail()            { return __failed_test; }
    inline Wt::Signal <>& succeed()         { return __succeeded_test;}
    inline Wt::Signal <>& onLoad()          { return __loaded; }
    inline Wt::Signal <>& hiddenChanged()   { return __hidden_changed; }
    inline Wt::Signal <>& indexChanged()    { return __index_changed;}

    inline void increment(int byAmount=1) {
        NodeValue v(0);
        getAttribute(MoguSyntax::index,v);
        int newindex = v.getInt() + byAmount;
        v.setInt(newindex);
        setAttribute(MoguSyntax::index,v);
    }

    inline void increment(NodeValue& val) {
        int amt = val.getInt();
        getAttribute(MoguSyntax::index,val);
        amt += val.getInt();
        val.setInt(amt);
        setAttribute(MoguSyntax::index,val);
    }

    inline void decrement(int byAmount=1) {
        NodeValue v(0);
        getAttribute(MoguSyntax::index,v);
        int newindex = v.getInt()-byAmount;
        v.setInt(newindex);
        setAttribute(MoguSyntax::index,v);
    }

    inline void decrement(NodeValue& val) {
        int amt = val.getInt();
        getAttribute(MoguSyntax::index,val);
        amt = val.getInt()-amt;
        setAttribute(MoguSyntax::index,val);
    }


    virtual void inline reload()
    {
        __flags |= (uint8_t)MoldableFlags::allow_reload;
        clear();
        initializeGlobalAttributes();
        load();
        __flags -= (uint8_t)MoldableFlags::allow_reload;
    }

    /*!\deprecated*/
    inline virtual void reset()     {reload();}
    inline void setFlag(MoldableFlags flag) { __flags |= (uint8_t) flag; }

    inline void unsetFlag(MoldableFlags flag) { 
        if (testFlag(flag))
            __flags -= (uint8_t) flag; 
    }
    inline bool testFlag(MoldableFlags flag) {return __flags & (uint8_t) flag; }
    inline void shun() { setFlag(MoldableFlags::shun); }
    inline void unshun() { unsetFlag(MoldableFlags::shun); }

};


#endif /* MOLDABLE_H_ */
