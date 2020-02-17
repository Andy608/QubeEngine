#include <iostream>

#include <qubeengine/core/QubeApplication.h>
#include <qubeengine/core/QubeEngine.h>

namespace qe
{
	class QubeEngine::QubeEngineImpl
	{
	public:
		QubeEngineImpl();
		~QubeEngineImpl();

		void cleanupEngine();
		void cleanupApplication();
		void constructApplication();

		bool init();
		bool isInitialized();
		
		void start();
		void engineLoop();
		void update();
		void render();

		memory::MemoryTracker& getMemoryTracker() const;

	private:
		std::unique_ptr<memory::MemoryTracker> mpMemoryTracker;
		std::shared_ptr<application::QubeApplication> mpApplication;

		bool mIsInitialized;
		bool mIsRunning;
	};

	QubeEngine::QubeEngine() :
		mpImpl(std::make_unique<QubeEngineImpl>())
	{

	}

	QubeEngine::~QubeEngine()
	{
		cleanup();
		std::cout << "Destroying Engine" << std::endl;
	}

	bool QubeEngine::isInitialized()
	{
		return mpImpl->isInitialized();
	}

	void QubeEngine::cleanup()
	{
		mpImpl->cleanupEngine();
	}

	void QubeEngine::constructApplication()
	{
		mpImpl->constructApplication();
	}

	void QubeEngine::init()
	{
		mpImpl->init();
	}

	void QubeEngine::constructEngine()
	{
		QubeEngine& engine = instance();
		if (!engine.mpImpl->isInitialized())
		{
			engine.mpImpl->start();
		}
	}

	QubeEngine::QubeEngineImpl::QubeEngineImpl() :
		mIsInitialized(false),
		mIsRunning(false),
		mpApplication(nullptr)
	{

	}

	QubeEngine::QubeEngineImpl::~QubeEngineImpl()
	{
		std::cout << "Destroying Engine Impl" << std::endl;
	}

	void QubeEngine::QubeEngineImpl::cleanupEngine()
	{
		mIsInitialized = false;

		std::cout << "Cleaning up QubeEngine." << std::endl;
		cleanupApplication();

		mpMemoryTracker->printMemoryReport();
	}

	void QubeEngine::QubeEngineImpl::cleanupApplication()
	{
		std::cout << "Cleaning up Qube Application." << std::endl;
		mpApplication->cleanup();
	}

	void QubeEngine::QubeEngineImpl::constructApplication()
	{
		mpApplication = qe::application::createApplication();
		mpApplication->init();
		std::cout << "Successfully constructed and initialized application: " << mpApplication->getApplicationName() << std::endl;
	}

	bool QubeEngine::QubeEngineImpl::init()
	{
		if (!mIsInitialized)
		{
			mpMemoryTracker = std::make_unique<memory::MemoryTracker>();
			mIsInitialized = true;
		}



		return mIsInitialized;
	}

	bool QubeEngine::QubeEngineImpl::isInitialized()
	{
		return mIsInitialized;
	}

	memory::MemoryTracker& QubeEngine::getMemoryTracker() const
	{
		return mpImpl->getMemoryTracker();
	}
	
	void QubeEngine::QubeEngineImpl::start()
	{
		if (init())
		{
			std::cout << "Successfully initialized QubeEngine." << std::endl;
		}

		constructApplication();
		mpApplication->start();
		engineLoop();
	}

	void QubeEngine::QubeEngineImpl::engineLoop()
	{
		std::cout << "Enter Engine Loop." << std::endl;
		update();
		render();
	}

	void QubeEngine::QubeEngineImpl::update()
	{
		std::cout << "Enter Update." << std::endl;
		mpApplication->update();
	}

	void QubeEngine::QubeEngineImpl::render()
	{
		std::cout << "Enter Render." << std::endl;
		mpApplication->render();
	}

	memory::MemoryTracker& QubeEngine::QubeEngineImpl::getMemoryTracker() const
	{
		return *mpMemoryTracker;
	}
}