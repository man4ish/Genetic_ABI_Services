#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/DtectDisDrug.o \
	${OBJECTDIR}/DtectEffect.o \
	${OBJECTDIR}/DtectGenotype.o \
	${OBJECTDIR}/DtectMapper.o \
	${OBJECTDIR}/DtectMapper_Private.o \
	${OBJECTDIR}/DtectReport.o \
	${OBJECTDIR}/DtectSort.o \
	${OBJECTDIR}/GblPopAveSnp.o \
	${OBJECTDIR}/GblPopSimPatient.o \
	${OBJECTDIR}/GblPopVfCall.o \
	${OBJECTDIR}/GssXmlWriter.o \
	${OBJECTDIR}/QSGenPop.o \
	${OBJECTDIR}/QSPop.o \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=../QSDb_Core/dist/Release/GNU-Linux-x86/libqsdb_core.a ../GSSDb_Core/dist/Release/GNU-Linux-x86/libgssdb_core.a -lxerces-c -lrt -lmysqlpp ../LIMSDb_Core/dist/Release/GNU-Linux-x86/liblimsdb_core.a ../DbBase/dist/Release/GNU-Linux-x86/libdbbase.a -lmysqlclient -lpthread ../TRANSDb_Core/dist/Release/GNU-Linux-x86/libtransdb_core.a ../GSSConfig/dist/Release/GNU-Linux-x86/libgssconfig.a ../TRANSConfig/dist/Release/GNU-Linux-x86/libtransconfig.a ../XmlBase/dist/Release/GNU-Linux-x86/libxmlbase.a ../OpenDb_Core/dist/Release/GNU-Linux-x86/libopendb_core.a ../VarDb_Core/dist/Release/GNU-Linux-x86/libvardb_core.a -dynamic ../GSSLogger/dist/Release/GNU-Linux-x86/libgsslogger.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtector

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtector: ../QSDb_Core/dist/Release/GNU-Linux-x86/libqsdb_core.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtector: ../GSSDb_Core/dist/Release/GNU-Linux-x86/libgssdb_core.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtector: ../LIMSDb_Core/dist/Release/GNU-Linux-x86/liblimsdb_core.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtector: ../DbBase/dist/Release/GNU-Linux-x86/libdbbase.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtector: ../TRANSDb_Core/dist/Release/GNU-Linux-x86/libtransdb_core.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtector: ../GSSConfig/dist/Release/GNU-Linux-x86/libgssconfig.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtector: ../TRANSConfig/dist/Release/GNU-Linux-x86/libtransconfig.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtector: ../XmlBase/dist/Release/GNU-Linux-x86/libxmlbase.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtector: ../OpenDb_Core/dist/Release/GNU-Linux-x86/libopendb_core.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtector: ../VarDb_Core/dist/Release/GNU-Linux-x86/libvardb_core.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtector: ../GSSLogger/dist/Release/GNU-Linux-x86/libgsslogger.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtector: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtector ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/DtectDisDrug.o: DtectDisDrug.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Werror -I../GSSDb_Core -I/usr/include/mysql -I../QSDb_Core -I../LIMSDb_Core -I../DbBase -I../TRANSDb_Core -I../GSSConfig -I../GSSLogger -I../TRANSConfig -I../XmlBase -I../OpenDb_Core -I../VarDb_Core -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/DtectDisDrug.o DtectDisDrug.cpp

${OBJECTDIR}/DtectEffect.o: DtectEffect.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Werror -I../GSSDb_Core -I/usr/include/mysql -I../QSDb_Core -I../LIMSDb_Core -I../DbBase -I../TRANSDb_Core -I../GSSConfig -I../GSSLogger -I../TRANSConfig -I../XmlBase -I../OpenDb_Core -I../VarDb_Core -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/DtectEffect.o DtectEffect.cpp

${OBJECTDIR}/DtectGenotype.o: DtectGenotype.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Werror -I../GSSDb_Core -I/usr/include/mysql -I../QSDb_Core -I../LIMSDb_Core -I../DbBase -I../TRANSDb_Core -I../GSSConfig -I../GSSLogger -I../TRANSConfig -I../XmlBase -I../OpenDb_Core -I../VarDb_Core -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/DtectGenotype.o DtectGenotype.cpp

${OBJECTDIR}/DtectMapper.o: DtectMapper.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Werror -I../GSSDb_Core -I/usr/include/mysql -I../QSDb_Core -I../LIMSDb_Core -I../DbBase -I../TRANSDb_Core -I../GSSConfig -I../GSSLogger -I../TRANSConfig -I../XmlBase -I../OpenDb_Core -I../VarDb_Core -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/DtectMapper.o DtectMapper.cpp

