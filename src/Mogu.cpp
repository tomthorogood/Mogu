/*
 * preview.cpp
 *
 *  Created on: Jul 17, 2012
 *      Author: tom
 */

#include <Mogu.h>
#include <string>
#include <Core/Moldable.h>
#include <Wt/WApplication>
#include <Wt/WScrollArea>
#include <Wt/WString>

Mogu::Mogu(const Wt::WEnvironment& env)
:   Wt::WApplication(env)
{
    std::string styleSheet("/resources/mogu/style.css");
    useStyleSheet(styleSheet);

    std::string outermost_container = "widgets.wrapper";

    /* This will be deleted by the wrapper! */
    Goo::Moldable* moldableWrapper =
            new Goo::Moldable(outermost_container);
    root()->addWidget(moldableWrapper);
}
