/*
 * FiniteAutomaton.h
 *
 *  Created on: Oct 24, 2012
 *      Author: tom
 */

#ifndef FINITEAUTOMATON_H_
#define FINITEAUTOMATON_H_

namespace Automaton{

class FiniteAutomaton;

class AbstractState
{
public:
	/*!\brief An Automaton state can be a combination of these things.
	 * Therefore, the enum allows bit masking.
	 */
	enum Type {
		UNDEFINED 	= 0x0
		,START 		= 0x1
		,INTER		= 0x2
		,FINAL		= 0x4
	};
	AbstractState(FiniteAutomaton* automaton) : __automaton(automaton)
	{ __type = UNDEFINED;}

	inline const uint8_t& getType() { return __type;}

protected:
	FiniteAutomaton* __automaton;
	uint8_t __type;
};

class FiniteAutomaton
{
public:
	enum Status{
		ERR_INVALID	= 0x0 //<! Input invalid
		,ERR_ICPL 	= 0x1 //<! Input finished before end state reached
		,ERR_ECPL 	= 0x2 //<! End state reached but input persists
		,OK_ICPL	= 0x3 //<! Valid input, end state not yet reached
		,OK_CPL		= 0x4 //<! Valid input, end state reached
	};


	FiniteAutomaton(){}
	virtual ~FiniteAutomaton() {}
	virtual bool hasNextToken()  =0;
};

/* T is the type that will be sent through the finite state machine.
 * U is the result type that should be parseable from type T.
 * V is the indexing type that will be used to map states for easy access
 * during development.*/
template <class T, class U=T, class V=std::string>
class FiniteAutomatonTmpl : public FiniteAutomaton
{
protected:
	Status __status;
	AbstractState* __starting_state;
	std::map <V, AbstractState*> __states;

public:
	FiniteAutomatonTmpl() : FiniteAutomaton()
	{
		__status = ERR_INVALID;
		__starting_state =0;
	}

	virtual ~FiniteAutomatonTmpl(){}
	virtual bool hasNextToken() =0;
	virtual U giveInput(const T& input) =0;

	inline Status getStatus() { return __status; }

	inline void registerState(V index, AbstractState* state)
	{
		__states[index] = state;
		if (state->getType() & AbstractState::START)
		{
			if (__starting_state != 0) return;
			__starting_state = state;
		}
	}

	inline AbstractState* getState(const V& index)
	{
		return __states[index];
	}

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

template <class T, class U, class E, class V=uint32_t>
class State : public AbstractState
{
public:

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

	FiniteAutomaton::Status input(const U& input)
	{
		output_type = (T) parse(input);

		if (!(output_type & __valid_outputs)	)
			return FiniteAutomaton::ERR_INVALID;

		/* If this is an intermediate node that must continue */
		if (__type == INTER)
		{

			if (__automaton->hasNextToken()) return FiniteAutomaton::OK_ICPL;
			else return FiniteAutomaton::ERR_ICPL;
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
	inline AbstractState* destination (T type)
	{
		return __destinations[type];
	}

	inline State<T,U,E,V>& registerDestination(T type, AbstractState* other)
	{
		__destinations[type] = other;
		return *this;
	}

	inline E getID() { return __name; }
	inline const T& getOutputType () { return output_type; }
	T (*parse)(const U& reagent);

private:
	T output_type;
	const V& __valid_outputs;
	E __name;
	std::map  <T,AbstractState*> __destinations;
};


}//namespace Automaton



#endif /* FINITEAUTOMATON_H_ */
