#!/bin/sh
cd DbBase
make CONF=Release clean all
cd ../GSSConfig
make CONF=Release clean all
cd ../GSSDb_Core
make CONF=Release clean all
cd ../GSSLogger
make CONF=Release clean all
cd ../LIMSDb_Core
make CONF=Release clean all
cd ../OpenDb_Core
make CONF=Release clean all
cd ../QSDb_Core
make CONF=Release clean all
cd ../TRANSConfig
make CONF=Release clean all
cd ../TRANSDb_Core
make CONF=Release clean all
cd ../VarDb_Core
make CONF=Release clean all
cd ../XmlBase
make CONF=Release clean all
cd ../Dtect_01_SnpList
make CONF=Release clean all
cd ../Dtect_02_GenoEffect
make CONF=Release clean all
cd ../Dtector
make CONF=Release clean all
cd ../DtectParser
make CONF=Release clean all
cd ..
