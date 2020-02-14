#ifndef QUBEENGINE_CORE_QUBEOBJECT_H_
#define QUBEENGINE_CORE_QUBEOBJECT_H_

#include <qubeengine/memory/ITrackable.h>

namespace qe
{
	struct QubeObject : public qe::memory::ITrackable
	{
	public:
		const int& getID() const;

		inline bool operator==(const QubeObject& other) const { return mID == other.mID; }
		inline bool operator!=(const QubeObject& other) const { return mID != other.mID; }
		inline bool operator< (const QubeObject& other)	const { return mID < other.mID;	 }
		inline bool operator> (const QubeObject& other)	const { return mID > other.mID;  }
		inline bool operator<=(const QubeObject& other) const { return mID <= other.mID; }
		inline bool operator>=(const QubeObject& other) const { return mID >= other.mID; }

		static bool equals(const QubeObject& a1, const QubeObject& a2);

	protected:
		QubeObject();
		virtual ~QubeObject() = default;

	private:
		static int smNextID;
		int mID;
	};
}

#endif