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
#include <Moldable/Moldable.h>
#include <Security/UserManager.h>
#include <Factories/MoldableFactory.h>
#include <Types/SlotManager.h>
#include <Parsers/NodeValueParser.h>


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
    ContextMap* contextMap_;

    /*!\brief A map of named widgets. */
    std::unordered_map <std::string, Moldable*> widgetRegister;

    void loadMoguStyles();

    /*!\brief The widget that started it all... */
    Moldable* __wrapper;

    Parsers::NodeValueParser __interpreter;

    std::string __user_keyspace;  //!< Currently active user session
    int __group;    //!< Currently active user group
    std::string __instanceid;

    UserManager* userManager;

    SlotManager __slotMgr;

public:
    Mogu(const Wt::WEnvironment& env);
    virtual ~Mogu();
    
    /*!\brief Adds a widget into the widget registry.
     *
     * @param name The name used for looking up the widget
     * @param widget The pointer to the widget itself.
     */

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
        try {
            return widgetRegister.at(name); 
        } catch (const std::out_of_range& e) {
            return nullptr;
        }
        return nullptr;
    }

    //TODO
    inline void setPath(const std::string&){}

    /*!\brief Removes a widget from the registry. */
    inline void deregisterWidget(
        std::string name)
    {
        widgetRegister.erase(name);
    }

    inline std::string& instanceID()
    {
        return __instanceid;
    }

    inline const std::string& getUser() const
    {
        return userManager->getUser();
    }

    inline UserManager& getUserManager() {return *userManager;}
    inline const int& getGroup() const {return __group;}
    inline const MoldableFactory& getFactory() { return moldableFactory; }
    inline SlotManager& slotManager() { return __slotMgr;}
    inline void alert (const std::string& message) {
        std::string jsalert = "alert(\""+message+"\");";
	    doJavaScript(jsalert);
    }

    inline ContextMap* contextMap() { return contextMap_;}
    void removeWidget(const std::string& identifier);
};

#endif /* PREVIEW_H_ */
