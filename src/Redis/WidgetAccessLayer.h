/*
 * WidgetAccessLayer.h
 *
 *  Created on: Oct 27, 2012
 *      Author: tom
 */

#ifndef WIDGETACCESSLAYER_H_
#define WIDGETACCESSLAYER_H_

#include <Redis/AccessLayer.h>

namespace Redis {

class WidgetAccessLayer
{
public:
    enum ReadRequests
    {
        widet_exists, widget_has_children,
    };
};

}

#endif /* WIDGETACCESSLAYER_H_ */
