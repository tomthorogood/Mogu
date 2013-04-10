/*
 * WriteUserField.cpp
 *
 *  Created on: Apr 8, 2013
 *      Author: tom
 */

#include "WriteUserField.h"
#include "ContextQuery.h"
namespace Redis {

void writeUserField (
    const std::string& keyspace, Field& field, const NodeValue& value)
{
    using QuerySet::REQUIRE_INT;
    using QuerySet::REQUIRE_STRING;
    using QuerySet::IGNORE_RESPONSE;

    std::string enc_value;
    std::string query_cmd;

    const char* c_field = field.name.c_str();
    const char* c_keyspace = keyspace.c_str();
    const char* c_arg = field.arg_c_str();
    const char* c_value;
    value.c_str(c_value);

    /* First, we need to look up the storage policy for the field.
     * After that, we'll extrapolate the type and encryption settings for the
     * field, and then make the write to the database.
     */

    ContextQuery policy(Prefix::policies);

    policy.appendQuery(std::make_shared<Query>("hget policies.%s %d",
        c_field, (int) MoguSyntax::type), REQUIRE_INT);

    policy.appendQuery(std::make_shared<Query>("hget policies.%s %d",
        c_field, "encrypted"), REQUIRE_INT);

    policy.appendQuery(std::make_shared<Query>("hget policies.%s %d",
        c_field, (int) MoguSyntax::mode), REQUIRE_INT);

    int  field_type = policy.yieldResponse<int>();
    bool encrypted  = policy.yieldResponse<bool>();
    int  mode       = policy.yieldResponse<int>();

    if (encrypted) {
        enc_value = Security::encrypt(c_value);
        c_value = enc_value.c_str();
    }

    if (field_type == (int) MoguSyntax::hash)
    {
        // There is no other mode but 'replace' for hashes.
        // hset user.[keyspace].[field] [key] [value]
        query_cmd = "hset user.%s.%s %s %s";
    }

    else if (field_type == (int) MoguSyntax::list)
    {
        if (mode == (int) MoguSyntax::replace) {
            //lset user.[keyspace].[field] [index] [value]
            query_cmd = "lset user.%s.%s %d %s";
        }
        else if (mode == (int) MoguSyntax::append) {
            //rpush user.[keyspace].[field] [value]
            c_arg = EMPTY;
            query_cmd = "rpush user.%s.%s %s";
        }
    }

    else if (field_type == (int) MoguSyntax::string)
    {
        // There is no other mode but 'replace' for strings.
        // set user.[keyspace].[field] [value]
        c_arg = EMPTY;
        query_cmd = "set user.%s.%s %s";
    }
    else
    {
        return;
    }

    std::shared_ptr <Query> write = std::make_shared <Query>(
        new Query(query_cmd.c_str(),c_keyspace,c_field,c_arg, c_value));
    ContextQuery user(Prefix::user);
    user.appendQuery(write, IGNORE_RESPONSE);
    user.execute();

}

}//namespace Redis
