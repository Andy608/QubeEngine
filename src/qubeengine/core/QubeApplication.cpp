#include <qubeengine/core/QubeApplication.h>

namespace qe::application
{
	QubeApplication::QubeApplication(const std::string& applicationName) :
		mApplicationName(applicationName)
	{

	}

	const std::string& QubeApplication::getApplicationName() const
	{
		return mApplicationName;
	}

	QubeApplication::~QubeApplication() {}

	void QubeApplication::setApplicationName(const std::string& name)
	{
		mApplicationName = name;
	}
}