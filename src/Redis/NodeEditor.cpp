#include "NodeEditor.h"
#include "../Security/Encryption.h"
#include "../Types/MoguLogger.h"
namespace Redis {

NodeEditor::NodeEditor()
    : db(new MoguQueryHandler(Application::contextMap, Prefix::widgets))
{
}

NodeEditor::NodeEditor(

    const Prefix& prefix_
    , const std::string& node_
    , NodeValue* arg_)
    :
        prefix(prefix_)
        , db(new MoguQueryHandler(Application::contextMap, prefix_))
        , arg(arg_)
        , prefix_str(prefixMap.at(prefix_))
        , node(node_)
        
{
    setup();
}

void NodeEditor::setup()
{
    if (id_required() && !hasId()) return;
    db->appendQuery(buildCommand("exists"));

    /* Get the raw type of the node.*/
    db->appendQuery(buildCommand("type"));
    exists = db->yieldResponse <bool>();
    if (exists)
        type = MoguSyntax::get(db->yieldResponse<std::string>()).integer;
    /* Don't try and guess the type of a node that doesn't exist. */
    else if (prefix == Prefix::user || prefix == Prefix::group)
    {
        type = getPolicyType();
    }
    else
    {
        db->flush();
    }

    if (arg) setArgInfo();
    is_setup = true;
}


int NodeEditor::getPolicyType()
{
    NodeValue tmp(MoguSyntax::type.integer);
    swapArg(&tmp);

    Prefix prefix_ = prefix;
    setPrefix(Prefix::policies);
    db->appendQuery(buildCommand("hget"));
    int type_ = MoguSyntax::get(db->yieldResponse<std::string>()).integer;

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
    prefix_str = prefixMap.at(prefix);
    db->newContext(Application::contextMap->get(prefix_));
    unsetArgInfo();

    db->appendQuery(buildCommand("exists"));
    db->appendQuery(buildCommand("type"));
    exists = db->yieldResponse<bool>();
    if (exists) type = MoguSyntax::get(db->yieldResponse<std::string>()).integer;
    else db->flush();
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
            return "";
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
        db->appendQuery(buildCommand("get"));
        setArgInfo();
    }
    else if (type == MoguSyntax::list.integer)
    {
        if (isdigit(arg_str[0]))
            db->appendQuery(buildCommand("lindex"));
    }
    else
    {
        if (arg_str.empty())
            db->appendQuery(buildCommand("hget"));
    }

    if (encrypted)
        return Security::decrypt(db->yieldResponse<std::string>());
    return db->yieldResponse <std::string>();
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
    db->appendQuery(buildCommand("hgetall"));
    std::vector <std::string> response =
        db->yieldResponse <std::vector <std::string>>();
    
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
    db->appendQuery(buildCommand("llen"));
    int range = db->yieldResponse <int>();
    
    // Since this is a very specific command format, we will 
    // not worry about building this logic into the 'appendCommand'
    // tree.

    db->appendQuery("lrange %s 0 %d", full_node.c_str(), range);

    iovec = db->yieldResponse<std::vector<std::string>>();
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


bool NodeEditor::write(std::string value, bool hold_queue)
{
    if (encrypted) value = Security::encrypt(value);
    NodeValue tmp(value);
    if (type==MoguSyntax::string.integer)
    {
        // We shouldn't already have an arg, and if we do, it's 
        // probably not there on purpose, so we'll allow the overwrite.
        swapArg(&tmp);
        db->appendQuery(buildCommand("set"));
        swapArg();
    }
    else if (type==MoguSyntax::list.integer)
    {
        swapArg(&tmp);
        db->appendQuery(buildCommand("rpush"));
        swapArg();
    }
    /* This is a special circumstance, so we'll build the logic right here. 
     * The 'arg' should be the key in the Redis hash.
     */
    else if (type==MoguSyntax::hash.integer && !arg_str.empty())
    {
        db->appendQuery(buildCommand("hset",value));
    }
    if (!hold_queue) db->flush();
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
        write(iter.second, true);
        swapArg();
    }
    db->flush();

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
        write(str,true);
    }
    db->flush();
    return true;
}

bool NodeEditor::remove()
{
    if (arg == NULL)
    {
        db->appendQuery(buildCommand("del"));
    }
    else if (type == MoguSyntax::hash.integer)
    {
        db->appendQuery(buildCommand("hdel"));
    }
    else return false;
    return true;
}

bool NodeEditor::remove(const std::string& value)
{
    std::string s_node = buildNode();
    if (type == MoguSyntax::list.integer)
    {
        db->appendQuery("lrem %s 1 %s", s_node.c_str(), value.c_str());
        return true;
    }
    else return false;

}

std::string NodeEditor::buildCommand(const std::string& cmd, std::string extra)
{
    std::string s_node = buildNode();
    std::stringstream cmdbuf;
    cmdbuf << cmd << " " << s_node;
    if (!arg_str.empty()) cmdbuf << " " << arg_str;
    if (!extra.empty()) cmdbuf << " " << extra;
    std::string scmd = cmdbuf.str();
    Application::log.log(LogLevel::NOTICE, "Built Redis command: ", scmd);
    return scmd;
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


}//namespace Redis
