/*
 * FieldLookups.cpp
 *
 *  Created on: Apr 8, 2013
 *      Author: tom
 */
#include "ContextQuery.h"

namespace Redis {

void genFieldLookup(const char* c_prefix,Field& field, NodeValue& result)
{
    using QuerySet::REQUIRE_INT;
    using QuerySet::REQUIRE_STRING;

    const char* c_field = field.name.c_str();

    ContextQuery db(Prefix::meta);

    std::shared_ptr <Query> field_exists = std::make_shared <Query>(
        new Query("hexists %s.%s", c_prefix,c_field)
    );

    std::shared_ptr <Query> field_type = std::make_shared <Query> (
        new Query("type %s.%s", c_prefix,c_field)
    );

    db.appendQuery(field_exists, REQUIRE_INT);
    db.appendQuery(field_type, REQUIRE_STRING);

    if (!db.yieldResponse<bool>()) return;

    std::shared_ptr <Query> lookup;

    std::string type = db.yieldResponse <std::string>();
    if (type == "hash") {
        const char* c_arg = field.arg_c_str();
        lookup = std::make_shared <Query>(new Query("hget %s.%s %s"),
            c_prefix,c_field, c_arg);
    }

    else if (type == "list") {
        lookup = std::make_shared <Query>(new Query("lindex %s.%s %d"),
            c_prefix,c_field, field.index);
    }

    else if (type == "string") {
        lookup = std::make_shared <Query>(new Query("get %s.%s", c_prefix,c_field));
    }

    else return;

    db.appendQuery(lookup, REQUIRE_STRING);
    result.setString(db.yieldResponse<std::string>());
}

void pvtFieldLookup(
    const char* c_prefix,
    const std::string& keyspace,
    Field& field,
    NodeValue& result)
{
    using QuerySet::REQUIRE_INT;
    using QuerySet::REQUIRE_STRING;
    using QuerySet::IGNORE_RESPONSE;

    const char* c_keyspace = keyspace.c_str();
    const char* c_field = field.name.c_str();
    ContextQuery db(Prefix::user);

    std::shared_ptr <Query> field_exists =
        std::make_shared <Query>(
            new Query("exists %s.%s.%s", c_prefix,c_keyspace, c_field));

    std::shared_ptr <Query> field_type =
        std::make_shared <Query>(
            new Query("type %s.%s.%s", c_prefix,c_keyspace, c_field));

    db.appendQuery(field_exists, REQUIRE_INT);
    db.appendQuery(field_type, REQUIRE_STRING);

    if (!db.yieldResponse <bool>())
    {
        // get the default value from the meta keyspace
        if (strcmp(c_prefix,"user")==0)
            metaFieldLookup(field,result);
        // or, if it's a group, get the default value from the meta group
        else if (strcmp(c_prefix,"group") ==0) {
            // Avoid an infinite loop with a nonexistent field.
            if (strcmp(c_keyspace,"0") == 0) return;

            // Exactly one recursion can take place.
            pvtFieldLookup("group", "0", field, result);
        }
        return;
    }

    std::string type = db.yieldResponse <std::string>();
    std::shared_ptr <Query> lookup;

    if (type == "hash") {
           const char* c_arg;

           // Determine whether the calling class gave a numeric or string
           // value for the hash key argument.
           if (field.arg == EMPTY) {
               std::string arg = std::to_string(field.index);
               c_arg = arg.c_str();
           }
           const char* c_arg = field.arg.c_str();
           lookup = std::make_shared <Query>(new Query("hget %s.%s.%s %s"),
               c_prefix,c_keyspace, c_field, c_arg);
       }

       else if (type == "list") {
           lookup = std::make_shared <Query>(new Query("lindex %s.%s.%s %d"),
               c_prefix,c_keyspace, c_field, field.index);
       }

       else if (type == "string") {
           lookup = std::make_shared <Query>(new Query("get %s.%s.%s",
               c_prefix,c_keyspace, c_field));
       }

       else return;

    db.appendQuery(lookup, REQUIRE_STRING);
    result.setString(db.yieldResponse <std::string>());
}

void metaFieldLookup(Field& field, NodeValue& result)
{
    genFieldLookup("meta", field, result);
}

void dataFieldLookup(Field& field, NodeValue& result)
{
    genFieldLookup("data", field, result);
}

void userFieldLookup(const std::string& keyspace, Field& field, NodeValue& result)
{
    pvtFieldLookup("user", keyspace,field,result);
}

void groupFieldLookup(const std::string& keyspace, Field& field, NodeValue& result)
{
    pvtFieldLookup("group", keyspace, field, result);
}

}//namespace Redis
