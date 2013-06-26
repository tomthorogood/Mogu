#include "NodeEditor.h"
#include <Mogu.h>
#include <Security/Encryption.h>
namespace Redis {

NodeEditor::NodeEditor(
        MoguSyntax m_prefix
        , const std::string& node_name
        , NodeValue* arg)
    :
         __db(__prefix)
        , __policies(Prefix::policies)
        , __arg(arg)
{
    __prefix = syntax_to_prefix.at(m_prefix);
    __node = node_name.c_str();
    encrypted = fieldHasEncryption();
    __id = getObjectId();
    std::string s_prefix = prefixMap.at(__prefix);
    c_prefix = s_prefix.c_str();
    __type = getNodeType();
    setKey();
    id_required = !(__prefix == Prefix::data || __prefix == Prefix::meta);
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
                __db.appendQuery("hget %s.%s. %s", c_prefix, __node, __hashkey);
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

void NodeEditor::write(NodeValue& raw_value)
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
}

}//namespace NodeEditor
