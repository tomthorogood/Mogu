#ifndef NODEDITOR_H_
#define NODEDITOR_H_

#include <declarations.h>

namespace Redis
{

class NodeEditor
{
public:
    NodeEditor();
    NodeEditor(const Prefix& prefix,
            const std::string& node,
            NodeValue* arg=nullptr);

    ~NodeEditor()
    {
        if (policy != nullptr) delete policy;
    }

    void setSub(const std::string& sub)
    {
        c_sub = sub.c_str();
    }

    void clearSub()
    {
        c_sub = EMPTY;
    }

    /* Read a single value */
    std::string read();

    /* Read a list of values */
    void read(std::vector<std::string>()&);

    /* Read a hash node */
    void read(std::map<std::string,std::string>()&);

    /* Write a single value at the node. */
    bool write(const std::string&);

    /* Write an entire hash key */
    bool write(std::map <std::string,std::string>()&);

    /* Write an entire list */
    bool write(std::vector<std::string>&);

    /* Deletes a node. */
    bool remove();

    /* Deletes either a list value or hash key */
    bool remove(const std::string& val);

    bool setEncrypted()
    { 
        policy->appendQuery(
            "hget policies.%s %d", node, MoguSyntax::encrypted.integer);
        encrypted = policy->yieldResponse<std::string>() == "yes"; 
        return encrypted;
    }

private:

    Prefix  prefix      = Prefix::__NONE__;
   
    ContextQuery db;
    
    /* For user and group nodes, we'll need to read the node policy as well.*/
    ContextQuery* policy    =nullptr;

    bool exists             = false;
    bool hash_exists        = false;
    int  type               = MoguSyntax::__NONE__.integer;
    
    /*  The string representation of the Prefix given. */
    const char* c_prefix    = EMPTY;

    /* The [name] in the [prefix].[id?].[name]... format. */
    const char* c_node      = EMPTY;

    /* If applicable, the subnode for the prefix:
     * [prefix].[id?].[name].[sub]
     */
    const char* c_sub       = EMPTY;

    /* If applicable, the hashkey used when trying to access
     * a single value in a hash node. If this is attempted but the
     * hashkey is empty, the read function will return false.
     */
    const char* c_hashkey   = EMPTY;

    /* The id used to read user or group nodes */
    int id      = -1;

    /* The index used to read from a list node. If this is still -1
     * and the read method is called on a list value, will return false. */
    int list_index  = -1;

    /* Whether or not the content of this node is encrypted, and therefore 
     * must be decrypted. Only applies to user/group data.
     */
    bool encrytped  = false;

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
        if (policy == nullptr)
            policy = new ContextQuery(Prefix::policies);
    }

    void setArgInfo();

    inline void setId()
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

    inline void unset_arg()
    {
        arg = nullptr;
        c_hashkey = EMPTY;
        list_index = -1;
    }

}//class NodeEditor


}//namespace Redis

#endif
