/*
 * TokenGenerator.h
 *
 *  Created on: Sep 11, 2012
 *      Author: tom
 */

#ifndef TOKENGENERATOR_H_
#define TOKENGENERATOR_H_

#include <declarations.h>

namespace Parsers{

/*!\brief Yields tokens from a string. Tokens may be defined by a pair of
 * wrappers. A delineator may also be set for the space between tokns. If no
 * token wrappers are present, each whitespace character will be interpreted
 * as a token delineator.
 *
 * For instance, if you give it the string "Mary had a little lamb", the default
 * behaviour would be to tokenize each word of the sentence. However,
 * you could also pass in: "{Mary had} {a little lamb}" and define the
 * bracket pair ('{','}') as a token delineator in order to get the two
 * sentence fragments.
 *
 * When returning tokens, the TokenGenerator assumes that you want them complete,
 * with the wrappings in tact. Therefore, in the above example, '{Mary had}'
 * would be the first token returned.
 *
 * Any number of token delineators may be registered as pairs of chars, and
 * they may be the same char: If you also registerd ('^','^') and ('%','%'),
 * you could pass in the following string for tokenized output:
 * "{Mary} ^had a^ %little% {lamb} ^whose fleece was^ %white% ^as^ {snow}"
 *
 * This would yield each token as expected, and external logic could parse
 * the tokens, saying that '%' wrapped tokens are adjectives, and so forth.
 *
 */
class TokenGenerator
{
	/*!\brief The original string that should be parsed as
	 * various types of tokens.
	 */
	std::string __orig;

	/*!\brief This is the current index of the string's iterator. */
	unsigned int __cpos;

	/*!\brief This is a vector of possible pairs of characters that can
	 * delineate tokens.
	 * \sa add_pair
	 */
	CharCouplets __chars;

protected:

	CharCouplets* getWrappers() { return &__chars;}
	void setCurrentPosition(size_t i) { __cpos = i;}



public:

	/*!\brief Registers a new pair of chars to be used when finding tokens.
	 *
	 * If the generator is iterating over the string "%foo% {bar}", and both
	 * 'foo' and 'bar' are special tokens, the pairs '%','%' and '{','}' must
	 * both be registered. By default the TokenGenerator uses whitespace as its
	 * defacto delineator.
	 *
	 * @param a The opening char
	 * @param b The closing char
	 * @return A reference to the object for method chaining:
	 * 	tgen
	 * 		.add_pair('{','}')
	 * 		.add_pair('$','$')
	 * 		.add_pair('!','@')
	 *
	 */
	inline TokenGenerator& add_pair(char a,char b)
	{
		__chars.push_back( std::make_pair(a,b) );
		return *this;
	}

	/*!\brief Resets the iterator to position 0.
	 *
	 * @return A reference to the object
	 */
	inline TokenGenerator& reset()
	{
		__cpos = 0;
		return *this;
	}

	/*!\brief Defines a new string to be tokenized. Note that it does NOT
	 * reset the iterator position. The Generator does not assume you want
	 * to do this. However, it does make method chaining possible to make
	 * this easy: `tgen.newString(foo).reset();` or `tgen.reset().newString(foo);`
	 *
	 * @param str the new string to be tokenized
	 * @return A reference to the object
	 */
	inline TokenGenerator& newString(std::string str)
	{
		__orig = str;
		return *this;
	}

    inline int getCurrentPosition()
    {
        return __cpos;
    }


	/*!\brief Constructor takes an optional string. Defaults  to the empty
	 * string.
	 * @param orig
	 */
	TokenGenerator (std::string orig = EMPTY);

	/*!\brief Finds the next token. If no wrapped delineators can be found, it
	 * will instead use only the char delineator given as a parameter (defaults
	 * to whitespace).
	 *
	 * If there are no more tokens, it will return "".
	 *
	 * @param delineator (optional) Delineator separating tokens
	 * @return The next token in the string
	 */
	virtual std::string next(char delineator = ' ');

	virtual inline bool isWrapped(const std::string& token) const
	{
		char char0 = token.at(0);
		char fchar = token.at(token.length()-1);
		if (starts_pair(char0) == fchar) return true;
		return false;

	}

	virtual inline char starts_pair(const char& ch) const
	{
		size_t sz = __chars.size();
		for (size_t c = 0; c < sz; c++)
		{
			const std::pair<char,char>* pr = &__chars[c];
			if (ch == pr->first) return pr->second;
		}
		return 0;
	}

	virtual inline char ends_pair(const char& ch) const
	{
		size_t sz = __chars.size();
		for (size_t c = 0; c < sz; c++)
		{
			const std::pair <char,char>* pr = &__chars[c];
			if (ch == pr->second) return pr->first;
		}
		return 0;
	}

	inline std::string getOriginal() { return __orig;}


	virtual ~TokenGenerator(){}
};


} //namespace Parsers


#endif /* TOKENGENERATOR_H_ */
