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
    : prefix(prefix_), c_node(node_.c_str()), arg(arg_), db(prefix_),
        c_prefix(prefixMap.at(prefix_).c_str())
{
    setId(); // This has to be the first thing we do!
    /* Determine whether the node exists */
    appendCommand("exists");

    /* Get the raw type of the node.*/
    appendCommand("type");
    exists = db.yieldResponse <bool>();
    if (exists) type = MoguSyntax::get(db.yieldResponse<std::string>()).integer;
    /* Don't try and guess the type of a node that doesn't exist. */
    else db.clear();

    if (arg) setArgInfo();

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
    c_prefix = prefixMap.at(prefix).c_str();
    db.setPrefix(prefix);
    NodeValue* arg_ = arg;
    unset_arg();

    appendCommand("exists");
    appendCommand("type");
    exists = db.yieldResponse<bool>();
    if (exists) type = MoguSyntax::get(db.yieldResponse<std::string>()).integer;
    else db.clear();

    if (arg_ && exists)
    {
        arg = arg_;
        setArgInfo();
    }
}


/* A standard read command will be either 
 * 'get', 'lindex', or 'hget'
 */
std::string NodeEditor::read()
{
    if (!exists) 
    {
        if (prefix != Prefix::user || prefix != Prefix::group)
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
        if (isEmpty(c_hashkey) && list_index == -1)
            appendCommand("get");
    }
    else if (type == MoguSyntax::list.integer)
    {
        if (list_index != -1)
            appendCommand("lindex");
    }
    else
    {
        if (!isEmpty(c_hashkey))
            appendCommand("hget");
    }

    if (encrypted)
        return Security::decrypt(db.yieldResponse<std::string>());
    return db.yieldResponse <std::string>();
}

void NodeEditor::read(std::map<std::string,std::string>& iomap)
{
    if (type != MoguSyntax::hash.integer) return;
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
            return getDefault(iomap);
        }
    }

    /* We have to clear arg info in order for an 'hgetall' command to work. */
    NodeValue* arg_ = arg;
    unset_arg();
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

    /* Put arg info back the way we found it. */
    arg = arg_;
    setArgInfo();
}

void NodeEditor::read(std::vector <std::string>& iovec)
{
    if (type != MoguSyntax::list.integer) return;
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
            return getDefault(iovec);
        }
    }
    NodeValue* arg_ = arg;
    unset_arg();
    appendCommand("llen");
    int range = db.yieldResponse <int>();
    
    // Since this is a very specific command format, we will 
    // not worry about building this logic into the 'appendCommand'
    // tree.
    if (hasId())
    {
        if (!isEmpty(c_sub))
        {
            db.appendQuery("lrange %s.%d.%s.%s 0 %d",
                c_prefix, id, c_node, c_sub, range);
        }
        else
        {
            db.appendQuery("lrange %s.%d.%s 0 %d",
                c_prefix, id, c_node, range);
        }
    }
    else
    {
        if (!isEmpty(c_sub))
        {
            db.appendQuery("lrange %s.%s.%s 0 %d",
                c_prefix, c_node, c_sub, range);
        }
        else
        {
            db.appendQuery("lrange %s.%s 0 %d",
                c_prefix, c_node, range);
        }
    }
    iovec = db.yieldResponse<std::vector<std::string>>();
    if (encrypted)
    {
        for (size_t i = 0; i < iovec.size(); ++i)
        {
            std::string decrypted = Security::decrypt(iovec[i]);
            iovec[i] = decrypted;
        }
    }

    arg = arg_;
    setArgInfo();
}

std::string NodeEditor::getDefault()
{
    std::string node(c_node);
    NodeEditor editor(Prefix::policies, node, arg);
    editor.setSub("default");
    return editor.read();
}

void NodeEditor::getDefault(std::vector<std::string>& iovec)
{
    std::string node(c_node);
    NodeEditor editor(Prefix::policies, node, arg);
    editor.setSub("default");
    editor.read(iovec);
}

void NodeEditor::getDefault(std::map<std::string,std::string>& iomap)
{
    std::string node(c_node);
    NodeEditor editor(Prefix::policies, node, arg);
    editor.setSub("default");
    editor.read(iomap);
}


