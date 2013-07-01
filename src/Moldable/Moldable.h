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

class Moldable;

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


    size_t num_triggers;
    MoguSyntax __widget_type   = MoguSyntax::__NONE__;

    bool updateStackIndex(size_t  index);

    // Whether or not the base application allows this widget to talk to it.
    bool shunned = false;
    bool is_templated;

protected:
    bool force_reload;
    std::string __node;
    const char* template_name = EMPTY;

    virtual void __init__();
    std::string getParameter(Redis::ContextQuery&,MoguSyntax);

public:

    Moldable(const std::string& node, const MoguSyntax widget_type);

    operator std::string() {
        std::string s_type = std::to_string((int)__widget_type);
        std::string s_num_children = std::to_string(children().size());
        return "Widget "
            + getNode()
            + ": Type "
            + s_type + ": # Children: "
            + s_num_children;
    }

    virtual ~Moldable();

    virtual std::string moldableValue()                 =0;

    virtual void setMoldableValue(const std::string&)   =0;
    virtual void appendChild(Moldable*){}

    inline size_t getNumTriggers() { return num_triggers;}

    virtual void getAttribute(MoguSyntax state, NodeValue& val);
    virtual bool setAttribute(const MoguSyntax state, NodeValue& val);
    inline virtual bool setAttribute(const MoguSyntax state, NodeValue&& val)
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

    virtual void load() override;

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
        force_reload = true;
        clear();
        __init__();
        load();
        force_reload = false;
    }

    inline virtual void reset()     {__init__();}

    inline void shun() { shunned = true;}
    inline void unshun() { shunned = false;}
};


#endif /* MOLDABLE_H_ */
