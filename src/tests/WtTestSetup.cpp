
#include <tests/WtTestSetup.h>
#include <Wt/Test/WTestEnvironment>
#include <Core/Moldable.h>
#include <Core/MoldableTemplate.h>
#include <Core/MoldableFactory.h>
#include <Wt/WContainerWidget>
#include <Wt/WText>


namespace{
static Wt::Test::WTestEnvironment env;
}

WtTestApp* createWtTest()
{
	return new WtTestApp(env);
}

WtTestApp::WtTestApp(const Wt::WEnvironment& env)
:	Mogu(env)
{

}

Goo::Moldable* addTextWidget(WtTestApp& instance, Goo::MoldableTemplate* tpl)
{
	Goo::Moldable* w = Goo::MoldableFactory::sculpt(tpl);
	instance.root()->addWidget(w);
	return w;
}


