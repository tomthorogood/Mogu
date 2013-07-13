#include <declarations.h>
#include <Wt/WObject>
class CommandProcessor : public Wt::WObject
{
public:
    CommandProcessor(Moldable& broadcaster);
    void processCommand(CommandValue& v);
protected:
    Moldable& broadcaster;
};

