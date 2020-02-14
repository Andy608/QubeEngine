#ifndef QUBEENGINE_CORE_QUBEENGINE_H_
#define QUBEENGINE_CORE_QUBEENGINE_H_

#include <qubeengine/core/Common.h>
#include <qubeengine/memory/MemoryTracker.h>

namespace qe
{
	class QubeEngine : public qe::QubeObject
	{
		friend class qe::memory::ITrackable;

	public:
		static void constructEngine();

		QubeEngine(const QubeEngine&) = delete;
		QubeEngine& operator=(const QubeEngine&) = delete;

		QubeEngine(QubeEngine&& other) = delete;
		QubeEngine& operator=(QubeEngine&& other) = delete;

		bool isInitialized();

		memory::MemoryTracker& getMemoryTracker() const;

	private:
		static QubeEngine& instance()
		{
			static QubeEngine sInstance;
			return sInstance;
		}

		void cleanup();
		void constructApplication();
		void init();

		class QubeEngineImpl;
		const std::unique_ptr<QubeEngineImpl> mpImpl;

		QubeEngine();
		virtual ~QubeEngine();
	};
}

#endif