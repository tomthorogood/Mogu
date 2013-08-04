#ifndef MOGUQUERYHANDLER_H_
#define MOGUQUERYHANDLER_H_

#include "DatabaseConfigReader.h"
#include "QueryHandler/QueryHandler.h"
#include "ContextMap.h"

namespace Redis {

class MoguQueryHandler : public QueryHandler {
public:
    MoguQueryHandler(ContextMap& m, Prefix prefix)
        : QueryHandler(m.get(prefix)) {}

    MoguQueryHandler(ContextMap* m, Prefix prefix)
        : QueryHandler(m->get(prefix)) {}
};

}//namespace Redis

#endif
