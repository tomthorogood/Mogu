#ifndef QUERYCONSTRUCT_H_
#define QUERYCONSTRUCT_H_

#include <declarations.h>

namespace Redis {

/*!\brief Given a QueryBlueprint, and a command, fills in the blanks
 * required to create a query.
 */
class QueryConstruct {
public:
    QueryConstruct(QueryBlueprint& blueprint, const CommandValue& command);
    void appendQuery(ContextQuery& db);

private:
    std::string node;
    std::string sarg1, sarg2;
    int iarg1, iarg2;
    QueryBlueprint& blueprint;

};


}//namespace Redis

#endif
