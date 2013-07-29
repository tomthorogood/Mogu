#include "NodeEditor.h"
#include <Mogu.h>
#include <Security/Encryption.h>
namespace Redis {

NodeEditor::NodeEditor()
    : db(Prefix::widgets)
{
}

NodeEditor::NodeEditor(
    const Prefix& prefix_
    , const std::string& node_
    , NodeValue* arg_)
    :
        prefix(prefix_)
        , db(prefix_)
        , arg(arg_)
        , prefix_str(prefixMap.at(prefix_))
        , node(node_)
        
{
    setId(); // This has to be the first thing we do!
    /* Determine whether the node exists */
    appendCommand("exists");

    /* Get the raw type of the node.*/
    appendCommand("type");
    exists = db.yieldResponse <bool>();
    if (exists)
        type = MoguSyntax::get(db.yieldResponse<std::string>()).integer;
    /* Don't try and guess the type of a node that doesn't exist. */
    else if (prefix == Prefix::user || prefix == Prefix::group)
    {
        type = getPolicyType();
    }
    else
    {
        db.clear();
    }

    if (arg) setArgInfo();

}

inline int NodeEditor::getPolicyType()
{
    NodeValue tmp(MoguSyntax::type.integer);
    swapArg(&tmp);

    Prefix prefix_ = prefix;
    setPrefix(Prefix::policies);
    appendCommand("hget");
    int type_ = MoguSyntax::get(db.yieldResponse<std::string>()).integer;

    setPrefix(prefix_);
    swapArg();
    return type_;
}

/* Resets anything that could potentially change
 * by the changing of a prefix, changes the prefix.
 */
void NodeEditor::setPrefix(Prefix prefix_)
{
    if (prefix_ == prefix) return; // Ignore this request
    prefix = prefix_;
    id = -1;
    setId();
    prefix_str = prefixMap.at(prefix);
    db.setPrefix(prefix);
    unsetArgInfo();

    appendCommand("exists");
    appendCommand("type");
    exists = db.yieldResponse<bool>();
    if (exists) type = MoguSyntax::get(db.yieldResponse<std::string>()).integer;
    else db.clear();

    setArgInfo();
}


/* A standard read command will be either 
 * 'get', 'lindex', or 'hget'
 */
std::string NodeEditor::read()
{
    if (!exists) 
    {
        if (prefix != Prefix::user && prefix != Prefix::group)
        {
            return EMPTY;
        }
        else
        {
            /* If the node doesn't exist, we can try and see if there is
             * a default set in the node's policy.
             */
            return getDefault();
        }
    }
    if (type == MoguSyntax::string.integer)
    {
        unsetArgInfo();
        appendCommand("get");
        setArgInfo();
    }
    else if (type == MoguSyntax::list.integer)
    {
        if (isdigit(arg_str[0]))
            appendCommand("lindex");
    }
    else
    {
        if (!isEmpty(arg_str))
            appendCommand("hget");
    }

    if (encrypted)
        return Security::decrypt(db.yieldResponse<std::string>());
    return db.yieldResponse <std::string>();
}

void NodeEditor::read(std::map<std::string,std::string>& iomap)
{
    /* This is only a valid action when we're dealing with a hash node. */
    if (type != MoguSyntax::hash.integer && exists) return;

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
            getDefault(iomap);
            return;
        }
    }

    /* We have to clear arg info in order for an 'hgetall' command to work. */
    unsetArgInfo();
    appendCommand("hgetall");
    std::vector <std::string> response =
        db.yieldResponse <std::vector <std::string>>();
    
    for (size_t index = 0; index < response.size(); ++index)
    {
        std::string key = response[index];
        std::string val = response[++index];
        if (encrypted)
            val = Security::decrypt(val);
        iomap[key] = val;
    }

    setArgInfo();
}

void NodeEditor::read(std::vector <std::string>& iovec)
{
    std::string full_node = buildNode();
    const char* local_node = full_node.c_str();
    if (exists && type != MoguSyntax::list.integer) return;
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
            getDefault(iovec);
            return;
        }
    }
    appendCommand("llen");
    int range = db.yieldResponse <int>();
    
    // Since this is a very specific command format, we will 
    // not worry about building this logic into the 'appendCommand'
    // tree.

    db.appendQuery("lrange %s 0 %d", local_node, range);

    iovec = db.yieldResponse<std::vector<std::string>>();
    if (encrypted)
    {
        for (size_t i = 0; i < iovec.size(); ++i)
        {
            std::string decrypted = Security::decrypt(iovec[i]);
            iovec[i] = decrypted;
        }
    }
}

