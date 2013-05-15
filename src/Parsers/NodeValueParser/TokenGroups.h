/*
 * TokenGroups.h
 *
 *  Created on: March 12th, 2013
 *      Author: cameron
 */

#ifndef TOKENGROUPS_H_
#define TOKENGROUPS_H_

#include <Types/syntax.h>

namespace Parsers {

	inline bool isWidgetToken(MoguSyntax token)
	{
		return __widgetTokens.count(token) == 1;
	}

	inline bool isDBToken(MoguSyntax token)
	{
		return __dbTokens.count(token) == 1;
	}

	inline bool isFlippedActionToken(MoguSyntax token)
	{
		return __flippedActionTokens.count(token) == 1;
	}

	inline bool isObjectToken(MoguSyntax token)
	{
		return __objectTokens.count(token) == 1;
	}

	inline bool isPrepositionToken(MoguSyntax token)
	{
		return __prepositionTokens.count(token) == 1;
	}

	const std::unordered_set<int> __widgetTokens = {
		(int) MoguSyntax::own,
		(int) MoguSyntax::parent,
		(int) MoguSyntax::child,
		(int) MoguSyntax::widget
	};

	const std::unordered_set<int> __dbTokens = {
		(int) MoguSyntax::user,
		(int) MoguSyntax::session,
		(int) MoguSyntax::group,
		(int) MoguSyntax::data
	}
	
	const std::unordered_set<int> __flippedActionTokens = {
		(int) MoguSyntax::remove,
		(int) MoguSyntax::append
	};

	const std::unordered_set<int> __objectTokens = {
		(int) MoguSyntax::own,
		(int) MoguSyntax::parent,
		(int) MoguSyntax::user,
		(int) MoguSyntax::session,
		(int) MoguSyntax::group,
		(int) MoguSyntax::child,
		(int) MoguSyntax::data,
		(int) MoguSyntax::slot,
		(int) MoguSyntax::widget
	};

	const std::unordered_set<int> __prepositionTokens = {
		(int) MoguSyntax::to,
		(int) MoguSyntax::as,
		(int) MoguSyntax::at
	};
};

}	// namespace Parsers

#endif /* TOKENGROUPS_H_ */
