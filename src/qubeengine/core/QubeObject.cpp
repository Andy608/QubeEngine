#include <typeinfo>

#include <qubeengine/core/QubeObject.h>

namespace qe
{
	int QubeObject::smNextID = -1;

	QubeObject::QubeObject() : 
		mID(++smNextID) {}

	const int& QubeObject::getID() const
	{
		return mID;
	}

	bool QubeObject::equals(const QubeObject& a1, const QubeObject& a2)
	{
		return (typeid(a1) == typeid(a2));
	}
}