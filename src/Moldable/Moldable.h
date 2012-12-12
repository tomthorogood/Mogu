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

class Moldable : public Wt::WContainerWidget
{
    Events::EventBindery* __bindery;

    Wt::Signal <> __style_changed;
    Wt::Signal <> __failed_test;
    Wt::Signal <> __succeeded_test;
    Wt::Signal <> __loaded;
    Wt::Signal <> __hidden_changed;


    bool has_events;

protected:
    bool force_reload;
    virtual void __init__();

    MoguNode __node;
    std::string getParameter(const std::string& param);
    inline std::string getParameter(const std::string&& param)
    {
        return getParameter(param);
    }

public:

    Moldable(const std::string& node);

    virtual ~Moldable();

    virtual std::string moldableValue() =0;
    virtual void setMoldableValue(const std::string&) =0;
    virtual void getState(
        Enums::WidgetTypes::States state, NodeValue& val);

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

    bool allowsAction(Enums::SignalActions::SignalAction action);

    virtual void inline reload()
    {
        force_reload = true;
        clear();
        __init__();
        load();
        force_reload = false;
    }

};

#endif /* MOLDABLE_H_ */
