#ifndef MOGUQUERYHANDLER_H_
#define MOGUQUERYHANDLER_H_

#include "../Types/Prefixes.h"
#include "QueryHandler/QueryHandler.h"
namespace Redis {
class Mogu_Query_Handler : public Query_Handler {
public:
    Mogu_Query_Handler(Prefix prefix);
};

}//namespace Redis

#endif
