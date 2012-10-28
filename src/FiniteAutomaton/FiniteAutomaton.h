/*
 * FiniteAutomaton.h
 *
 *  Created on: Oct 24, 2012
 *      Author: tom
 */

#ifndef FINITEAUTOMATON_H_
#define FINITEAUTOMATON_H_

/* This is a set of templates and abstract classes for creating
 * a finite automaton.
 */

namespace Automaton{

//Forward declaration.
class FiniteAutomaton;

/*!\brief A shell class of an automaton state, or node. This provides the bare
 * minimum needed for polymorphic activites on states, without having to know
 * the templated details.
 */
class AbstractState
{
public:
	/*!\brief An Automaton state can be a combination of these things.
	 * Therefore, the enum is bit aligned and allows masking. Any tests
	 * for different state types will mask the type.
	 */
	enum Type {
		UNDEFINED 	= 0x0
		,START 		= 0x1
		,INTER		= 0x2
		,FINAL		= 0x4
	};

	/*!\brief The most basic of constructors requires at the minimum a pointer
	 * to an Automaton. This is necessary for doing lookaheads.
	 * @param automaton A pointer to the automaton that houses this state.
	 */
	AbstractState(FiniteAutomaton* automaton) : __automaton(automaton)
	{ __type = UNDEFINED;}

	virtual ~AbstractState(){}

	/*!\brief Provides an accessor to this state's type. Note that the type
	 * will not necessarily be one of the defined types above, but because
	 * of the bit masking can be a combination of them, which is why it
	 * returns a uint8_t, the smallest possible data value.
	 * @return
	 */
	inline const uint8_t& getType() { return __type;}

protected:

	/*!\brief The automaton which houses this state.*/
	FiniteAutomaton* __automaton;

	/*!\brief The type of this state. */
	uint8_t __type;
};


/*!\brief The base abstraction for all finite automata.
 */
class FiniteAutomaton
{
public:
	/*!\brief The status of the automaton at any given time. This is directly
	 * influenced by the states, and the tests within them. On any error,
	 * the machine stops processing. If OK_CPL is reached, the machine stops
	 * and it's a good thing. Otherwise (OK_ICPL), the machine continues.
	 */
	enum Status{
		ERR_INVALID	= 0x0 //<! Input invalid
		,ERR_ICPL 	= 0x1 //<! Input finished before end state reached
		,ERR_ECPL 	= 0x2 //<! End state reached but input persists
		,OK_ICPL	= 0x3 //<! Valid input, end state not yet reached
		,OK_CPL		= 0x4 //<! Valid input, end state reached
	};

	FiniteAutomaton(){}
	virtual ~FiniteAutomaton() {}

	//!\brief All derived classes must provide a way to look ahead!
	virtual bool hasNextToken()  =0;
};

/*!\brief This is the template for building an automaton.
 *
 * template arg T is the type that the automaton will accept as input.
 * template arg U is the type that the automaton will return, and defaults
 * to type T. If U is different than T, the 'giveInput()' function must handle
 * any necessary conversion.
 * template arg V is the means with which the automaton will index its different
 * states, so that it knows how to traverse the pathway from input->output. This
 * defaults to a string, but an enumerated type is recommended.
 */
template <class T, class U=T, class V=std::string>
class FiniteAutomatonTmpl : public FiniteAutomaton
{
protected:
	/*!\brief The automaton's current status. */
	Status __status;

	/*!\brief The initial state. There can only be one starting state
	 * per automaton.
	 */
	AbstractState* __starting_state;

	/*!\brief This is the map that correlates your states to some identifier,
	 * and how you will implement the machine traversal in the giveInput
	 * virtual method.
	 */
	std::map <V, AbstractState*> __states;

public:

	/*!\brief The default constructor sets the status to invalid, and nullifies
	 * the starting state.
	 */
	FiniteAutomatonTmpl() : FiniteAutomaton()
	{
		__status = ERR_INVALID;
		__starting_state =0;
	}

	virtual ~FiniteAutomatonTmpl(){}

	//!\brief The responsibility falls to the next level for implementation of this function.
	virtual bool hasNextToken() =0;

	/*!\brief This function is where all of the magic happens, and
	 * needs to contain an iterative loop that returns the final output.
	 * Any errors that should be thrown by your automaton should be thrown
	 * in this member function in order to be caught by callers.
	 * @param input The initial data to be sent through the machine.
	 * @return The final output.
	 */
	virtual U giveInput(T input) =0;

	/*!\brief provides an accessor for the machine's status. */
	inline Status getStatus() { return __status; }

	/*!\brief Allows states to be added into the machine.
	 *
	 * @param index The identifier for the state
	 * @param state A pointer to the state itself.
	 */
	inline void registerState(V index, AbstractState* state)
	{
		__states[index] = state;
		if (state->getType() & AbstractState::START)
		{
			if (__starting_state != 0) return;
			__starting_state = state;
		}
	}

	/*!\brief Provides a generic pointer to any state type. */
	inline AbstractState* getState(const V& index)
	{
		return __states[index];
	}

