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
	${OBJECTDIR}/QSDb_Public.o \
	${OBJECTDIR}/QSDb_Private.o \
	${OBJECTDIR}/QSDb_Map.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m64
CXXFLAGS=-m64

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libqsdb_core.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libqsdb_core.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libqsdb_core.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libqsdb_core.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libqsdb_core.a

${OBJECTDIR}/QSDb_Public.o: QSDb_Public.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Werror -I../GSSDb_Core -I/usr/include/mysql -I/usr/local/chilkat-9.3.1/include -I../DbBase -MMD -MP -MF $@.d -o ${OBJECTDIR}/QSDb_Public.o QSDb_Public.cpp

${OBJECTDIR}/QSDb_Private.o: QSDb_Private.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Werror -I../GSSDb_Core -I/usr/include/mysql -I/usr/local/chilkat-9.3.1/include -I../DbBase -MMD -MP -MF $@.d -o ${OBJECTDIR}/QSDb_Private.o QSDb_Private.cpp

${OBJECTDIR}/QSDb_Map.o: QSDb_Map.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Werror -I../GSSDb_Core -I/usr/include/mysql -I/usr/local/chilkat-9.3.1/include -I../DbBase -MMD -MP -MF $@.d -o ${OBJECTDIR}/QSDb_Map.o QSDb_Map.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libqsdb_core.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
