#include <Wt/WObject>
#include "../Moldable/Moldable.h"
#include "../Types/CommandValue.h"
class Command_Processor : public Wt::WObject
{
public:
    Command_Processor(Moldable& w) : broadcaster(w) {}
    void process_command(Command_Value&);
protected:
    Moldable& broadcaster;
};

