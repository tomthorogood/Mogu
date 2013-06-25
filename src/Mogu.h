/*
 * preview.h
 *
 *  Created on: May 29, 2012
 *      Author: tom
 */

#ifndef MOGU_H_
#define MOGU_H_

#include <declarations.h>
#include <Wt/WApplication>
#include <signal.h>
#include <Redis/DatabaseConfigReader.h>
#include <Moldable/Moldable.h>
#include <Security/UserManager.h>
#include <Factories/MoldableFactory.h>
#include <Types/SlotManager.h>
#include <Parsers/NodeValueParser.h>

class Mogu: public Wt::WApplication
{
    /*!\brief Changes the state of the application based on the URL */
    MoldableFactory moldableFactory;
    ContextMap* contextMap_         =nullptr;
    Moldable* __wrapper             =nullptr;
    UserManager* userManager        =nullptr;

    /*!\brief A map of named widgets. */
    std::unordered_map <std::string, Moldable*> widgetRegister;
    Parsers::NodeValueParser __interpreter;
    int __user;
    int __group                     =0;    //!< Currently active user group
    std::string __instanceid;
    SlotManager __slotMgr;

    void loadMoguStyles();
    void handlePathChange( std::string path);

    /*!\brief The widget that started it all... */

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

    inline void setPath(const std::string& path){
        setInternalPath(path, true);
    }

    /*!\brief Removes a widget from the registry. */
    inline void deregisterWidget(
        std::string name)
    {
        if (registeredWidget(name) != nullptr)
            widgetRegister[name] = nullptr;
    }

    inline std::string& instanceID()
    {
        return __instanceid;
    }
    
    inline const int& getUser() const
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

#endif /* MOGU_H_ */