${OBJECTDIR}/DtectMapper_Private.o: DtectMapper_Private.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Werror -I../GSSDb_Core -I/usr/include/mysql -I../QSDb_Core -I../LIMSDb_Core -I../DbBase -I../TRANSDb_Core -I../GSSConfig -I../GSSLogger -I../TRANSConfig -I../XmlBase -I../OpenDb_Core -I../VarDb_Core -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/DtectMapper_Private.o DtectMapper_Private.cpp

${OBJECTDIR}/DtectReport.o: DtectReport.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Werror -I../GSSDb_Core -I/usr/include/mysql -I../QSDb_Core -I../LIMSDb_Core -I../DbBase -I../TRANSDb_Core -I../GSSConfig -I../GSSLogger -I../TRANSConfig -I../XmlBase -I../OpenDb_Core -I../VarDb_Core -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/DtectReport.o DtectReport.cpp

${OBJECTDIR}/DtectSort.o: DtectSort.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Werror -I../GSSDb_Core -I/usr/include/mysql -I../QSDb_Core -I../LIMSDb_Core -I../DbBase -I../TRANSDb_Core -I../GSSConfig -I../GSSLogger -I../TRANSConfig -I../XmlBase -I../OpenDb_Core -I../VarDb_Core -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/DtectSort.o DtectSort.cpp

${OBJECTDIR}/GblPopAveSnp.o: GblPopAveSnp.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Werror -I../GSSDb_Core -I/usr/include/mysql -I../QSDb_Core -I../LIMSDb_Core -I../DbBase -I../TRANSDb_Core -I../GSSConfig -I../GSSLogger -I../TRANSConfig -I../XmlBase -I../OpenDb_Core -I../VarDb_Core -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/GblPopAveSnp.o GblPopAveSnp.cpp

${OBJECTDIR}/GblPopSimPatient.o: GblPopSimPatient.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Werror -I../GSSDb_Core -I/usr/include/mysql -I../QSDb_Core -I../LIMSDb_Core -I../DbBase -I../TRANSDb_Core -I../GSSConfig -I../GSSLogger -I../TRANSConfig -I../XmlBase -I../OpenDb_Core -I../VarDb_Core -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/GblPopSimPatient.o GblPopSimPatient.cpp

${OBJECTDIR}/GblPopVfCall.o: GblPopVfCall.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Werror -I../GSSDb_Core -I/usr/include/mysql -I../QSDb_Core -I../LIMSDb_Core -I../DbBase -I../TRANSDb_Core -I../GSSConfig -I../GSSLogger -I../TRANSConfig -I../XmlBase -I../OpenDb_Core -I../VarDb_Core -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/GblPopVfCall.o GblPopVfCall.cpp

${OBJECTDIR}/GssXmlWriter.o: GssXmlWriter.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Werror -I../GSSDb_Core -I/usr/include/mysql -I../QSDb_Core -I../LIMSDb_Core -I../DbBase -I../TRANSDb_Core -I../GSSConfig -I../GSSLogger -I../TRANSConfig -I../XmlBase -I../OpenDb_Core -I../VarDb_Core -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/GssXmlWriter.o GssXmlWriter.cpp

${OBJECTDIR}/QSGenPop.o: QSGenPop.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Werror -I../GSSDb_Core -I/usr/include/mysql -I../QSDb_Core -I../LIMSDb_Core -I../DbBase -I../TRANSDb_Core -I../GSSConfig -I../GSSLogger -I../TRANSConfig -I../XmlBase -I../OpenDb_Core -I../VarDb_Core -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/QSGenPop.o QSGenPop.cpp

${OBJECTDIR}/QSPop.o: QSPop.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Werror -I../GSSDb_Core -I/usr/include/mysql -I../QSDb_Core -I../LIMSDb_Core -I../DbBase -I../TRANSDb_Core -I../GSSConfig -I../GSSLogger -I../TRANSConfig -I../XmlBase -I../OpenDb_Core -I../VarDb_Core -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/QSPop.o QSPop.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Werror -I../GSSDb_Core -I/usr/include/mysql -I../QSDb_Core -I../LIMSDb_Core -I../DbBase -I../TRANSDb_Core -I../GSSConfig -I../GSSLogger -I../TRANSConfig -I../XmlBase -I../OpenDb_Core -I../VarDb_Core -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtector

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
