#ifndef WIDGETSERVER_H_
#define WIDGETSERVER_H_

#include <declarations.h>
#include <Redis/Node_Editor.h>
#include <Types/SyntaxDef.h>
#include <Types/Widget_Assembly.h>

/*\brief The Widget_Server acts as the intermediary between Redis and
 * the Mogu renderer, putting together a nice package to be passed into
 * the MoldableFactory, handling all the reslutions for widget templates and
 * foreach recursion.
 */
class Widget_Server 
{
public:
    typedef std::map <int,Node_Value> Attribute_Map;

    Widget_Server () {};
    ~Widget_Server();

    /* This will always be the root widget of that which has been
     * requested.
     */
    Widget_Assembly* request(const std::string& node);

private:
    int member_context {};

    Redis::Node_Editor* wnode {};
    Redis::Node_Editor* tnode {};
    Node_Value arg {};
    
    bool list_iter_complete {};

    std::string stateful_node {};
    std::string stateful_tmpl {};

    Widget_Assembly* assembly {};

    void populate_map(Redis::Node_Editor*);
    void setup_states();
    void resolve_iter_values();
    void resolve_values(std::map <int, Node_Value>&);
    void create_anonymous_widgets();
    void get_values_from_user_ids();
    void get_values_from_list_nodes();

    std::string find_member_call (std::map <int,Node_Value>&);
    inline std::string extract_node_name(const std::string& s)
    {
        size_t start = input.find("member")+7;
        size_t end = input.find_first_of(" \0", start);
        return s.substr(start,(end-start));
    }

    inline std::string get_attribute(
            Redis::Node_Editor* node, const Syntax_Def& attr)
    {
        Node_Value arg {attr.str};
        node->swap_arg(&arg);
        std::string attr_val = node->read();
        node->swap_arg();
        return attr_val;
    }

    int get_max_iters(const std::string& node_name);
};

#endif
