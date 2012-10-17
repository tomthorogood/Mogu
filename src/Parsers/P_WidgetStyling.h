/*
 * P_WidgetStyling.h
 *
 *  Created on: Aug 18, 2012
 *      Author: tom
 */

#ifndef P_WIDGETSTYLING_H_
#define P_WIDGETSTYLING_H_


namespace Parsers{

class WtAlignmentParser :
	public TurnLeft::Utils::EnumParser <Wt::AlignmentFlag>
{
public:
	WtAlignmentParser()
	{

	    enumMap["bottom"]           = Wt::AlignBottom;
	    enumMap["baseline"]         = Wt::AlignBaseline;
	    enumMap["top"]              = Wt::AlignTop;
	    enumMap["text top"]         = Wt::AlignTextTop;
	    enumMap["text bottom"]      = Wt::AlignTextBottom;
	    enumMap["left"]             = Wt::AlignLeft;
	    enumMap["center"]           = Wt::AlignCenter;
	    enumMap["middle"]           = Wt::AlignMiddle;
	    enumMap["right"]            = Wt::AlignRight;
	    enumMap["justify"]          = Wt::AlignJustify;
	}
};

class WtAnimationParser:
	public TurnLeft::Utils::EnumParser<Wt::WAnimation::AnimationEffect>
{
public:
	WtAnimationParser()
	{
	    enumMap["slideInFromLeft"]  = Wt::WAnimation::SlideInFromLeft;
	    enumMap["slideInFromRight"] = Wt::WAnimation::SlideInFromRight;
	    enumMap["SlideInFromBottom"] = Wt::WAnimation::SlideInFromBottom;
	    enumMap["SlideInFromTop"] = Wt::WAnimation::SlideInFromTop;
	    enumMap["Pop"] = Wt::WAnimation::Pop;
	    enumMap["Fade"] = Wt::WAnimation::Fade;
	}
};



}//namespace Parsers

#endif /* P_WIDGETSTYLING_H_ */
