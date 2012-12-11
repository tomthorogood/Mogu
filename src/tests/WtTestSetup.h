#include <declarations.h>
#include <Mogu.h>
#include <TurnLeftLib/TestSuite/TestSuite.h>
#include <Wt/Test/WTestEnvironment>


class WtTestApp : public Mogu
{
public:
	WtTestApp(const Wt::WEnvironment& env);
};

WtTestApp* createWtTest();
Moldable* addTextWidget(WtTestApp& instance, MoldableTemplate* tpl);
