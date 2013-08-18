#include "MoguQueryHandler.h"
#include "ContextMap.h"
#include "DatabaseConfigReader.h"

namespace Application {
    extern Context_Map* context_map;
}

namespace Redis {
Mogu_Query_Handler::Mogu_Query_Handler(Prefix p)
    : Query_Handler(Application::context_map->get(p)) {}
}
