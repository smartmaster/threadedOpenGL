#include "XSurfaceFormat.h"

void XSurfaceFormatUtils::SurfaceFormat()
{
	QSurfaceFormat defaultFormat;
	defaultFormat.setVersion(4, 5);
	defaultFormat.setProfile(QSurfaceFormat::CoreProfile);
	defaultFormat.setOptions(QSurfaceFormat::DebugContext);
	QSurfaceFormat::setDefaultFormat(defaultFormat);
}
