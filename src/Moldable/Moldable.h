/*
 * Moldable.h
 *
 *  Created on: Dec 9, 2012
 *      Author: tom
 */

#ifndef MOLDABLE_H_
#define MOLDABLE_H_

#include <declarations.h>
#include <Wt/WContainerWidget>
#include <Wt/WSignal>

namespace Mogu
{

class Moldable : public Wt::WContainerWidget
{
    Events::EventBindery* __bindery;
    Wt::Signal <> __style_changed;
    Wt::Signal <> __failed_test;
    Wt::Signal <> __succeeded_test;
    Wt::Signal <> __loaded;

    MoguNode __node;

public:

    Moldable();

    virtual std::string moldableValue() =0;
    virtual void setMoldableValue() =0;
    inline virtual void setStyleClass (const Wt::WString& style)
    {
        Wt::WContainerWidget::setStyleClass(style);
        __style_changed.emit();
    }
};

}


#endif /* MOLDABLE_H_ */
