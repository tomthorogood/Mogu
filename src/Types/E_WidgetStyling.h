/*
 * E_WidgetStyling.h
 *
 *  Created on: Aug 18, 2012
 *      Author: tom
 */

#ifndef E_WIDGETSTYLING_H_
#define E_WIDGETSTYLING_H_

namespace StyleDirectivesFlags{
enum StyleDirectivesFlags {
    is_node             =0x1,
    is_positioning      =0x2,
    is_content          =0x4,
    is_external         =0x8,
    is_uri              =0x10,
    is_event            =0x20,
    is_widget_type      =0x40
};
} //namespace StyleDirectivesFlags

/*!\brief The style keys available to Goo */
namespace StyleDirectives{
enum StyleDirectives {
    css_class           =0x0,
    children            =0x1,
    content_alignment   =0x2,
    text_content        =0x4,
    content_dimensions  =0x6,
    text_file           =0xC,
    source              =0x14,
    location            =0x18,
    events              =0x21,
    widget_type         =0x40,
    content_node        =0x5,
    internal_path       =0xF
};
} //namespace StyleDirectives



#endif /* E_WIDGETSTYLING_H_ */
