/*
 * preview.h
 *
 *  Created on: May 29, 2012
 *      Author: tom
 */

#ifndef MOGU_H_
#define MOGU_H_

#include <Wt/WApplication>
#include <signal.h>
#include <Redis/DatabaseConfigReader.h>
#include <Moldable/Moldable.h>
#include <Security/UserManager.h>
#include <Factories/MoldableFactory.h>
#include <Types/SlotManager.h>
#include <Parsers/NodeValueParser.h>
#include <Types/MoguLogger.h>

#define MoguApp static_cast<Mogu*>(wApp)

//Provides access to the user-specific instance of Mogu
#define mApp Mogu* app = MoguApp

class Mogu: public Wt::WApplication
{
    int group {-1};
    std::string application_name {};
    
    Moldable_Factory moldable_factory {};
    
    Moldable* wrapper {};
    User_Manager user_manager {};

    /*!\brief A map of named widgets. */
    std::unordered_map <std::string, Moldable*> widget_register {};
    Parsers::Node_Value_Parser interpreter;
    Slot_Manager slot_manager;

    void handle_path_change (const std::string& path);

public:
    Mogu(const Wt::WEnvironment& env);
    virtual ~Mogu();
    
    /*!\brief Adds a widget into the widget registry.
     *
     * @param name The name used for looking up the widget
     * @param widget The pointer to the widget itself.
     */

    inline void register_widget( const std::string& name, Moldable* widget)
        { widget_register[name] = widget; }

    inline Moldable* get_widget( const std::string& name)
    { return 
            widget_register.count(name) ? widget_register.at(name) : nullptr;
    }

    inline Parsers::Node_Value_Parser& get_interpreter()
        { return interpreter; }

    /* Expose this method to the public, instead of keeping it private. */
    inline void set_path(const std::string& path)
        { setInternalPath(path,true); }

    inline void deregister_widget(const std::string& name)
        { if (widget_register.count(name)) widget_register.erase(name); }

    inline const int& get_user() const
        { return user_manager.get_user(); }

    inline User_Manager& get_user_manager() 
        { return user_manager; }
    
    inline const int& get_group() const
        { return group; }
    
    inline void set_group(const int& i) {
        group = i;
    }

    inline Moldable_Factory& get_factory()
        { return moldable_factory; }
    
    inline Slot_Manager& get_slot_manager()
        { return slot_manager; }

    inline const std::string& get_application_name() const
        { return application_name; }
};

#endif /* MOGU_H_ */