std::string NodeEditor::getDefault()
{
    NodeValue nv(MoguSyntax::default_.str);
    NodeEditor def(Prefix::policies, node, arg);
    def.setSub(MoguSyntax::default_.str);
    return def.read();
}

void NodeEditor::getDefault(std::vector<std::string>& iovec)
{
    std::string node(node);
    NodeEditor editor(Prefix::policies, node, arg);
    editor.setSub(MoguSyntax::default_.str);
    editor.read(iovec);
}

void NodeEditor::getDefault(std::map<std::string,std::string>& iomap)
{
    NodeEditor editor(Prefix::policies, node, arg);
    editor.setSub(MoguSyntax::default_.str);
    editor.read(iomap);
}


bool NodeEditor::write(std::string value)
{
    if (encrypted) value = Security::encrypt(value);
    NodeValue tmp(value);
    if (type==MoguSyntax::string.integer)
    {

        // We shouldn't already have an arg, and if we do, it's 
        // probably not there on purpose, so we'll allow the overwrite.
        swapArg(&tmp);
        appendCommand("set");
        swapArg();

    }
    else if (type==MoguSyntax::list.integer)
    {
        swapArg(&tmp);
        appendCommand("rpush");
        swapArg();
    }
    /* This is a special circumstance, so we'll build the logic right here. 
     * The 'arg' should be the key in the Redis hash.
     */
    else if (type==MoguSyntax::hash.integer && !arg_str.empty())
    {
        std::string s_node = buildNode();
        db.appendQuery("hset %s %s %s",
            s_node.c_str(), arg_str.c_str(), value.c_str());
    }
    if (!delay_execution)
        db.execute();
    return true;
}

bool NodeEditor::write(std::map<std::string,std::string>& iomap)
{
    if (encrypted)
    {
        for (auto iter : iomap)
        {
            std::string val = Security::encrypt(iter.second);
            iomap[iter.first] = val;
        }
    }
    /* Here, we are necessarily dealing with a hash node.*/
    if (type != MoguSyntax::hash.integer) return false;
    
    NodeValue tmp;
    delay_execution = true;

    for (auto iter : iomap)
    {
        tmp.setString(iter.first); // Set the key as the arg.
        swapArg(&tmp);
        write(iter.second);
        swapArg();
    }

    db.execute();
    delay_execution = false;
    return true;
}

/* Here, we are necessarily dealing with a list node. */
bool NodeEditor::write(std::vector<std::string>& iovec)
{
    if (encrypted)
    {
        for (size_t i =0; i < iovec.size(); ++i)
        {
            std::string str = Security::encrypt(iovec[i]);
            iovec[i] = str;
        }
    }
    if (type != MoguSyntax::list.integer) return false;

    /* Write everything as a batch */
    delay_execution = true;
    for (std::string str : iovec)
    {
        write(str);
    }
    db.execute();
    delay_execution = false;
    return true;
}

bool NodeEditor::remove()
{
    if (arg == NULL)
    {
        appendCommand("del");
    }
    else if (type == MoguSyntax::hash.integer)
    {
        appendCommand("hdel");
    }
    else return false;
    return true;
}

bool NodeEditor::remove(const std::string& value)
{
    std::string s_node = buildNode();
    if (type == MoguSyntax::list.integer)
    {
        db.appendQuery("lrem %s 1 %s", s_node.c_str(), value.c_str());
        return true;
    }
    else return false;

}

void NodeEditor::appendCommand(const char* cmd)
{

    std::string s_node = buildNode();
    const char* full_node = s_node.c_str();
#ifdef DEBUG
    static int iter = 0;
    ++iter;
    std::cout << "Performing Redis request #" << iter << " with command " <<
        cmd << " " << full_node << "(arg: " << arg_str << ")" << std::endl;
#endif
    if (!arg_str.empty())
    {
        db.appendQuery("%s %s %s", cmd, full_node, arg_str.c_str());
    }
    else
    {
        db.appendQuery("%s %s", cmd, full_node);
    }
}

void NodeEditor::setArgInfo()
{
    if (arg == NULL) return;

    /* Lists args cannot be strings, and therefore we can safely 
     * assume that's what we'll be dealing with for a dash of type safety.
     */

    if (arg->isString())
    {
        arg_str = arg->getString();
    }
    else if (arg->isInt())
    {
        std::string s = std::to_string(arg->getInt());
        arg_str = s;
    }
}


void NodeEditor::setId()
{
    mApp;
    if (prefix == Prefix::group)
    {
        id = app->getGroup();
    }
    else if (prefix == Prefix::user)
    {
        id = app->getUser();
    }
}

}//namespace Redis
