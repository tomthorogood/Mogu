#ifndef NODEDITOR_H_
#define NODEDITOR_H_

#include <declarations.h>
#include "ContextQuery.h"
#include <cassert>
namespace Redis
{

class NodeEditor
{
public:
    NodeEditor();
    NodeEditor(const Prefix& prefix,
            const std::string& node,
            NodeValue* arg=nullptr);

    void setPrefix(Prefix);

    ~NodeEditor()
    {
    }

    inline void setSub(const std::string& sub_)
    {
        sub = sub_;
        setExists();
        setType();
    }

    inline void clearSub()
    {
        sub = EMPTY;
        setExists();
        setType();
    }

    int getPolicyType();

    /* Read a single value */
    std::string read();

    /* Read a list of values */
    void read(std::vector<std::string>&);

    /* Read a hash node */
    void read(std::map<std::string,std::string>&);

    /* Write a single value at the node. */
    bool write(std::string);

    /* Write an entire hash key */
    bool write(std::map <std::string,std::string>&);

    /* Write an entire list */
    bool write(std::vector<std::string>&);

    /* Deletes a node. */
    bool remove();

    /* Deletes either a list value or hash key */
    bool remove(const std::string& val);

    inline bool setEncrypted()
    {
        setPolicy(); 
        policy.appendQuery(
            "hget policies.%s %d", node.c_str(), MoguSyntax::encrypted.integer);
        encrypted = policy.yieldResponse<std::string>() == "yes";
        return encrypted;
    }

    inline void setNode(const std::string& node_)
    { node = node_;}
    
    inline const SyntaxDef& policyType()
    {
        setPolicy();
        policy.appendQuery(
            "hget policies.%s %d", node.c_str(), MoguSyntax::type.integer);
        return MoguSyntax::get(policy.yieldResponse<std::string>());
    }

    inline void unsetArgInfo()
    {
        hashkey = "";
        list_index = -1;
    }

    /* Set the arg to a new arg. The old arg is lost. */
    inline void setArg(NodeValue* arg_)
    {
        unsetArgInfo();
        arg = arg_;
        setArgInfo();
    }

    /* When given a new argument to swap in, the old
     * argument is put into 'arg_storage', where it can be retrieved
     * at a later time.
     */
    inline void swapArg (NodeValue* arg_)
    {
        unsetArgInfo();
        arg_storage = arg;
        arg = arg_;
        setArgInfo();
    }

    /* When given no new argument, the argument in storage will be
     * swapped in instead. */
    inline void swapArg()
    {
        unsetArgInfo();
        arg = arg_storage;
        arg_storage = NULL;
        setArgInfo();
    }


    inline const SyntaxDef& getType() const
    {
        return MoguSyntax::get(type);
    }

private:

    Prefix  prefix      = Prefix::__NONE__;
   
    ContextQuery db;
    
    /* For user and group nodes, we'll need to read the node policy as well.*/
    ContextQuery policy;
    NodeValue* arg          =NULL;
    NodeValue* arg_storage  =NULL;

    bool exists             = false;
    bool hash_exists        = false;
    int  type               = MoguSyntax::__NONE__.integer;
    
    /*  The string representation of the Prefix given. */
    std::string prefix_str    = EMPTY;

    /* The [name] in the [prefix].[id?].[name]... format. */
    std::string node      = EMPTY;

    /* If applicable, the subnode for the prefix:
     * [prefix].[id?].[name].[sub]
     */
    std::string sub       = EMPTY;

    /* If applicable, the hashkey used when trying to access
     * a single value in a hash node. If this is attempted but the
     * hashkey is empty, the read function will return false.
     */
    std::string hashkey   = EMPTY;

    /* The id used to read user or group nodes */
    int id      = -1;

    /* The index used to read from a list node. If this is still -1
     * and the read method is called on a list value, will return false. */
    int list_index  = -1;

    /* Whether or not the content of this node is encrypted, and therefore 
     * must be decrypted. Only applies to user/group data.
     */
    bool encrypted  = false;

    /* When writing a lot of things at once, we'll set this to true in order
     * to speed things up and write them all in a batch, instead of
     * individual queries. 
     */
    bool delay_execution = false;


    /* In the case of user and group nodes that have no value, we need
     * to check and see if the node policy lists a default value to use instead.
     * If so, we'll retrieve that value.
     */
    std::string getDefault();
    void setArgInfo();
    void setId();

    /* Same as above but for default hashes. */
    void getDefault(std::map<std::string,std::string>&);

    /* Same as above but for default lists. */
    void getDefault(std::vector<std::string>&);

    void appendCommand(const char* cmd);

    /* Readability function */
    inline bool hasId() 
    { return id != -1; }

    inline void setPolicy()
    {
        policy.setPrefix(Prefix::policies);
    }

    inline void setExists()
    {
        unsetArgInfo();
        appendCommand("exists");
        exists = db.yieldResponse<bool>();
        setArgInfo();
    }

    inline void setType()
    {
        unsetArgInfo();
        appendCommand("type");
        type = MoguSyntax::get(db.yieldResponse<std::string>()).integer;
        setArgInfo();
    }

    inline std::string buildNode()
    {
        assert(!isdigit(node[0]));
        std::stringstream buf;
        buf << prefix_str;

        if (hasId())
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

};//class NodeEditor


}//namespace Redis

#endif
