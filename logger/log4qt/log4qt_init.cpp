/*****************************************************************
  initialize log4qt by default config file:log4qt.properties
*****************************************************************/

#include "propertyconfigurator.h"

namespace Log4Qt
{
    int init()
    {
        Log4Qt::PropertyConfigurator::configure("log4qt.properties");
        return 0;
    }

    static int ret = Log4Qt::init();
}
