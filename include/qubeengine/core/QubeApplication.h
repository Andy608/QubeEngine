#ifndef QUBEENGINE_CORE_QUBEENGINEAPPLICATION_H_
#define QUBEENGINE_CORE_QUBEENGINEAPPLICATION_H_

#include <string>

#include <qubeengine/core/Common.h>

namespace qe::application
{

	class QubeApplication : public qe::QubeObject
	{
		friend class QubeEngine;

	public:
		QubeApplication(const std::string& applicationName = "My Qube Application");
		virtual ~QubeApplication();

		virtual void init() = 0;
		virtual void start() = 0;
		virtual void update() = 0;
		virtual void render() = 0;
		virtual void cleanup() = 0;

		const std::string& getApplicationName() const;

	protected:

		void setApplicationName(const std::string& name);

	private:
		std::string mApplicationName;
	};

	//Forward declare for applications to override.
	std::shared_ptr<QubeApplication> createApplication();
}

#endif