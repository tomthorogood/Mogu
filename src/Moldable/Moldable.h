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
#include <Types/Node_Value.h>
#include <Types/syntax.h>
#include <Redis/NodeEditor.h>

class Moldable;

enum class Moldable_Flags
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
class Allows_External_Manipulation : public Wt::WContainerWidget
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
    public Allows_External_Manipulation
{
    Event_Handler* bindery {};

    Wt::Signal <> sig_style_changed;
    Wt::Signal <> sig_failed_test;
    Wt::Signal <> sig_succeeded_test;
    Wt::Signal <> sig_loaded;
    Wt::Signal <> sig_hidden_changed;
    Wt::Signal <> sig_index_changed;
    Wt::Signal <> sig_error_reported;

    const Syntax_Def&  widget_type;

    size_t num_triggers {};

    bool update_stack_index(size_t);
    void set_flags(Redis::Node_Editor&);

    std::string assembly_style {};
    std::string assembly_tooltip {};

protected:
    uint8_t flags {};
    std::string node {}
    std::string template_name {}

    virtual void init(Widget_Assembly*);
    void initialize_global_attributes();

public:

    Moldable(Widget_Assembly*, const Syntax_Def& type);

    virtual ~Moldable();

    virtual std::string moldable_value() =0;

    virtual void set_moldable_value(const std::string&) =0;
    virtual void append_child(Moldable*)  {}

    inline size_t get_num_triggers() { return num_triggers;}

    virtual void get_attribute(const Syntax_Def& a, Node_Value& v);
    virtual bool set_attribute(const Syntax_Def& a, Node_Value& v);

    /* Overload of parent class, which also emits the style_chaned signal.*/
    inline virtual void setStyleClass (const Wt::WString& style)
    {
        Wt::WContainerWidget::setStyleClass(style);
        sig_style_changed.emit();
    }

    inline const std::string& get_node() 
        { return node; }
    inline std::string get_node()
    {
        return node;
    }

    inline virtual void load() override
    {
        if (loaded())
        {
            if (!test_flag(Moldable_Flags::allow_reload)) return;
        }
        Wt::WContainerWidget::load();
    }

    /* Keeping with the Wt signal styling here...hence the camelCase */
    inline Wt::Signal <>& styleChanged()    { return sig_style_changed; }
    inline Wt::Signal <>& fail()            { return sig_failed_test; }
    inline Wt::Signal <>& succeed()         { return sig_succeeded_test;}
    inline Wt::Signal <>& onLoad()          { return sig_loaded; }
    inline Wt::Signal <>& hiddenChanged()   { return sig_hidden_changed; }
    inline Wt::Signal <>& indexChanged()    { return sig_index_changed;}
    inline Wt::Signal <>& errorReported()   { return sig_error_reported;}

    inline void increment(int amt=1)
    {
        Node_Value v {};
        get_attribute(Mogu_Syntax::index,v);
        int i {v.get_int() + amt};
        v.set_int(i);
        set_attribute(Mogu_Syntax::index,v);
    }

    inline void increment(Node_Value& v)
    {
        increment(v.get_int());
    }

    inline void decrement(int amt=1)
    {
        Node_Value v {};
        get_attribute(Mogu_Syntax::index,v);
        int i {v.get_int()-amt};
        v.set_int(i);
        set_attribute(Mogu_Syntax::index,v);
    }

    inline void decrement(Node_Value& val) {
        decrement(val.get_int());
    }

    virtual void inline reload()
    {
        flags |= (uint8_t)Moldable_Flags::allow_reload;
        clear();
        initialize_global_attributes();
        load();
        flags -= (uint8_t)Moldable_Flags::allow_reload;
    }

    inline void set_flag(Moldable_Flags f) 
        { flags |= (uint8_t) f;}

    inline void unset_flag(Moldable_Flags f)
        { flags -= (uint8_t)f;}
    
    inline void test_flag(Moldable_Flags f)
        { return flags & (uint8_t) f; }

    inline void shun()
        { set_flag(Moldable_Flags::shun); }
    
    inline void unshun() 
        { unset_flag(Moldable_Flags::shun); }

};

#endif /* MOLDABLE_H_ */
