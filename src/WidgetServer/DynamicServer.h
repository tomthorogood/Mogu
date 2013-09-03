/* DynamicServer.h
 * Source: DynamicServer.cpp
 *
 * Tom A. Thorogood
 * August 2013
 *
 */
#ifndef DYNAMIC_SERVER_H_
#define DYNAMIC_SERVER_H_

#include "WidgetServerInterface.h"
#include "../Types/KeyedAssembly.h"
#include "../Parsers/SourceDeclarationParser.h"
namespace Redis { class Node_Editor; }
class Source_Declaration_Parser;

class Dynamic_Server : public Widget_Server_Interface
{
    using Sortable = Keyed_Assembly<std::string>;

public:
    Dynamic_Server (){}
    virtual Widget_Assembly request (
        const std::string& node
        , const int& u=-1
        , const int& g=-1);

    inline void set_user_id(const int& i) { user_id = i;}
    inline void set_group_id(const int& i) { group_id = i;}

protected:

private:
    
    int user_id {};
    int group_id {};
    void sort(const Syntax_Def& dir, std::vector<Sortable>&);
    std::vector<std::string> fill_data_list(
        Redis::Node_Editor&, Source_Declaration_Parser&);
    Widget_Assembly spawn_anonymous_assembly(
        const std::string& data_point
        , const std::string& anon_tmpl
        , const Source_Declaration_Type &);
    Widget_Assembly spawn_anonymous_assembly(
        const std::string& data_point
        , const std::string& anon_tmpl
        , const int& alias_type);
    std::string get_template_name(const std::string&);
    std::string get_resolvable_string(const std::string&);
};


#endif //DYNAMIC_SERVER_H_
