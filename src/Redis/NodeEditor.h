#ifndef NODEEDITOR_H_
#define NODEEDITOR_H_

#include "ContextQuery.h"
#include <Types/NodeValue.h>
namespace Redis {

//!\brief Performs reads and writes from the Redis database.
class NodeEditor {
public:
    inline bool exists()
    {
        if (id_required)
            __db.appendQuery("exists %s.%d.%s", c_prefix, __id, __node);
        else
            __db.appendQuery("exists %s.%s", c_prefix, __node);
        return __db.yieldResponse <bool>();
    }

    inline bool subExists(const std::string& sub)
    {
        if (id_required)
            __db.appendQuery("exists %s.%d.%s.%s", c_prefix, __id, __node, sub.c_str());
        else
            __db.appendQuery("exists %s.%s.%s", c_prefix, __node, sub.c_str());
        return __db.yieldResponse<bool>();
    }
    inline void toggleSub(const std::string& sub)
    {
        if (__sub)
        {
            __sub = false;
            getType();
            setPrefix(__prefix);
        }
        else
        {
            __sub = true;
            __subType = getSubType(sub.c_str());
        }
    }

    NodeEditor(const SyntaxDef& m_prefix , const std::string& node_name
        , NodeValue* arg = nullptr);

    NodeEditor(
        Prefix p_prefix
        , const std::string& node_name
        , NodeValue* arg = nullptr);
    
    NodeEditor(
        Prefix p_prefix
        , const std::string& node_name
        , const SyntaxDef& nodetype);

    inline bool fieldHasEncryption()
    {
        Redis::ContextQuery __policies(Prefix::policies);
        __policies.appendQuery("hget policies.%s %d", __node,
                MoguSyntax::encrypted);
        return __policies.yieldResponse <bool>();
    }

    inline void setIsListValue(bool tf) { list_value = tf;}
    inline void setIsHashValue(bool tf) { hash_value = tf;}
    inline bool isListValue() { return list_value;}
    inline bool isHashValue() { return hash_value;}
    inline const SyntaxDef& getNodeType()
    {
        if (!exists()) return MoguSyntax::__NONE__;
        if (id_required)
            __db.appendQuery("type %s.%d.%s", c_prefix, __id, __node);
        else 
            __db.appendQuery("type %s.%s", c_prefix, __node);
        return MoguSyntax::get(__db.yieldResponse<std::string>());
    }

    inline const SyntaxDef& getSubType(const char* sub)
    {
        if (id_required)
            __db.appendQuery("type %s.%d.%s.%s", c_prefix, __id, __node, sub);
        else
            __db.appendQuery("type %s.%s.%s", c_prefix, __node, sub);
        return MoguSyntax::get(at(__db.yieldResponse <std::string>()));
    }
    inline int getObjectId();
    inline ContextQuery& getContext() { return __db;}
    inline const SyntaxDef& getType() const { return __type;}
    inline Prefix getPrefix() { return __prefix;}
    void readAll (std::map <std::string,std::string>&);
    std::string readSub (const std::string&);
    inline std::string readSub(const std::string&& str) { return readSub(str);}
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
    
    const char*     __node;
    NodeValue*      __arg;
    int             __id    =-1; // for groups or users;
    SyntaxDef       __type;
    SyntaxDef       __subType = MoguSyntax::__NONE__;

    bool encrypted;
    bool list_value =   false;
    bool hash_value =   true;
    const char* c_prefix;

    const char* __hashkey       = EMPTY;
    int         __list_index    = -1;
    bool        key_cached      = false;
    bool        id_required     = false;
    bool        __sub           = false;
    
    std::string hashkey_s;

    inline void setKey()
    {
        if (__arg == nullptr || key_cached) return;
        SyntaxDef type = __sub ? __subType : __type;
        switch(type)
        {
            case MoguSyntax::list:
                __list_index = __arg->getInt();
                break;
            case MoguSyntax::hash:
                {
                    std::string str;
                    if (__arg->isString())
                        hashkey_s = __arg->getString();
                    else if (__arg->isInt())
                        hashkey_s = std::to_string(__arg->getInt());
                    else
                        hashkey_s = std::to_string(__arg->getFloat());
                    __hashkey   = hashkey_s.c_str();
                }
                break;
            default:return;
        };
        key_cached = true;
    }
};

}//namespace Redis

#endif //NODEEDITOR_H_
