#include "NodeEditor.h"

namespace Redis {

NodeEditor::NodeEditor(
        MoguSyntax m_prefix
        , const std::string& node_name
        , Nodevalue* arg)
    :
        __prefix(syntax_to_prefix(m_prefix))
        , __db(__prefix)
        , __policies(Prefix::policies)
        , __node(node_name.c_str())
        , __arg(arg)
        , encrypted(fieldHasEncryption())
        , __id(getObjectId())
{
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

    if (encrypted) return Security::decrypt(db.yieldResposne <std::string>());
    else return db.yieldResponse <std::string>();
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
