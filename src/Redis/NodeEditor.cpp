#include "NodeEditor.h"
#include <Mogu.h>
#include <Security/Encryption.h>
namespace Redis {

namespace {
    int ENCRYPTED_FIELDS = (int) Prefix::user | (int) Prefix::group;
}

NodeEditor::NodeEditor(
    Prefix p_prefix
    , const std::string& node_name
    , NodeValue* arg)
    : __arg(arg), __list_index(0)
{
    setPrefix(p_prefix);
    setNodeName(node_name);
    if ((int) p_prefix & ENCRYPTED_FIELDS)
        encrypted = fieldHasEncryption();
    else 
        encrypted = false;
    __id = getObjectId();
    __type = getNodeType();
    setKey();
    __db.clear();
}

NodeEditor::NodeEditor(
    Prefix p_prefix
    , const std::string& node_name
    , MoguSyntax node_type
    )
:   __arg(nullptr), __list_index(0)
{
    setPrefix(p_prefix);
    setNodeName(node_name);
    encrypted = fieldHasEncryption();
    __id = getObjectId();
    __type = node_type;
    setKey();
    __db.clear();
}

NodeEditor::NodeEditor(
        MoguSyntax m_prefix
        , const std::string& node_name
        , NodeValue* arg)
    :
        NodeEditor(syntax_to_prefix.at(m_prefix), node_name, arg)
{
}

void NodeEditor::setArg(NodeValue* arg)
{
    key_cached = false;
    __arg = arg;
    setKey();
}

void NodeEditor::setNodeName(const std::string& node)
{
    __node = node.c_str();
}

void NodeEditor::setPrefix(Prefix prefix)
{
    __prefix = prefix;
    __db.setPrefix(__prefix);
    std::string s_prefix = prefixMap.at(__prefix);
    c_prefix = s_prefix.c_str();
    id_required =
        (__prefix == Prefix::user || __prefix == Prefix::group);
}

void NodeEditor::readAll(std::map <std::string,std::string>& iomap)
{
    if (id_required)
        __db.appendQuery("hgetall %s.%d.%s", c_prefix, __id, __node);
    else
        __db.appendQuery("hgetall %s.%s", c_prefix, __node);

    std::vector <std::string> ovec =
        __db.yieldResponse <std::vector <std::string>>();

    // Since the output from hiredis will be in the form of
    //
    // { key0, val0, key1, val1, ... , keyn ,valn }
    // we can simply iterate through the vector, assigning responses as needed 
    // to the map.
    for (size_t i =0; i < ovec.size() ; ++i)
    {
        std::string key = ovec[i++];
        std::string val = ovec[i];
        iomap[key] = val;
    }
}

std::string NodeEditor::read()
{
    switch(__type)
    {
        case MoguSyntax::string:
            if (id_required)
                __db.appendQuery("get %s.%d.%s", c_prefix, __id, __node);
            else
                __db.appendQuery("get %s.%s", c_prefix, __node);
            break;
        case MoguSyntax::hash:
            if (id_required)
                __db.appendQuery(
                    "hget %s.%d.%s %s", c_prefix, __id, __node, __hashkey);
            else
                __db.appendQuery("hget %s.%s %s", c_prefix, __node, __hashkey);
            break;
        case MoguSyntax::list:
            if (!list_value) return EMPTY;
            if (id_required)
                __db.appendQuery(
                    "lindex %s.%d.%s %d",
                    c_prefix, __id, __node, __list_index);
            else
                __db.appendQuery(
                    "lindex %s.%s %d", c_prefix, __node, __list_index);
            break;
        default:return EMPTY;
    };

    if (encrypted) return Security::decrypt(__db.yieldResponse <std::string>());
    else return __db.yieldResponse <std::string>();
}

std::string NodeEditor::readSub(const std::string& sub)
{
    const char* c_sub = sub.c_str();

    switch(__subType)
    {
        case MoguSyntax::string:
            if (id_required)
                __db.appendQuery("get %s.%d.%s.%s", c_prefix, __id, __node, c_sub);
            else
                __db.appendQuery("get %s.%s.%s", c_prefix, __node, c_sub);
            break;
        case MoguSyntax::hash:
            if (id_required)
                __db.appendQuery(
                    "hget %s.%d.%s.%s %s", c_prefix, __id, __node, c_sub, __hashkey);
            else
                __db.appendQuery("hget %s.%s.%s %s", c_prefix, __node, c_sub, __hashkey);
            break;
        case MoguSyntax::list:
            if (!list_value) return EMPTY;
            if (id_required)
                __db.appendQuery(
                    "lindex %s.%d.%s.%s %d",
                    c_prefix, __id, __node, c_sub, __list_index);
            else
                __db.appendQuery(
                    "lindex %s.%s.%s %d", c_prefix, __node, c_sub, __list_index);
            break;
        default:return EMPTY;
    };
    std::string result = __db.yieldResponse <std::string>();
    if (Security::isEncrypted(result))
        return Security::decrypt(result);
    return result;
}

void NodeEditor::writeAll(std::map <std::string, std::string>& source)
{
    NodeValue key_nv;
    __arg = &key_nv;
    NodeValue val_nv;
    __type = MoguSyntax::hash;
    for (auto iter = source.begin(); iter != source.end(); ++iter)
    {
        std::string key = iter->first;
        std::string val = iter->second;
        key_nv.setString(key);
        key_cached = false;
        setKey();
        val_nv.setString(val);
        write(val_nv);
    }
}

void NodeEditor::write(NodeValue raw_value)
{
    std::string val = 
        raw_value.isString()    ?  (std::string) raw_value
        : raw_value.isInt()     ?  std::to_string ((int) raw_value)
        : std::to_string((float) raw_value);

    if (encrypted)
        val = Security::encrypt(val);

    const char* c_val = val.c_str();

    switch (__type)
    {
        case MoguSyntax::string:
            if (id_required)
                __db.appendQuery("set %s.%d.%s %s",
                    c_prefix, __id, __node, c_val);
            else
                __db.appendQuery("set %s.%s %s", c_prefix, __node, c_val);
            break;
        case MoguSyntax::hash:
            if (id_required)
                __db.appendQuery("hset %s.%d.%s %s %s",
                    c_prefix, __id, __node, __hashkey, c_val);
            else
                __db.appendQuery("hset %s.%s %s %s",
                    c_prefix, __node, __hashkey, c_val);
            break;
        case MoguSyntax::list:
            if (list_value && id_required)
                __db.appendQuery("lset %s.%d.%s %d %s",
                    c_prefix, __id, __node, __list_index, c_val);
            else if (list_value)
                __db.appendQuery("lset %s.%s %d %s",
                    c_prefix, __node, __list_index, c_val);
            else if (id_required)
                __db.appendQuery("rpush %s.%d.%s %s",
                    c_prefix, __id, __node, c_val);
            else
                __db.appendQuery("rpush %s.%s %s", c_prefix, __node, c_val);
            break;
        default:return;
    }
    __db.execute();
}

void NodeEditor::remove(std::string value)
{
    const char* c_value = value.c_str();
    bool remove_by_value = (value != EMPTY);
    switch(__type)
    {
        case MoguSyntax::hash:
            if (id_required && hash_value)
            {
                __db.appendQuery("hdel %s.%d.%s %s", c_prefix, __id, __node, __hashkey); 
                break;
            }
            else if (hash_value)
            {
                __db.appendQuery("hdel %s.%s %s", c_prefix, __node, __hashkey);
                break;
            }
            // No break on purpose! If the node's a hash, 'list_value' will be
            // false, meaning the next two tests will fail, and we'll continue to
            // standard key deletion.
        case MoguSyntax::list:
            if (id_required && list_value)
            {
                if (!remove_by_value)
                {
                    __db.appendQuery("lindex %s.%d.%s %d",
                        c_prefix, __id, __node, __arg->getInt());
                    std::string val_at_index = __db.yieldResponse <std::string>();
                    c_value = val_at_index.c_str();
                }
                __db.appendQuery("lrem %s.%d.%s 1 %s", c_prefix, __id, __node, c_value);
                break;
            }
            else if (list_value)
            {
                if (!remove_by_value)
                {
                    __db.appendQuery("lindex %s.%s %d",
                        c_prefix, __node, __arg->getInt());
                    std::string val_at_index = __db.yieldResponse <std::string>();
                    c_value = val_at_index.c_str();
                }
                __db.appendQuery("lrem %s.%s 1 %s", c_prefix, __node, c_value);
                break;
            }
            // No break on purpose! All keys are deleted the same way.
        case MoguSyntax::string:
            if (id_required)
                __db.appendQuery("del %s.%d.%s", c_prefix, __id, __node);
            else
                __db.appendQuery("del %s.%s", c_prefix, __node);
            break;
        default: return;
    };
}

inline int NodeEditor::getObjectId()
{
    mApp;
    switch(__prefix)
    {
        case Prefix::group:
            return app->getGroup();
        case Prefix::user:
            return app->getUser();
        default:return -1;
    };
    return -1;
}

}//namespace NodeEditor
