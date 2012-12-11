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

class Moldable : public Wt::WContainerWidget
{
    Events::EventBindery* __bindery;

    Wt::Signal <> __style_changed;
    Wt::Signal <> __failed_test;
    Wt::Signal <> __succeeded_test;
    Wt::Signal <> __loaded;
    Wt::Signal <> __hidden_changed;

    void __init__();

    bool has_events;
    bool has_children;

protected:
    MoguNode __node;
    std::string getParameter(const std::string& param);
    std::string getParameter(const std::string&& param);

public:

    Moldable(const std::string& node);

    virtual ~Moldable() {
        if (__bindery != NULL) delete __bindery;
    }

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

    inline Wt::Signal <> styleChanged() { return __style_changed; }
    inline Wt::Signal <> fail() { return __failed_test; }
    inline Wt::Signal <> succeed() { return __succeeded_test;}
    inline Wt::Signal <> onLoad() { return __loaded; }
    inline Wt::Signal <> hiddenChanged() { return __hidden_changed; }

};

#endif /* MOLDABLE_H_ */
