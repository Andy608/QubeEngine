#ifndef QUBEENGINE_MEMORY_ITRACKABLE_H_
#define QUBEENGINE_MEMORY_ITRACKABLE_H_

#include <memory>

namespace qe::memory
{
	class ITrackable
	{
	public:
		ITrackable() = default;
		virtual ~ITrackable() = default;

		//replaceable allocation functions
		void* operator new	(std::size_t size);
		void* operator new[](std::size_t size);
		void* operator new	(std::size_t size, std::align_val_t alignment);
		void* operator new[](std::size_t size, std::align_val_t alignment);

		//replaceable non-throwing allocation functions
		void* operator new	(std::size_t size, const std::nothrow_t& tag) noexcept;
		void* operator new[](std::size_t size, const std::nothrow_t& tag) noexcept;
		void* operator new	(std::size_t size, std::align_val_t alignment, const std::nothrow_t& tag) noexcept;
		void* operator new[](std::size_t size, std::align_val_t alignment, const std::nothrow_t& tag) noexcept;

		//replaceable usual deallocation functions
		void operator delete  (void* pointer) noexcept;
		void operator delete[](void* pointer) noexcept;
		void operator delete  (void* pointer, std::align_val_t alignment) noexcept;
		void operator delete[](void* pointer, std::align_val_t alignment) noexcept;
		void operator delete  (void* pointer, std::size_t size) noexcept;
		void operator delete[](void* pointer, std::size_t size) noexcept;
		void operator delete  (void* pointer, std::size_t size, std::align_val_t alignment) noexcept;
		void operator delete[](void* pointer, std::size_t size, std::align_val_t alignment) noexcept;

		//replaceable placement deallocation functions
		void operator delete  (void* pointer, const std::nothrow_t& tag) noexcept;
		void operator delete[](void* pointer, const std::nothrow_t& tag) noexcept;
		void operator delete[](void* pointer, std::align_val_t alignment, const std::nothrow_t& tag) noexcept;
		void operator delete  (void* pointer, std::align_val_t alignment, const std::nothrow_t& tag) noexcept;

	private:
		static void* allocateMemory(size_t size);
		static void deallocateMemory(void* pointer);
	};
}

#endif