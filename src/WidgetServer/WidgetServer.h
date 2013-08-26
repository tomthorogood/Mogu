#ifndef WIDGETSERVER_H_
#define WIDGETSERVER_H_

#include "WidgetServerInterface.h"
#include "DynamicServer.h"

/*\brief The Widget_Server acts as the intermediary between Redis and
 * the Mogu renderer, putting together a nice package to be passed into
 * the MoldableFactory, handling all the reslutions for widget templates and
 * foreach recursion.
 */
class Widget_Server : public Widget_Server_Interface
{
public:

    Widget_Server () {};
    virtual ~Widget_Server() {}

    /* This will always be the root widget of that which has been
     * requested.
     */
    virtual Widget_Assembly request(const std::string&, const int&, const int&);

    inline Dynamic_Server& dynamic_cache() { return dynamic; }

private:

    inline std::string get_attribute(
            Redis::Node_Editor* node, const Syntax_Def& attr)
    {
        Node_Value arg {attr.str};
        node->swap_arg(&arg);
        std::string attr_val = node->read();
        node->swap_arg();
        return attr_val;
    }

    Dynamic_Server dynamic {};
};

#endif
