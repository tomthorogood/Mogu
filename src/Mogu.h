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
#include <Redis/RedisCore.h>
#include <Moldable/Moldable.h>
#include <Types/ApplicationManager.h>
#include <Security/UserManager.h>
#include <Parsers/NodeValueParser.h>
#include <Factories/MoldableFactory.h>

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

    std::string __session;  //!< Currently active user session
    std::string __group;    //!< Currently active user group
    std::string __auth_token;
    std::string __instanceid;
    redisContext* __redis;  //!< Database connection
    redisReply* __reply;    //!< State of last database query

    ApplicationManager manager;
    Security::UserManager userManager;

    std::map<std::string, std::string> __slots;

public:
    enum KeepReply
    {
        Discard, Keep
    };
    Mogu(
        const Wt::WEnvironment& env);
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

    inline void loadAnalytics(
        std::string id)
    {
        std::string script[] = { "var _gaq = _gaq || [];",
            "_gaq.push(['_setAccount', '", id, "']);",
            "_gaq.push(['_setDomainName', 'gomogu.org']);",
            "_gaq.push(['_trackPageview']);", "(function() {",
            "var ga = document.createElement('script');",
            "ga.type = 'text/javascript'; ga.async = true;",
            "ga.src = ('https:' == document.location.protocol ? ",
            "'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';",
            "var s = document.getElementsByTagName('script')[0];",
            "s.parentNode.insertBefore(ga, s);", "})();" };
        int array_len = 14;
        std::string final = "";
        for (int i = 0; i < array_len; i++) {
            final += script[i];
        }
        doJavaScript(final);
    }

    inline void redisExec(
        KeepReply r, const char* cmd, ...)
    {
        va_list ap;
        va_start(ap, cmd);
        __reply = (redisReply*) redisvCommand(__redis, cmd, ap);
#ifdef DEBUG
        va_start(ap, cmd);
        vprintf(cmd, ap);
        std::cout << std::endl;
#endif
        va_end(ap);
        if (r != Keep) freeReply();
    }

    inline void redisCommand(
        const char* cmd, ...)
    {
        va_list ap;
        va_start(ap, cmd);
        __reply = (redisReply*) redisvCommand(__redis, cmd, ap);
#ifdef DEBUG
        va_start(ap, cmd);
        vprintf(cmd, ap);
        std::cout << std::endl;
#endif
        va_end(ap);
    }

    inline redisReply* reply()
    {
        return __reply;
    }
    inline void freeReply()
    {
        freeReplyObject(__reply);
    }
    inline std::string& instanceID()
    {
        return __instanceid;
    }
    inline std::string& sessionID()
    {
        return __session;
    }
    inline std::string& authToken()
    {
        return __auth_token;
    }
    inline void setSessionID(
    const std::string&& sid) {__session = sid;}
    inline void setSessionID (const std::string& sid) {__session = sid;}
    inline void setAuthToken (const std::string& ath) {__auth_token = ath;}
    inline redisContext* DBConnection() {return __redis;}
    inline ApplicationManager& getManager() {return manager;}
    inline Security::UserManager& getUserManager() {return userManager;}
    inline std::map<std::string,std::string>& getSlots() {return __slots;}
    inline void setGroup(const std::string& group) {__group = group;}
    inline void setGroup(const std::string&& group) {__group = group;}
    inline std::string& getGroup() {return __group;}
    inline const MoldableFactory& getFactory() { return moldableFactory; }
    };

#endif /* PREVIEW_H_ */
