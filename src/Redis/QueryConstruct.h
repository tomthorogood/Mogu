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
    std::string node    = EMPTY;
    std::string sarg1   = EMPTY;
    std::string sarg2   = EMPTY;
    int iarg1 = -1; 
    int iarg2 = -1;
    QueryBlueprint& blueprint;

};


}//namespace Redis

#endif
