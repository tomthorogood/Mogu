/*
 * PropertyCallbacks.cpp
 *
 *  Created on: Oct 8, 2012
 *      Author: tom
 */


#include <Core/FactoryStations/PropertyCallbacks.h>
#include <Wt/WText>
#include <Wt/WLineEdit>
#include <Parsers/StyleParser.h>
#include <Core/Moldable.h>

namespace Goo
{
namespace MoldableFactory
{
namespace Callbacks
{

std::string __value_wtext(Moldable& widget)
{
	Wt::WText* txtwidget = static_cast<Wt::WText*>(widget.widget(0));
	return txtwidget->text().toUTF8();
}

std::string __value_wlineedit(Moldable& widget)
{
	Wt::WLineEdit* input = static_cast<Wt::WLineEdit*>(widget.widget(0));
	return input->valueText().toUTF8();
}

std::string __value_abstract(Moldable& widget)
{
	using namespace Parsers::StyleParser;
	if (widgetHasField(widget.getNode(), "abstract"))
		return getWidgetField(
				widget.getNode(), "abstract");
	return EMPTY;
}

std::string __value_container(Moldable& widget)
{
	Nodes::NodeValue v;
	widget.getState(Enums::WidgetTypes::currentstyle, v);
	return v.getString();
}

void __setvalue_wtext(Moldable& widget, std::string& newtext)
{
	Wt::WText* txtwidget = static_cast<Wt::WText*>(widget.widget(0));
	Wt::WString wtxt(newtext);
	txtwidget->setText(wtxt);
}

void __setvalue_wlineedit(Moldable& widget, std::string& newtext)
{
	Wt::WLineEdit* input = static_cast<Wt::WLineEdit*>(widget.widget(0));
	Wt::WString wtxt(newtext);
	input->setValueText(wtxt);
}

}//namespace Callbacks
}//namespace ModlableFactory
}//namespace Goo


