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
    EventHandler* bindery = nullptr;

    Wt::Signal <> sig_style_changed;
    Wt::Signal <> sig_failed_test;
    Wt::Signal <> sig_succeeded_test;
    Wt::Signal <> sig_loaded;
    Wt::Signal <> sig_hidden_changed;
    Wt::Signal <> sig_index_changed;

    const SyntaxDef&  widget_type;

    size_t num_triggers;

    bool updateStackIndex(size_t  index);
    void setFlags(Redis::NodeEditor&);

    std::string assembly_style;
    std::string assembly_tooltip;

protected:
    uint8_t flags     = 0;
    std::string node;
    std::string template_name;

    virtual void init(WidgetAssembly*);
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
        sig_style_changed.emit();
    }

    inline std::string getNode()
    {
        return node;
    }

    inline virtual void load() override
    {
        if (loaded())
        {
            if (!testFlag(MoldableFlags::allow_reload))
                return;
        }
        Wt::WContainerWidget::load();
    }

    inline Wt::Signal <>& styleChanged()    { return sig_style_changed; }
    inline Wt::Signal <>& fail()            { return sig_failed_test; }
    inline Wt::Signal <>& succeed()         { return sig_succeeded_test;}
    inline Wt::Signal <>& onLoad()          { return sig_loaded; }
    inline Wt::Signal <>& hiddenChanged()   { return sig_hidden_changed; }
    inline Wt::Signal <>& indexChanged()    { return sig_index_changed;}

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
        flags |= (uint8_t)MoldableFlags::allow_reload;
        clear();
        initializeGlobalAttributes();
        load();
        flags -= (uint8_t)MoldableFlags::allow_reload;
    }

    /*!\deprecated*/
    inline virtual void reset()     {reload();}
    inline void setFlag(MoldableFlags flag) { flags |= (uint8_t) flag; }

    inline void unsetFlag(MoldableFlags flag) { 
        if (testFlag(flag))
            flags -= (uint8_t) flag; 
    }
    inline bool testFlag(MoldableFlags flag) {return flags & (uint8_t) flag; }
    inline void shun() { setFlag(MoldableFlags::shun); }
    inline void unshun() { unsetFlag(MoldableFlags::shun); }

};


#endif /* MOLDABLE_H_ */