	/*!\brief Provides a reference to a specific state implementation. */
	template <class S>
	S& getState(const V& index)
	{
		AbstractState* s = getState(index);
		S* cast = (S*) s;
		S& ref = *cast;
		return ref;
	}
};


/* T is a bit-aligned enumerated type.
 * U is the type that will be used in the callback -- there must be a way to
 * parse that type into Type T!
 * V is an unsigned integral type that is large enough to mask any bit from
 * type T, which is to say all possible outputs. It defaults to uint32_t --
 * allowing 32 different outputs!
 */

/*!\brief The template for automaton states.
 *
 * template arg T must be a bit-aligned enumerated type. This is used in order
 * to handle traversal.
 *
 * For example, if you have the following enum:
 *
 * 		enum Foo {
 * 			alpha	=0x1
 * 			,beta	=0x2
 * 			,gamma	=0x4
 * 			,delta	=0x8
 * 		};
 *
 * You can specify which paths are valid next steps by passing in a value
 * corresponding to those bits. if 'alpha', 'gamma', and 'delta' are all valid
 * outputs, you would pass in `1011`, and then register a state to each
 * alpha, beta, and gamma to be used to continue the traversal.
 *
 *
 * template arg U is the whatever type you will be passing into the actual
 * parse test. The parse test must somehow convert this value into type T.
 *
 * template arg V is an optional argument which must contain at least as many
 * bits as there are states in your machine. Using the above enum, for example,
 * you could use a uint8_t. However, the default (uint32_t) would also handle
 * this, it would just take up 4 times the space.
 */
template <class T, class U, class E, class V=uint32_t>
class State : public AbstractState
{
public:

	/*!\brief The default constructor for the automaton state.
	 *
	 * @param automaton The machine in which this state resides.
	 * @param state_type The type of state--can be a combination of several.
	 * @param state_name The identifier for this state which is also used in the automaton index.
	 * @param validOutputs An integral type which has corresponding bits that are "on"
	 * depending on what output paths are valid. Defaults to 0.
	 * @param parse_callback A function which accepts type U and returns type T, as
	 * long as T can be masked aganist V.
	 */
	State(
			FiniteAutomaton* automaton
			, uint8_t state_type
			, const E& state_name
			, const V& validOutputs=0
			, T (*parse_callback)(const U&)=0
	) :
		AbstractState(automaton),
		__valid_outputs(validOutputs),
		__name(state_name)
	{
		parse = parse_callback;
		__type = state_type;
	}

	/*!\brief The main function of an automaton state. It takes input, and uses
	 * that input with the assigned parse function. The output is then
	 * analyzed against the valid outputs, and it notifies the automaton
	 * of the status based on what it knows.
	 * @param input The input.
	 * @return The automaton status.
	 */
	FiniteAutomaton::Status input(const U& input)
	{
		output_type = (T) parse(input);

		if (!(output_type & __valid_outputs)	)
			return FiniteAutomaton::ERR_INVALID;

		/* If this is an intermediate node that must continue */
		if (__type == INTER)
		{
			if (__automaton->hasNextToken()) return FiniteAutomaton::OK_ICPL;
			else
			{
				AbstractState* v = destination(output_type);
				if (v->getType() & FINAL)
				{
					return FiniteAutomaton::OK_CPL;
				}
				return FiniteAutomaton::ERR_ICPL;
			}
		}

		/* If this is a final node that may not continue */
		else if (__type == FINAL)
		{
			if (__automaton->hasNextToken()) return FiniteAutomaton::ERR_ECPL;
			else return FiniteAutomaton::OK_CPL;
		}

		/* If this is an intermediate node that CAN be a final node */
		else
		{
			if (__automaton->hasNextToken())
				return FiniteAutomaton::OK_ICPL;
			else
				return FiniteAutomaton::OK_CPL;
		}
	}

	/*!\brief Returns the state that can be used as the next path for
	 * input. Note that this limits a one-to-one input type and state, meaning
	 * this is only valid for DETERMINISTIC MACHINES.
	 * @param type The type identifier of the output from this state.
	 * @return The next logical step in the process.
	 */
	inline AbstractState* destination (T type)
	{
		return __destinations[type];
	}

	/*!\brief Adds an output path to this state.  If the test for this path
	 * is of type T, and is valid, the path based on type T can then be called.
	 * It is perfectly valid for a state to have no outputs, if it is a final
	 * state.
	 *
	 * @param type	The output identifier for this type.
	 * @param other	The state to be called if output A is of type T.
	 * @return a reference to this object for function chaining.
	 */
	inline State<T,U,E,V>& registerDestination(T type, AbstractState* other)
	{
		__destinations[type] = other;
		return *this;
	}

	/*!\brief Returns this state's identifier. */
	inline E getID() { return __name; }

	/*!\brief Returns the output type determined after this state was called. */
	inline const T& getOutputType () { return output_type; }

	/*!\brief A function that allows input to be tested. */
	T (*parse)(const U& reagent);

private:

	/*!\brief After testing, this will hold the result of the test. */
	T output_type;

	/*!\brief An integral type whose "on" bits represent valid output paths. */
	const V& __valid_outputs;

	/*!\brief This state's identifier. */
	E __name;

	/*!\brief All possible paths out of this state. */
	std::map  <T,AbstractState*> __destinations;
};


}//namespace Automaton



#endif /* FINITEAUTOMATON_H_ */
