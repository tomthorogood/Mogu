#include "NodeEditor.h"
#include "../Security/Encryption.h"
#include "DatabaseConfigReader.h"
#include "../Types/MoguLogger.h"

namespace Application{
    extern Mogu_Logger log;
}

namespace Redis {

Node_Editor::Node_Editor()
    : db(new Mogu_Query_Handler(Prefix::widgets))
{
}

Node_Editor::Node_Editor(
        const Prefix& prefix_, const std::string& node_, Node_Value* arg_)
    : prefix(prefix_)
    , db(new Mogu_Query_Handler(prefix_))
    , arg(arg_)
    , prefix_str(prefix_map().at(prefix_))
    , node(node_)
        
{
    setup();
}

void Node_Editor::setup()
{
    if (id_required() && !has_id()) return;
    db->append_query(build_command("exists"));

    /* Get the raw type of the node.*/
    db->append_query(build_command("type"));
    exists = db->yield_response <bool>();
    if (exists)
        type = Mogu_Syntax::get(db->yield_response<std::string>()).integer;
    /* Don't try and guess the type of a node that doesn't exist. */
    else
    {
        db->flush();
    }

    if (prefix == Prefix::user || prefix == Prefix::group)
    {
        type = get_policy_type();
        set_encrypted();
    }

    if (arg) set_arg_info();
    is_setup = true;
}


int Node_Editor::get_policy_type()
{
    Node_Value tmp(Mogu_Syntax::type.integer);
    set_policy();
    policy->append_query(
        "hget policies.%s %d", node.c_str(), Mogu_Syntax::type.integer);
    int type_ = Mogu_Syntax::get(policy->yield_response<std::string>());
    return type_;
}

/* Resets anything that could potentially change
 * by the changing of a prefix, changes the prefix.
 */
void Node_Editor::set_prefix(Prefix prefix_)
{
    if (prefix_ == prefix) return; // Ignore this request
    prefix = prefix_;
    id = -1;
    prefix_str = prefix_map().at(prefix);
    db->new_context(Application::context_map()->get(prefix_));
    unset_arg_info();

    db->append_query(build_command("exists"));
    db->append_query(build_command("type"));
    exists = db->yield_response<bool>();
    if (exists)
    {
        type = Mogu_Syntax::get(db->yield_response<std::string>()).integer;
    }
    else
    {
        db->flush();
    }
    set_arg_info();
}


/* A standard read command will be either 
 * 'get', 'lindex', or 'hget'
 */
std::string Node_Editor::read()
{
    if (!exists)
    {
        if (prefix != Prefix::user && prefix != Prefix::group)
        {
            return "";
        }
        else
        {
            /* If the node doesn't exist, we can try and see if there is
             * a default set in the node's policy.
             */
            return get_default();
        }
    }
    if (type == Mogu_Syntax::string.integer)
    {
        unset_arg_info();
        db->append_query(build_command("get"));
        set_arg_info();
    }
    else if (type == Mogu_Syntax::list.integer)
    {
        if (isdigit(arg_str[0]))
            db->append_query(build_command("lindex"));
    }
    else
    {
        if (!arg_str.empty())
        {
            db->flush();
            db->append_query(build_command("hexists"));
            bool response {db->yield_response<bool>()};
            if (response)
                db->append_query(build_command("hget"));
            else if (is_dynamic_type())
                return get_default();
            else return "";
        }
    }

    if (encrypted)
        return Security::decrypt(db->yield_response<std::string>());
    return db->yield_response <std::string>();
}

bool Node_Editor::set_encrypted()
{
    set_policy();
    policy->append_query(
        "hget policies.%s %d", node.c_str(), Mogu_Syntax::encrypted.integer);
    std::string encryption_result {policy->yield_response <std::string>()};
    encrypted = encryption_result=="yes";
    return encrypted;
}


void Node_Editor::read(std::map<std::string,std::string>& iomap)
{
    /* This is only a valid action when we're dealing with a hash node. */
    if (type != Mogu_Syntax::hash.integer && exists) return;

    /* If it doesn't exist, we can check to see if there is a default set, 
     * which will only be the case in a user or group prefix. */
    else if (!exists) 
    {
        if (prefix != Prefix::user || prefix != Prefix::group)
        {
            return;
        }
        else
        {
            get_default(iomap);
            return;
        }
    }

    /* We have to clear arg info in order for an 'hgetall' command to work. */
    unset_arg_info();
    db->append_query(build_command("hgetall"));
    std::vector <std::string> response =
        db->yield_response <std::vector <std::string>>();
    
    for (size_t index = 0; index < response.size(); ++index)
    {
        std::string key {response[index]};
        std::string val {response[++index]};
        if (encrypted)
            val = Security::decrypt(val);
        iomap[key] = val;
    }

    set_arg_info();
}

void Node_Editor::read(std::vector <std::string>& iovec)
{
    std::string full_node {build_node()};
    if (exists && get_type() != Mogu_Syntax::list) return;
    if (!exists) 
    {
        if (prefix != Prefix::user || prefix != Prefix::group)
        {
            return;
        }
        else
        {
            /* If the node doesn't exist, we can try and see if there is
             * a default set in the node's policy.
             */
            get_default(iovec);
            return;
        }
    }
    db->append_query(build_command("llen"));
    int range {db->yield_response <int>()};
    
    // Since this is a very specific command format, we will 
    // not worry about building this logic into the 'appendCommand'
    // tree.

    db->append_query("lrange %s 0 %d", full_node.c_str(), range);

    iovec = db->yield_response<std::vector<std::string>>();
    if (encrypted)
    {
        for (size_t i = 0; i < iovec.size(); ++i)
        {
            std::string decrypted {Security::decrypt(iovec[i])};
            iovec[i] = decrypted;
        }
    }
}

std::string Node_Editor::get_default()
{
    Node_Value nv {Mogu_Syntax::default_.str};
    Node_Editor def {Prefix::policies, node, arg};
    def.set_sub(Mogu_Syntax::default_.str);
    return def.read();
}

void Node_Editor::get_default(std::vector<std::string>& iovec)
{
    std::string node {node};
    Node_Editor editor {Prefix::policies, node, arg};
    editor.set_sub(Mogu_Syntax::default_.str);
    editor.read(iovec);
}

void Node_Editor::get_default(std::map<std::string,std::string>& iomap)
{
    Node_Editor editor {Prefix::policies, node, arg};
    editor.set_sub(Mogu_Syntax::default_.str);
    editor.read(iomap);
}


bool Node_Editor::write(std::string value, bool hold_queue)
{
    if (encrypted) value = Security::encrypt(value);
    Node_Value tmp(value);
    if (type==Mogu_Syntax::string.integer)
    {
        // We shouldn't already have an arg, and if we do, it's 
        // probably not there on purpose, so we'll allow the overwrite.
        swap_arg(&tmp);
        db->append_query(build_command("set"));
        swap_arg();
    }
    else if (type==Mogu_Syntax::list.integer)
    {
        swap_arg(&tmp);
        db->append_query(build_command("rpush"));
        swap_arg();
    }
    /* This is a special circumstance, so we'll build the logic right here. 
     * The 'arg' should be the key in the Redis hash.
     */
    else if (type==Mogu_Syntax::hash.integer && !arg_str.empty())
    {
        db->append_query(build_command("hset","%s"),value.c_str());
    }
    /* If the node doesn't exist */
    else if (!arg)
    {
        swap_arg(&tmp);
        db->append_query(build_command("set"));
        swap_arg();
    }
    else
    {
        db->append_query(build_command("hset", "%s"), value.c_str());
    }
    if (!hold_queue) db->flush();
    exists = true;
    return true;
}

bool Node_Editor::write(std::map<std::string,std::string>& iomap)
{
    if (encrypted)
    {
        for (auto iter : iomap)
        {
            std::string s {Security::encrypt(iter.second)};
            iomap[iter.first] = s;
        }
    }
    /* Here, we are necessarily dealing with a hash node.*/
    if (type != Mogu_Syntax::hash.integer) return false;
    
    Node_Value tmp {};
    delay_execution = true;

    for (auto iter : iomap)
    {
        tmp.set_string(iter.first); // Set the key as the arg.
        swap_arg(&tmp);
        write(iter.second, true);
        swap_arg();
    }
    db->flush();
    exists = true;
    return true;
}

/* Here, we are necessarily dealing with a list node. */
bool Node_Editor::write(std::vector<std::string>& iovec)
{
    if (encrypted)
    {
        for (size_t i =0; i < iovec.size(); ++i)
        {
            std::string str {Security::encrypt(iovec[i])};
            iovec[i] = str;
        }
    }
    if (type != Mogu_Syntax::list.integer)
        return false;

    /* Write everything as a batch */
    delay_execution = true;
    for (std::string str : iovec)
    {
        write(str,true);
    }
    db->flush();
    exists = true;
    return true;
}

bool Node_Editor::remove()
{
    if (!arg)
    {
        db->append_query(build_command("del"));
    }
    else if (type == Mogu_Syntax::hash.integer)
    {
        db->append_query(build_command("hdel"));
    }
    else return false;
    db->flush();
    return true;
}

bool Node_Editor::remove(const std::string& value)
{
    std::string s_node {build_node()};
    if (type == Mogu_Syntax::list.integer)
    {
        db->append_query("lrem %s 1 %s", s_node.c_str(), value.c_str());
        db->flush();
        return true;
    }
    else return false;

}

std::string Node_Editor::build_command(const std::string& cmd, std::string extra)
{
    std::string s_node {build_node()};
    std::stringstream cmdbuf;
    cmdbuf << cmd << " " << s_node;
    if (!arg_str.empty()) cmdbuf << " " << arg_str;
    if (!extra.empty()) cmdbuf << " " << extra;
    std::string scmd {cmdbuf.str()};
    Application::log.log(Log_Level::notice, "Built Redis command: ", scmd);
    return scmd;
}


void Node_Editor::set_arg_info()
{
    if (arg == NULL) return;

    /* Lists args cannot be strings, and therefore we can safely 
     * assume that's what we'll be dealing with for a dash of type safety.
     */

    if (arg->is_string())
    {
        arg_str = arg->get_string();
    }
    else if (arg->is_int())
    {
        std::string s {std::to_string(arg->get_int())};
        arg_str = s;
    }
}


}//namespace Redis
