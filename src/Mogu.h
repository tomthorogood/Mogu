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

#ifndef AUTH_TOKEN
#define AUTH_TOKEN "BendTheWeb"
#endif

#ifndef GLOBAL_SESSION
#define GLOBAL_SESSION "global"
#endif

class Mogu : public Wt::WApplication
{
	/*!\brief Changes the state of the application based on the URL */
	void handlePathChange(std::string path);

	/*!\brief A map of named widgets. */
	WidgetRegister widgetRegister;

	/*!\brief The widget that started it all... */
	Goo::Moldable* __wrapper;

public:

	Mogu(const Wt::WEnvironment& env);
	virtual ~Mogu();

	/*!\brief Returns whether or not a name represents a registered widget
	 * within the application instance.
	 * @param name The name of the widget being sought.
	 */
	inline bool widgetIsRegistered(std::string name)
	{
		WidgetRegister::iterator iter = widgetRegister.find(name);
		return iter != widgetRegister.end();
	}

	/*!\brief Adds a widget into the widget registry.
	 *
	 * @param name The name used for looking up the widget
	 * @param widget The pointer to the widget itself.
	 */
	inline void registerWidget(std::string name, Goo::Moldable* widget)
	{
		widgetRegister[name] = widget;
	}

	/*!\brief Returns a widget from the registry based on its name. */
	inline Goo::Moldable* registeredWidget(std::string name)
	{
		return widgetRegister[name];
	}

	/*!\brief Removes a widget from the registry. */
	inline void deregisterWidget(std::string name)
	{
		if (!widgetIsRegistered(name)) return;
		WidgetRegister::iterator iter = widgetRegister.find(name);
		widgetRegister.erase(iter);
	}

	inline void loadAnalytics(std::string id)
	{
		std::string script[] = {
				"var _gaq = _gaq || [];"
				,"_gaq.push(['_setAccount', '"
				,id
				,"']);"
				,"_gaq.push(['_setDomainName', 'gomogu.org']);"
				,"_gaq.push(['_trackPageview']);"
				,"(function() {"
				,"var ga = document.createElement('script');"
				,"ga.type = 'text/javascript'; ga.async = true;"
				,"ga.src = ('https:' == document.location.protocol ? "
				,"'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';"
				,"var s = document.getElementsByTagName('script')[0];"
				,"s.parentNode.insertBefore(ga, s);"
				,"})();"
		};
		int array_len = 14;
		std::string final = "";
		for (int i = 0; i < array_len; i++)
		{
			final += script[i];
		}
		doJavaScript(final);
	}
};


#endif /* PREVIEW_H_ */
