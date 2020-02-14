#include <qubeengine/core/QubeEngine.h>
#include <qubeengine/memory/ITrackable.h>

namespace qe::memory
{
	void* ITrackable::allocateMemory(size_t size)
	{
		void* memory = nullptr;

		QubeEngine& engine = QubeEngine::instance();

		if (engine.isInitialized())
		{
			memory = engine.getMemoryTracker().allocate(size);
		}
		else
		{
			memory = malloc(size);
		}

		return memory;
	}

	void ITrackable::deallocateMemory(void* pointer)
	{
		QubeEngine& engine = QubeEngine::instance();

		if (engine.isInitialized())
		{
			engine.getMemoryTracker().deallocate(pointer);
		}
		else
		{
			free(pointer);
		}
	}

	//replaceable allocation functions
	void* ITrackable::operator new  (std::size_t size) { return allocateMemory(size); }
	void* ITrackable::operator new[](std::size_t size) { return allocateMemory(size); }
	void* ITrackable::operator new	(std::size_t size, std::align_val_t alignment) { return allocateMemory(size); }
	void* ITrackable::operator new[](std::size_t size, std::align_val_t alignment) { return allocateMemory(size); }

		//replaceable non-throwing allocation functions
	void* ITrackable::operator new	(std::size_t size, const std::nothrow_t& tag) noexcept { return allocateMemory(size); }
	void* ITrackable::operator new[](std::size_t size, const std::nothrow_t& tag) noexcept { return allocateMemory(size); }
	void* ITrackable::operator new	(std::size_t size, std::align_val_t alignment, const std::nothrow_t& tag) noexcept { return allocateMemory(size); }
	void* ITrackable::operator new[](std::size_t size, std::align_val_t alignment, const std::nothrow_t& tag) noexcept { return allocateMemory(size); }

		//replaceable usual deallocation functions
	void ITrackable::operator delete  (void* pointer) noexcept { deallocateMemory(pointer); }
	void ITrackable::operator delete[](void* pointer) noexcept { deallocateMemory(pointer); }
	void ITrackable::operator delete  (void* pointer, std::align_val_t alignment) noexcept { deallocateMemory(pointer); }
	void ITrackable::operator delete[](void* pointer, std::align_val_t alignment) noexcept { deallocateMemory(pointer); }
	void ITrackable::operator delete  (void* pointer, std::size_t size) noexcept { deallocateMemory(pointer); }
	void ITrackable::operator delete[](void* pointer, std::size_t size) noexcept { deallocateMemory(pointer); }
	void ITrackable::operator delete  (void* pointer, std::size_t size, std::align_val_t alignment) noexcept { deallocateMemory(pointer); }
	void ITrackable::operator delete[](void* pointer, std::size_t size, std::align_val_t alignment) noexcept { deallocateMemory(pointer); }

		//replaceable placement deallocation functions
	void ITrackable::operator delete  (void* pointer, const std::nothrow_t& tag) noexcept { deallocateMemory(pointer); }
	void ITrackable::operator delete[](void* pointer, const std::nothrow_t& tag) noexcept { deallocateMemory(pointer); }
	void ITrackable::operator delete[](void* pointer, std::align_val_t alignment, const std::nothrow_t& tag) noexcept { deallocateMemory(pointer); }
	void ITrackable::operator delete  (void* pointer, std::align_val_t alignment, const std::nothrow_t& tag) noexcept { deallocateMemory(pointer); }
}