#ifndef NODEDITOR_H_
#define NODEDITOR_H_

#include "MoguQueryHandler.h"
#include "DatabaseConfigReader.h"
#include "../Types/NodeValue.h"
#include "../Types/SyntaxDef.h"
#include "../Types/syntax.h"
#include <cassert>
#include <string>
#include <vector>
#include <map>


namespace Redis
{

class Node_Editor
{
public:
    Node_Editor();
    Node_Editor(
        const Prefix& prefix, const std::string& node, Node_Value* arg=nullptr);

    void set_prefix(Prefix);

    ~Node_Editor()
    {
        if (db)
        { 
            db->flush();
            delete db;
        }
        if (policy)
        {
            policy->flush();
            delete policy;
        }
    }

    inline void set_sub(const std::string& sub_)
    {
        sub = sub_;
        set_exists();
        set_type();
    }

    inline void clear_sub()
    {
        sub = "";
        set_exists();
        set_type();
    }

    int get_policy_type();

    /* Read a single value */
    std::string read();

    /* Read a list of values */
    void read(std::vector<std::string>&);

    /* Read a hash node */
    void read(std::map<std::string,std::string>&);

    /* Write a single value at the node. */
    bool write(std::string, bool hold_for_multiple_writes=false);

    /* Write an entire hash key */
    bool write(std::map <std::string,std::string>&);

    /* Write an entire list */
    bool write(std::vector<std::string>&);

    /* Deletes a node. */
    bool remove();

    /* Deletes either a list value or hash key */
    bool remove(const std::string& val);

    /* Readability function */
    inline bool id_required()
    { return (prefix==Prefix::user)||(prefix==Prefix::group);}

    inline void set_id(int id_) {
        if (has_id()) return;
        id = id_; setup();
    }

    bool set_encrypted();

    inline void set_node(const std::string& node_)
    { node = node_;}
    
    inline const Syntax_Def& policy_type()
    {
        set_policy();
        policy->append_query(
            "hget policies.%s %d", node.c_str(), Mogu_Syntax::type.integer);
        return Mogu_Syntax::get(policy->yield_response<std::string>());
    }

    inline void unset_arg_info()
    {
        arg_str = "";
    }

    /* Set the arg to a new arg. The old arg is lost. */
    inline void set_arg(Node_Value* arg_)
    {
        unset_arg_info();
        arg = arg_;
        set_arg_info();
    }

    /* When given a new argument to swap in, the old
     * argument is put into 'arg_storage', where it can be retrieved
     * at a later time.
     */
    inline void swap_arg (Node_Value* arg_)
    {
        unset_arg_info();
        arg_storage = arg;
        arg = arg_;
        set_arg_info();
    }

    /* When given no new argument, the argument in storage will be
     * swapped in instead. */
    inline void swap_arg()
    {
        unset_arg_info();
        arg = arg_storage;
        arg_storage = NULL;
        set_arg_info();
    }


    inline const Syntax_Def& get_type() const
    {
        return Mogu_Syntax::get(type);
    }

    inline bool node_exists() const { return exists; }

private:

    Prefix prefix {Prefix::__NONE__};
   
    Mogu_Query_Handler* db {};
    
    /* For user and group nodes, we'll need to read the node policy as well.*/
    Mogu_Query_Handler* policy {};
    Node_Value* arg {};
    Node_Value* arg_storage {};

    bool exists {false};
    bool hash_exists {false};
    bool is_setup {false};
    int type {Mogu_Syntax::__NONE__.integer};
    
    /*  The string representation of the Prefix given. */
    std::string prefix_str {};

    /* The [name] in the [prefix].[id?].[name]... format. */
    std::string node {};

    /* If applicable, the subnode for the prefix:
     * [prefix].[id?].[name].[sub]
     */
    std::string sub {};

    /* If applicable, the hashkey used when trying to access
     * a single value in a hash node. If this is attempted but the
     * hashkey is empty, the read function will return false.
     */
    std::string arg_str {};

    /* The id used to read user or group nodes */
    int id {-1};

    /* Whether or not the content of this node is encrypted, and therefore 
     * must be decrypted. Only applies to user/group data.
     */
    bool encrypted {false};

    /* When writing a lot of things at once, we'll set this to true in order
     * to speed things up and write them all in a batch, instead of
     * individual queries. 
     */
    bool delay_execution {false};


    /* In the case of user and group nodes that have no value, we need
     * to check and see if the node policy lists a default value to use instead.
     * If so, we'll retrieve that value.
     */
    std::string get_default();
    void set_arg_info();
   


    /* Same as above but for default hashes. */
    void get_default(std::map<std::string,std::string>&);

    /* Same as above but for default lists. */
    void get_default(std::vector<std::string>&);
    std::string build_command(const std::string& cmd, std::string extra="");



    inline bool has_id() 
    { return id != -1; }

    inline void set_policy()
    {
        if (!policy) policy = new Redis::Mogu_Query_Handler(Prefix::policies);
    }

    inline void set_exists()
    {
        unset_arg_info();
        db->append_query(build_command("exists").c_str());
        exists = db->yield_response<bool>();
        set_arg_info();
    }

    inline void set_type()
    {
        unset_arg_info();
        db->append_query(build_command("type"));
        type = Mogu_Syntax::get(db->yield_response<std::string>()).integer;
        set_arg_info();
    }

    inline std::string build_node()
    {
        std::stringstream buf;
        buf << prefix_str;

        if (has_id())
        {
            buf << "." << id;
        }

        buf << "." << node.c_str();

        if (!sub.empty())
        {
            buf << "." << sub;
        }

        return buf.str();
    }

    void setup();

};//class Node_Editor


}//namespace Redis

#endif
