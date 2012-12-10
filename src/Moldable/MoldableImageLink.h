/*
 * MoldableImageLink.h
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#ifndef MOLDABLEIMAGELINK_H_
#define MOLDABLEIMAGELINK_H_

#include "MoldableLink.h"

class MoldableImageLink : public MoldableLink
{
    Wt::WImage* __image;
public:
    MoldableImageLink(const std::string& node);
};

#endif /* MOLDABLEIMAGELINK_H_ */
