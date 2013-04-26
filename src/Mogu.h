/*
 * preview.h
 *
 *  Created on: May 29, 2012
 *      Author: tom
 */

#ifndef PREVIEW_H_
#define PREVIEW_H_

#include <declarations.h>
#include <Wt/WApplication>
#include <signal.h>
#include <Redis/DatabaseConfigReader.h>
#include <Redis/RedisCore.h>
#include <Moldable/Moldable.h>
#include <Security/UserManager.h>
#include <Parsers/NodeValueParser.h>
#include <Factories/MoldableFactory.h>
#include <Types/SlotManager.h>

#ifndef AUTH_TOKEN
#define AUTH_TOKEN "BendTheWeb"
#endif

#ifndef GLOBAL_SESSION
#define GLOBAL_SESSION "global"
#endif

class Mogu: public Wt::WApplication
{
    /*!\brief Changes the state of the application based on the URL */
    void handlePathChange(
        std::string path);
    MoldableFactory moldableFactory;

    /*!\brief A map of named widgets. */
    WidgetRegister widgetRegister;

    void loadMoguStyles();

    /*!\brief The widget that started it all... */
    Moldable* __wrapper;

    Parsers::NodeValueParser __interpreter;

    std::string __user_keyspace;  //!< Currently active user session
    std::string __group;    //!< Currently active user group
    std::string __instanceid;
    redisContext* __redis;  //!< Database connection
    redisReply* __reply;    //!< State of last database query

    ApplicationManager manager;
    Security::UserManager userManager;

    SlotManager __slotMgr;

public:
    Mogu(const Wt::WEnvironment& env);
    virtual ~Mogu();

    /*!\brief Returns whether or not a name represents a registered widget
     * within the application instance.
     * @param name The name of the widget being sought.
     */
    inline bool widgetIsRegistered(
        std::string name)
    {
        WidgetRegister::iterator iter = widgetRegister.find(name);
        return iter != widgetRegister.end();
    }

    /*!\brief Adds a widget into the widget registry.
     *
     * @param name The name used for looking up the widget
     * @param widget The pointer to the widget itself.
     */
    inline void registerWidget(
        std::string name, Moldable* widget)
    {
        widgetRegister[name] = widget;
    }

    inline void registerWidget(
        std::string name, Moldable& widget)
    {
        widgetRegister[name] = &widget;
    }

    inline Parsers::NodeValueParser& interpreter()
    {
        return __interpreter;
    }

    /*!\brief Returns a widget from the registry based on its name. */
    inline Moldable* registeredWidget(
        std::string name)
    {      
       return widgetIsRegistered(name) ? widgetRegister.at(name) : NULL; 
    }

    /*!\brief Removes a widget from the registry. */
    inline void deregisterWidget(
        std::string name)
    {
        if (!widgetIsRegistered(name)) return;
        WidgetRegister::iterator iter = widgetRegister.find(name);
        widgetRegister.erase(iter);
    }

    inline std::string& instanceID()
    {
        return __instanceid;
    }

    inline void setUser(const std::string& userkeyspace)
    {
        __user_keyspace = userkeyspace;
    }

    inline const std::string& getUser()
    {
        return __user_keyspace;
    }

    inline Security::UserManager& getUserManager() {return userManager;}
    inline void setGroup(const std::string& group) {__group = group;}
    inline void setGroup(const std::string&& group) {__group = group;}
    inline std::string& getGroup() {return __group;}
    inline const MoldableFactory& getFactory() { return moldableFactory; }
    inline SlotManager& slotManager() { return __slotMgr;}
    void removeWidget(const std::string& identifier);
    };

#endif /* PREVIEW_H_ */
