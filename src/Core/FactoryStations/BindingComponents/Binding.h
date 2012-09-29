/*
 * Binding.h
 *
 *  Created on: Sep 28, 2012
 *      Author: tom
 */

#ifndef BINDING_H_
#define BINDING_H_

namespace Goo{
namespace MoldableFactory{

#include <Wt/WObject>
#include <Wt/WSignal>

class Binding : public Wt::WObject
{
	std::vector <Moldable*> listeners;
	Nodes::NodeValue* payload;
	Moldable* broadcaster;
	void (*callback)(Binding*);
	Wt::Signal<>* signal;

	inline void executeCallback()
	{
		callback(this);
	}

	bool isReady()
	{
		return(
				(listeners.size() > 0)
				&& (payload != 0)
				&& (broadcaster != 0)
				&& (callback != 0)
				&& (signal != 0)
				);
	}

	bool executeBind()
	{
		if (!isReady()) return false;
		signal->connect(this, &Binding::executeCallback);
		return true;
	}

public:
	Binding() : Wt::WObject(), listeners()
	{
		payload =0;
		broadcaster =0;
		callback =0;
		signal =0;
	}

	inline Binding& set (std::vector <Moldable>* vec)
	{
		listeners = vec;
		executeBind();
		return *this;
	}

	inline Binding& set (Nodes::NodeValue* pl)
	{
		payload = pl;
		executeBind();
		return *this;
	}

	inline Binding& set (Moldable* bc)
	{
		broadcaster = bc;
		executeBind();
		return *this;
	}

	inline Binding& set (Wt::Signal<>* sg)
	{
		signal = sg;
		executeBind();
		return *this;
	}

	inline Binding& set (BinderyCallbacks::callback cb)
	{
		callback = cb;
		executeBind();
		return *this;
	}
};

}//namespace MoldableFactory
}//namespace Goo
#endif /* BINDING_H_ */
