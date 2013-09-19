/*
 * Mogu.cpp
 *
 *  Created on: Jul 17, 2012
 *      Author: tom
 */

#include <algorithm>

#include <Wt/WApplication>
#include <Wt/WOverlayLoadingIndicator>
#include <Wt/WScrollArea>
#include <Wt/WString>
#include <Wt/WStackedWidget>

#include "Mogu.h"
#include "Moldable/Moldable.h"
#include "Factories/MoldableFactory.h"
#include "Events/PerspectiveHandler.h"
#include "Redis/NodeEditor.h"
#include "utilities.h"

#include <time.h>

Mogu::Mogu(
    const Wt::WEnvironment& env)
    : Wt::WApplication(env)
    , moldable_factory()
{
    Redis::Node_Editor js_node(Prefix::meta, "javascript");
    Redis::Node_Editor meta_root(Prefix::meta, "root");
    Node_Value t1 {static_cast<int>(time(nullptr))};
    slot_manager.set_slot("SESSION_START", t1);

    setLoadingIndicator(new Wt::WOverlayLoadingIndicator());
    internalPathChanged().connect(this, &Mogu::handle_path_change);

    // Set up global javascript hooks
    declareJavaScriptFunction("getWindowHeight", JavaScript::get_window_height);
    declareJavaScriptFunction("setVerticallyCentered",
        replace_js_refs(JavaScript::set_vertically_centered));

    std::string style_sheet("/resources/mogu/style.css");
    useStyleSheet(style_sheet);
    
    Wt::WCssStyleSheet& css = styleSheet();
    css.addRule(
        ".mogu_SCALED_IMAGE, .mogu_SCALED_IMAGE *", "width:100%;", "scaledImages");

    // Read custom javascript hooks from the database
    std::map <std::string,std::string> js_decl {};
    js_node.read(js_decl);
    for (auto i : js_decl)
    {
        declareJavaScriptFunction(i.first, replace_js_refs(i.second));
    }

    // Create the base Mogu widget
    std::string root_widget = meta_root.read();
    wrapper = moldable_factory.create_moldable_widget(root_widget);
    root()->addWidget(wrapper);



    // Mold the application based on the URL
    std::string entry_path = internalPath();
    if (entry_path != "/" && entry_path.length() > 0) {
        handle_path_change(entry_path);
    }
}

void Mogu::handle_path_change(const std::string& path)
{
    std::vector <std::string> perspectives {};
    split(path, '/', perspectives);
    for (std::string perspective : perspectives)
    {
        Perspective_Handler p {*wrapper, perspective};
        p.mold_perspective();
    }
}

Mogu::~Mogu()
{
    /* Stop the widgets from trying to communicate with the base application
     * during the application's destruction, as it leads to a segfault.
     */
    for (auto i = widget_register.begin() ; i != widget_register.end(); ++i)
    {
        if (i->second) i->second->shun();
    }
    
    int t1 {slot_manager.get_slot("SESSION_START").get_int()};
    // Report the session length in the user's meta fields.
    if (t1 && get_user()>-1)
    {
        int t2 {static_cast<int>(time(nullptr))};
        int len {t2-t1};
        Node_Value arg {std::to_string(t2)};
        Redis::Node_Editor e {Prefix::user, "__meta__.s", &arg};
        e.set_id(get_user());
        e.write(std::to_string(len));
    }
}
