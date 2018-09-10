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
	${OBJECTDIR}/GSSDb_Search.o \
	${OBJECTDIR}/GSSDb_Public.o \
	${OBJECTDIR}/VerSvn.o \
	${OBJECTDIR}/GSSDb_Private.o


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
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libgssdb_core.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libgssdb_core.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libgssdb_core.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libgssdb_core.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libgssdb_core.a

${OBJECTDIR}/GSSDb_Search.o: GSSDb_Search.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -Werror -I/usr/include/mysql -I../DbBase -MMD -MP -MF $@.d -o ${OBJECTDIR}/GSSDb_Search.o GSSDb_Search.cpp

${OBJECTDIR}/GSSDb_Public.o: GSSDb_Public.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -Werror -I/usr/include/mysql -I../DbBase -MMD -MP -MF $@.d -o ${OBJECTDIR}/GSSDb_Public.o GSSDb_Public.cpp

${OBJECTDIR}/VerSvn.o: VerSvn.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -Werror -I/usr/include/mysql -I../DbBase -MMD -MP -MF $@.d -o ${OBJECTDIR}/VerSvn.o VerSvn.cpp

${OBJECTDIR}/GSSDb_Private.o: GSSDb_Private.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -Werror -I/usr/include/mysql -I../DbBase -MMD -MP -MF $@.d -o ${OBJECTDIR}/GSSDb_Private.o GSSDb_Private.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libgssdb_core.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
