#ifndef NODEDITOR_H_
#define NODEDITOR_H_

#include "MoguQueryHandler.h"
#include "DatabaseConfigReader.h"
#include "../Types/NodeValue.h"
#include "../Types/SyntaxDef.h"
#include "../Types/syntax.h"
#include <cassert>
#include <string>
#include <vector>
#include <map>


namespace Redis
{

class NodeEditor
{
public:
    NodeEditor();
    NodeEditor(
            const Prefix& prefix
            , const std::string& node
            , NodeValue* arg=nullptr);

    void setPrefix(Prefix);

    ~NodeEditor()
    {
        if (db)
        { 
            db->flush();
            delete db;
        }
        if (policy)
        {
            policy->flush();
            delete policy;
        }
    }

    inline void setSub(const std::string& sub_)
    {
        sub = sub_;
        setExists();
        setType();
    }

    inline void clearSub()
    {
        sub = "";
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
    bool write(std::string, bool hold_for_multiple_writes=false);

    /* Write an entire hash key */
    bool write(std::map <std::string,std::string>&);

    /* Write an entire list */
    bool write(std::vector<std::string>&);

    /* Deletes a node. */
    bool remove();

    /* Deletes either a list value or hash key */
    bool remove(const std::string& val);

    /* Readability function */
    inline bool id_required()
    { return (prefix==Prefix::user)||(prefix==Prefix::group);}

    inline void setId(int id_) {
        if (hasId()) return;
        id = id_; setup();
    }

    bool setEncrypted();

    inline void setNode(const std::string& node_)
    { node = node_;}
    
    inline const SyntaxDef& policyType()
    {
        setPolicy();
        policy->appendQuery(
            "hget policies.%s %d", node.c_str(), MoguSyntax::type.integer);
        return MoguSyntax::get(policy->yieldResponse<std::string>());
    }

    inline void unsetArgInfo()
    {
        arg_str = "";
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

    inline bool nodeExists() const { return exists; }

private:

    Prefix  prefix      = Prefix::__NONE__;
   
    MoguQueryHandler* db = nullptr;
    
    /* For user and group nodes, we'll need to read the node policy as well.*/
    MoguQueryHandler* policy = nullptr;
    NodeValue* arg          =NULL;
    NodeValue* arg_storage  =NULL;

    bool exists             = false;
    bool hash_exists        = false;
    bool is_setup           = false;
    int  type               = MoguSyntax::__NONE__.integer;
    
    /*  The string representation of the Prefix given. */
    std::string prefix_str    = "";

    /* The [name] in the [prefix].[id?].[name]... format. */
    std::string node      = "";

    /* If applicable, the subnode for the prefix:
     * [prefix].[id?].[name].[sub]
     */
    std::string sub       = "";

    /* If applicable, the hashkey used when trying to access
     * a single value in a hash node. If this is attempted but the
     * hashkey is empty, the read function will return false.
     */
    std::string arg_str   = "";

    /* The id used to read user or group nodes */
    int id      = -1;

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
   


    /* Same as above but for default hashes. */
    void getDefault(std::map<std::string,std::string>&);

    /* Same as above but for default lists. */
    void getDefault(std::vector<std::string>&);
    std::string buildCommand(const std::string& cmd, std::string extra="");



    inline bool hasId() 
    { return id != -1; }

    inline void setPolicy()
    {
        if (!policy) policy = new Redis::MoguQueryHandler(
            Application::contextMap, Prefix::policies);
    }

    inline void setExists()
    {
        unsetArgInfo();
        db->appendQuery(buildCommand("exists").c_str());
        exists = db->yieldResponse<bool>();
        setArgInfo();
    }

    inline void setType()
    {
        unsetArgInfo();
        db->appendQuery(buildCommand("type"));
        type = MoguSyntax::get(db->yieldResponse<std::string>()).integer;
        setArgInfo();
    }

    inline std::string buildNode()
    {
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

    void setup();

};//class NodeEditor


}//namespace Redis

#endif
