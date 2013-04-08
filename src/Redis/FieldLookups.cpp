/*
 * FieldLookups.cpp
 *
 *  Created on: Apr 8, 2013
 *      Author: tom
 */
#include "ContextQuery.h"

namespace Redis {

void metaFieldLookup(const Field& field, NodeValue& result)
{
    using QuerySet::REQUIRE_INT;
    using QuerySet::REQUIRE_STRING;

    const char* c_field = field.name.c_str();

    ContextQuery db(Prefix::meta);

    std::shared_ptr <Query> field_exists = std::make_shared <Query>(
        new Query("hexists meta.%s", c_field)
    );

    std::shared_ptr <Query> field_type = std::make_shared <Query> (
        new Query("type meta.%s", c_field)
    );

    db.appendQuery(field_exists, REQUIRE_INT);
    db.appendQuery(field_type, REQUIRE_STRING);

    if (!db.yieldResponse<bool>()) return;

    std::shared_ptr <Query> lookup;

    std::string type = db.yieldResponse <std::string>();
    if (type == "hash") {
        const char* c_arg;

        // Determine whether the calling class gave a numeric or string
        // value for the hash key argument.
        if (field.arg == EMPTY) {
            std::string arg = std::to_string(field.index);
            c_arg = arg.c_str();
        }
        const char* c_arg = field.arg.c_str();
        lookup = std::make_shared <Query>(new Query("hget meta.%s %s"),
            c_field, c_arg);
    }

    else if (type == "list") {
        lookup = std::make_shared <Query>(new Query("lindex meta.%s %d"),
            c_field, field.index);
    }

    else if (type == "string") {
        lookup = std::make_shared <Query>(new Query("get meta.%s", c_field));
    }

    else return;

    db.appendQuery(lookup, REQUIRE_STRING);
    result.setString(db.yieldResponse<std::string>());
}

void userFieldLookup(
    const std::string& keyspace,
    const Field& field,
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
            new Query("exists user.%s %s", c_keyspace, c_field));

    std::shared_ptr <Query> field_type =
        std::make_shared <Query>(
            new Query("type user.%s %s", c_keyspace, c_field));

    db.appendQuery(field_exists, REQUIRE_INT);
    db.appendQuery(field_type, REQUIRE_STRING);

    if (!db.yieldResponse <bool>())
    {
        //get the default value from the meta keyspace
        metaFieldLookup(field,result);
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
           lookup = std::make_shared <Query>(new Query("hget user.%s %s"),
               c_field, c_arg);
       }

       else if (type == "list") {
           lookup = std::make_shared <Query>(new Query("lindex user.%s %d"),
               c_field, field.index);
       }

       else if (type == "string") {
           lookup = std::make_shared <Query>(new Query("get user.%s", c_field));
       }

       else return;

    db.appendQuery(lookup, REQUIRE_STRING);
    result.setString(db.yieldResponse <std::string>());
}

}//namespace Redis
