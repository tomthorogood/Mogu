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
#include <Types/MoguNode.h>
#include <Types/NodeValue.h>
#include <Types/syntax.h>

struct TemplateNodeReply {
    bool exists;
    std::string response;

    TemplateNodeReply(const char* widgetnode, const char* field);
};

class Moldable : public Wt::WContainerWidget
{
    Events::EventBindery* __bindery;

    Wt::Signal <> __style_changed;
    Wt::Signal <> __failed_test;
    Wt::Signal <> __succeeded_test;
    Wt::Signal <> __loaded;
    Wt::Signal <> __hidden_changed;
    Wt::Signal <> __index_changed;


    size_t num_events;

protected:
    bool force_reload;
    const char* template_name = EMPTY;
    virtual void __init__();

    MoguNode __node;
    std::string getParameter(Redis::ContextQuery&,MoguSyntax);

public:

    Moldable(const std::string& node);

    virtual ~Moldable();

    virtual std::string moldableValue()                 =0;
    virtual void setMoldableValue(const std::string&)   =0;
    virtual void getAttribute(
        MoguSyntax state, NodeValue& val);

    virtual bool setAttribute(
        MoguSyntax state, NodeValue& val);

    inline virtual void setStyleClass (const Wt::WString& style)
    {
        Wt::WContainerWidget::setStyleClass(style);
        __style_changed.emit();
    }

    inline virtual std::string getNode()
    {
        return __node.addPrefix("widgets");
    }

    virtual void load();

    inline Wt::Signal <>& styleChanged() { return __style_changed; }
    inline Wt::Signal <>& fail() { return __failed_test; }
    inline Wt::Signal <>& succeed() { return __succeeded_test;}
    inline Wt::Signal <>& onLoad() { return __loaded; }
    inline Wt::Signal <>& hiddenChanged() { return __hidden_changed; }
    inline Wt::Signal <>& indexChanged() { return __index_changed;}

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

    //!\TODO
    virtual void reset();

};

#endif /* MOLDABLE_H_ */
