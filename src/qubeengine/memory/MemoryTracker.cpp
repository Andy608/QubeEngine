#include <iostream>
#include <map>
#include <string>

#include <qubeengine/memory/MemoryTracker.h>

namespace qe::memory
{
	class MemoryTracker::MemoryImpl
	{
	public:
		void* allocate(std::size_t size);
		void deallocate(void* pointer) noexcept;
		bool insertMemoryRecord(void* memory, const size_t& size);
		void printMemoryReport() const;

	protected:
		struct MemoryRecord
		{
			const int& id;
			size_t memSize;

			MemoryRecord() = delete;
			MemoryRecord(const int& id, const size_t& size) : id(id), memSize(size) {};
		};

		static int smNextID;

		std::map<void*, MemoryRecord> mMemoryRecords;
	};

	int MemoryTracker::MemoryImpl::smNextID = -1;

	void* MemoryTracker::MemoryImpl::allocate(std::size_t size)
	{
		void* memory = malloc(size);
		if (memory)
		{
			if (insertMemoryRecord(memory, size))
			{
				return memory;
			}
		}

		return nullptr;
	}

	void MemoryTracker::MemoryImpl::deallocate(void* pointer) noexcept
	{
		auto iter = mMemoryRecords.find(pointer);
		if (iter != mMemoryRecords.end())
		{
			mMemoryRecords.erase(iter);
		}

		free(pointer);
	}

	bool MemoryTracker::MemoryImpl::insertMemoryRecord(void* memory, const size_t& size) 
	{
		if (memory)
		{
			std::pair p = mMemoryRecords.insert(std::make_pair(memory, MemoryRecord(++smNextID, size)));
			return p.second;
		}

		std::cerr << "Could not insert memory record." << std::endl;
		return false;
	}

	void MemoryTracker::MemoryImpl::printMemoryReport() const
	{
		if (mMemoryRecords.empty())
		{
			std::cout << "All memory is free." << std::endl;
		}
		else
		{
			std::cerr << "Leftover memory leaked:" << std::endl;
			std::cout << mMemoryRecords.size() << " entries being tracked:" << std::endl;

			auto iter = mMemoryRecords.begin();
			while (iter != mMemoryRecords.end())
			{
				std::cout << "Id: " + std::to_string(iter->second.id) + " Size: " + std::to_string(iter->second.memSize) << std::endl;
				iter++;
			}
		}
	}

	MemoryTracker::MemoryTracker() :
		qe::QubeObject(),
	mpImpl(std::make_unique<MemoryImpl>()) {}

	MemoryTracker::~MemoryTracker() = default;

	void* MemoryTracker::allocate(std::size_t size)
	{
		return mpImpl->allocate(size);
	}

	void MemoryTracker::deallocate(void* pointer) noexcept
	{
		mpImpl->deallocate(pointer);
	}

	bool MemoryTracker::insertMemoryRecord(void* memory, const size_t& size)
	{
		return mpImpl->insertMemoryRecord(memory, size);
	}
	
	void MemoryTracker::printMemoryReport()
	{
		mpImpl->printMemoryReport();
	}
}