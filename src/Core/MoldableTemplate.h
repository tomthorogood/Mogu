/*
 * MoldableTemplate.h
 *
 *  Created on: Sep 27, 2012
 *      Author: tom
 */

#ifndef MOLDABLETEMPLATE_H_
#define MOLDABLETEMPLATE_H_

#include <declarations.h>

namespace Goo
{

struct MoldableTemplate
{
	uint16_t	flags;
	uint16_t	num_children;
	masked 		actionBlocking;
	mapped 		type;
	std::string location;
	std::string source;
	std::string content;
	std::string style;
	std::string name;
	std::string node;
	bool deleteable;
	uint16_t	num_connected_widgets;


	MoldableTemplate() {
		flags 			=0;
		actionBlocking 	=0;
		type			=0;
		num_children	=0;
		num_connected_widgets =0;
		location		=EMPTY;
		source			=EMPTY;
		content			=EMPTY;
		style			=EMPTY;
		deleteable 		=false;
	}

	inline void disconnect()
	{
		if (--num_connected_widgets <= 0) deleteable = true;
	}
};

////////////////////////////////////////////////////////
/*!\brief Holds the information about the widget retrieved from the database
 * so that the widget doesn't actually build the information until it is
 * necessary.
 */
struct GooVariables
{
    Redis::strvector children;

    /*!\brief Corresponds to a core set of eight widget properties. */
    masked flags; //!<\sa Enums::SignalTypes::Properties

    /*!\brief Determines which actions are blocked by the widget in qusetion. */
    masked actionBlocking;

    /*!\brief The integral type of the widget in question. See the ByteMaps
     * spreadsheet in dev_resources for more information.
     */
    mapped type;

    bool dynamic; //TODO This needs to be worked into 'flags'.

    std::string location;    //!< Used for external links only.
    std::string source;      //!< URI for image file, if applicable.

    /*!\brief This field can contain any number of things. For text widgets,
     * this represents the text (or html/text combo) that will be displayed
     * on the user's screen. For images, this represents the 'alt' tag. For
     * links, this is the anchor text.
     */
    std::string content;

    GooVariables();
};


}//namespace Goo
#endif /* MOLDABLETEMPLATE_H_ */