bool NodeEditor::write(std::string value)
{
    if (encrypted)
        value = Security::encrypt(value);
    NodeValue tmp(value);
    if (type==MoguSyntax::string.integer)
    {

        // We shouldn't already have an arg, and if we do, it's 
        // probably not there on purpose, so we'll allow the overwrite.
        arg = &tmp;
        setArgInfo();
        appendCommand("set");
        unset_arg();
    }
    else if (type==MoguSyntax::list.integer)
    {
        NodeValue* arg_ = arg;
        unset_arg();
        arg = &tmp;
        setArgInfo();
        appendCommand("rpush");
        arg = arg_;
        setArgInfo();
    }
    /* This is a special circumstance, so we'll build the logic right here. 
     * The 'arg' should be the key in the Redis hash.
     */
    else if (type==MoguSyntax::hash.integer && !isEmpty(c_hashkey))
    {
        if (hasId())
        {
            if (!isEmpty(c_sub))
            {
                db.appendQuery("hset %s.%d.%s.%s %s %s",
                    c_prefix, id, c_node, c_sub, c_hashkey, value.c_str());
            }
            else
            {
                db.appendQuery("hset %s.%d.%s %s %s",
                    c_prefix, id, c_node, c_hashkey, value.c_str());
            }
        }
        else
        {
            if (!isEmpty(c_sub))
            {
                db.appendQuery("hset %s.%s.%s %s %s",
                    c_prefix, c_node, c_sub, c_hashkey, value.c_str());
            }
            else
            {
                db.appendQuery("hset %s.%s %s %s",
                    c_prefix, c_node, c_hashkey, value.c_str());
            }
        }
    }
    else
    {
        return false;
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
    NodeValue* arg_ = arg;
    NodeValue tmp;
    arg = &tmp;
    unset_arg();
    delay_execution = true;

    for (auto iter : iomap)
    {
        tmp.setString(iter.first); // Set the key as the arg.
        setArgInfo();
        write(iter.second);
    }

    db.execute();
    arg = arg_;
    setArgInfo();
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
    delay_execution = true;
    for (auto str : iovec)
    {
        write(str);
    }
    db.execute();
    delay_execution = false;
    return true;
}

bool NodeEditor::remove()
{
    if (!arg)
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
    if (type == MoguSyntax::list.integer)
    {
        if (hasId())
        {
            if (!isEmpty(c_sub))
            {
                db.appendQuery("lrem %s.%d.%s.%s 1 %s",
                    c_prefix, id, c_node, c_sub, value.c_str());
            }
            else
            {
                db.appendQuery("lrem %s.%d.%s 1 %s",
                    c_prefix, id, c_node, value.c_str());
            }
        }
        else
        {
            if (!isEmpty(c_sub))
            {
                db.appendQuery("lrem %s.%s.%s 1 %s",
                    c_prefix, c_node, c_sub, value.c_str());
            }
            else
            {
                db.appendQuery("lrem %s.%s 1 %s",
                    c_prefix, c_node, value.c_str());
            }
        }
        return true;
    }
    else return false;

}

void NodeEditor::appendCommand(const char* cmd)
{
    if (hasId())
    {
        if (!isEmpty(c_sub))
        {
            if (list_index != -1)
            {
                db.appendQuery("%s %s.%d.%s.%s %d",
                    cmd, c_prefix, id, c_node, c_sub, list_index);
            }
            else if (!isEmpty(c_hashkey))
            {
                db.appendQuery("%s %s.%d.%s.%s %s",
                    cmd, c_prefix, id, c_node, c_sub, c_hashkey);
            }
            else
            {
                db.appendQuery("%s %s.%d.%s.%s",
                    cmd, c_prefix, id, c_node, c_sub);
            }
        }
        else
        {
            if (list_index != -1)
            {
                db.appendQuery("%s %s.%d.%s %d",
                    cmd, c_prefix, id, c_node, list_index);
            }
            else if (!isEmpty(c_hashkey))
            {
                db.appendQuery("%s %s.%d.%s %s",
                    cmd, c_prefix, id, c_node, c_hashkey);
            }
            else
            {
                db.appendQuery("%s %s.%d.%s",
                    cmd, c_prefix, id, c_node);
            }
        }
    }
    else
    {
        if (!isEmpty(c_sub))
        {
            if (list_index != -1)
            {
                db.appendQuery("%s %s.%s.%s %d",
                    cmd, c_prefix, c_node, c_sub, list_index);
            }
            else if (!isEmpty(c_hashkey))
            {
                db.appendQuery("%s %s.%s.%s %s",
                    cmd, c_prefix, c_node, c_sub, c_hashkey);
            }
            else
            {
                db.appendQuery("%s %s.%s.%s",
                    cmd, c_prefix, c_node, c_sub);
            }
        }
        else
        {
            if (list_index != -1)
            {
                db.appendQuery("%s %s.%s %d",
                    cmd, c_prefix, c_node, list_index);
            }
            else if (!isEmpty(c_hashkey))
            {
                db.appendQuery("%s %s.%s %s",
                    cmd, c_prefix, c_node, c_hashkey);
            }
            else
            {
                db.appendQuery("%s %s.%s",
                    cmd, c_prefix, c_node);
            }
        }
    }
}

void NodeEditor::setArgInfo()
{
    if (arg == nullptr) return;
    if (type == MoguSyntax::hash.integer)
    {
        list_index = -1;
        if (arg->isString())
        {
            c_hashkey = arg->getString().c_str();
        }
        else if (arg->isInt())
        {
            std::string s = std::to_string(arg->getInt());
            c_hashkey = s.c_str();
        }
    }
    else if (type == MoguSyntax::list.integer)
    {
        c_hashkey = EMPTY;
        if (arg->isInt())
        {
            list_index = arg->getInt();
        }
        else if (arg->isString())
        {
            list_index = std::atoi(arg->getString().c_str());
        }
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
