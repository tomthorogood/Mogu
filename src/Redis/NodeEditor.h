#ifndef NODEEDITOR_H_
#define NODEEDITOR_H_

#include "ContextQuery.h"
#include <Types/NodeValue.h>
namespace Redis {

//!\brief Performs reads and writes from the Redis database.
class NodeEditor {
public:
    NodeEditor(
        MoguSyntax m_prefix
        , const std::string& node_name
        , NodeValue* arg = nullptr);

    NodeEditor(
        Prefix p_prefix
        , const std::string& node_name
        , NodeValue* arg = nullptr);
    
    NodeEditor(
        Prefix p_prefix
        , const std::string& node_name
        , MoguSyntax nodetype);

    inline bool fieldHasEncryption()
    {
        __policies.appendQuery("hget policies.%s %d", __node,
                MoguSyntax::encrypted);
        return __policies.yieldResponse <bool>();
    }

    inline void setIsListValue(bool tf) { list_value = tf;}
    inline void setIsHashValue(bool tf) { hash_value = tf;}
    inline bool isListValue() { return list_value;}
    inline bool isHashValue() { return hash_value;}
    inline MoguSyntax getNodeType()
    {
        __db.appendQuery("type %s.%s", c_prefix, __node);
        return string_to_node_type.at(__db.yieldResponse <std::string>());
    }
    inline int getObjectId();
    inline ContextQuery& getContext() { return __db;}
    inline MoguSyntax getType() const { return __type;}
    inline Prefix getPrefix() { return __prefix;}
    void readAll (std::map <std::string,std::string>&);
    void writeAll (std::map <std::string, std::string>&);
    void setArg(NodeValue*);
    void setNodeName(const std::string&);
    void setPrefix(Prefix);

    std::string read();
    void write (NodeValue);
    void remove(std::string value = EMPTY);


private:

    Prefix          __prefix;
    
    ContextQuery __db;
    ContextQuery __policies;
    
    const char*     __node;
    NodeValue*      __arg;
    int             __id    =-1; // for groups or users;
    MoguSyntax      __type;

    bool encrypted;
    bool list_value =   false;
    bool hash_value =   true;
    const char* c_prefix;

    const char* __hashkey       = EMPTY;
    int         __list_index    = -1;
    bool        key_cached      = false;
    bool        id_required     = false;
    
    inline void setKey()
    {
        if (__arg == nullptr || key_cached) return;
        switch(__type)
        {
            case MoguSyntax::list:
                __list_index = __arg->getInt();
                break;
            case MoguSyntax::hash:
                {std::string str = __arg->getString();
                __hashkey   = str.c_str();}
                break;
            default:return;
        };

        key_cached = true;
    }


};

}//namespace Redis

#endif //NODEEDITOR_H_
