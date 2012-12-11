/*
 * Action_TestMatch.cpp
 *
 *  Created on: Oct 8, 2012
 *      Author: tom
 */

#include <Events/MoldableActionCenter.h>
#include <Types/NodeValue.h>
#include <Mogu.h>

namespace Events {
namespace ActionCenter {
namespace Actions {

bool test(
    Moldable& widget, NodeValue& val)
{
    mApp;
    app->interpreter().giveInput(val.getString(), val);

    if (widget.allowsAction(Enums::SignalActions::match)) {
        return val.getString().find(widget.moldableValue()) != std::string::npos;
    }

    return false;
}

bool true_test(NodeValue& alpha, NodeValue& beta)
{
#ifdef DEBUG

    if (alpha.getType() != beta.getType()) return false;
    switch (alpha.getType())
    {
    case string_value:
        std::cout << "Testing " << alpha.getString() << " == " << beta.getString();
        std::cout << std::endl;
        return alpha.getString() == beta.getString();
        break;
    case int_value:
        std::cout << "Testing " << alpha.getInt() << " == " << beta.getInt();
        std::cout << std::endl;
        return alpha.getInt() == beta.getInt();
        break;
    case float_value:
        std::cout << "Testing " << alpha.getFloat() << " == " << beta.getFloat();
        std::cout << std::endl;
        return alpha.getFloat() == beta.getFloat();
        break;
    default:
        return false;
    }
#else
    if (alpha.getType() != beta.getType()) return false;
    switch (alpha.getType())
    {
    case string_value:
        return alpha.getString() == beta.getString();
        break;
    case int_value:
        return alpha.getInt() == beta.getInt();
        break;
    case float_value:
        return alpha.getFloat() == beta.getFloat();
        break;
    default:
        return false;
    }
#endif
}

}    //Actions
}    //ActionCenter
}    //Events

