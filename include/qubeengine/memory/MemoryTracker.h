#ifndef QUBEENGINE_MEMORY_MEMORYTRACKER_H_
#define QUBEENGINE_MEMORY_MEMORYTRACKER_H_

#include <qubeengine/core/Common.h>

namespace qe::memory
{
	class MemoryTracker : public qe::QubeObject
	{
	public:
		static inline void init() { instance(); }

		MemoryTracker();
		virtual ~MemoryTracker();

		void* allocate(std::size_t size);
		void deallocate(void* pointer) noexcept;
		bool insertMemoryRecord(void* memory, const size_t& size);
		void printMemoryReport();

	private:
		static MemoryTracker& instance()
		{
			static MemoryTracker sInstance;
			return sInstance;
		}

		class MemoryImpl;
		const std::unique_ptr<MemoryImpl> mpImpl;
	};
}

#endif