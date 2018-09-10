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
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/DirHandler.o \
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
LDLIBSOPTIONS=../QSDb_Core/dist/Debug/GNU-Linux-x86/libqsdb_core.a -lxerces-c -lrt -lmysqlpp ../LIMSDb_Core/dist/Debug/GNU-Linux-x86/liblimsdb_core.a ../DbBase/dist/Debug/GNU-Linux-x86/libdbbase.a -lmysqlclient -lpthread ../TRANSDb_Core/dist/Debug/GNU-Linux-x86/libtransdb_core.a ../GSSLogger/dist/Debug/GNU-Linux-x86/libgsslogger.a ../GSSConfig/dist/Debug/GNU-Linux-x86/libgssconfig.a ../XmlBase/dist/Debug/GNU-Linux-x86/libxmlbase.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtectparser

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtectparser: ../QSDb_Core/dist/Debug/GNU-Linux-x86/libqsdb_core.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtectparser: ../LIMSDb_Core/dist/Debug/GNU-Linux-x86/liblimsdb_core.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtectparser: ../DbBase/dist/Debug/GNU-Linux-x86/libdbbase.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtectparser: ../TRANSDb_Core/dist/Debug/GNU-Linux-x86/libtransdb_core.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtectparser: ../GSSLogger/dist/Debug/GNU-Linux-x86/libgsslogger.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtectparser: ../GSSConfig/dist/Debug/GNU-Linux-x86/libgssconfig.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtectparser: ../XmlBase/dist/Debug/GNU-Linux-x86/libxmlbase.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtectparser: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtectparser ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/DirHandler.o: DirHandler.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include/mysql -I../QSDb_Core -I../LIMSDb_Core -I../DbBase -I../GSSLogger -I../GSSConfig -I../XmlBase -MMD -MP -MF $@.d -o ${OBJECTDIR}/DirHandler.o DirHandler.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include/mysql -I../QSDb_Core -I../LIMSDb_Core -I../DbBase -I../GSSLogger -I../GSSConfig -I../XmlBase -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:
	cd ../QSDb_Core && ${MAKE}  -f Makefile CONF=Debug
	cd ../LIMSDb_Core && ${MAKE}  -f Makefile CONF=Debug
	cd ../DbBase && ${MAKE}  -f Makefile CONF=Debug
	cd ../TRANSDb_Core && ${MAKE}  -f Makefile CONF=Debug
	cd ../GSSLogger && ${MAKE}  -f Makefile CONF=Debug
	cd ../GSSConfig && ${MAKE}  -f Makefile CONF=Debug
	cd ../XmlBase && ${MAKE}  -f Makefile CONF=Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtectparser

# Subprojects
.clean-subprojects:
	cd ../QSDb_Core && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../LIMSDb_Core && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../DbBase && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../TRANSDb_Core && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../GSSLogger && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../GSSConfig && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../XmlBase && ${MAKE}  -f Makefile CONF=Debug clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
