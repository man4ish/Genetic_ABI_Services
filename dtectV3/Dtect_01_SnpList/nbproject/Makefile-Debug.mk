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
LDLIBSOPTIONS=../GSSDb_Core/dist/Debug/GNU-Linux-x86/libgssdb_core.a -lmysqlpp -lxerces-c -lmysqlclient ../DbBase/dist/Debug/GNU-Linux-x86/libdbbase.a ../GSSConfig/dist/Debug/GNU-Linux-x86/libgssconfig.a ../TRANSDb_Core/dist/Debug/GNU-Linux-x86/libtransdb_core.a ../XmlBase/dist/Debug/GNU-Linux-x86/libxmlbase.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtect_01_snplist

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtect_01_snplist: ../GSSDb_Core/dist/Debug/GNU-Linux-x86/libgssdb_core.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtect_01_snplist: ../DbBase/dist/Debug/GNU-Linux-x86/libdbbase.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtect_01_snplist: ../GSSConfig/dist/Debug/GNU-Linux-x86/libgssconfig.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtect_01_snplist: ../TRANSDb_Core/dist/Debug/GNU-Linux-x86/libtransdb_core.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtect_01_snplist: ../XmlBase/dist/Debug/GNU-Linux-x86/libxmlbase.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtect_01_snplist: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtect_01_snplist ${OBJECTFILES} ${LDLIBSOPTIONS} -L/usr/lib64/mysql

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Werror -I../GSSDb_Core -I/usr/include/mysql -I../DbBase -I../GSSConfig -I../TRANSDb_Core -I../XmlBase -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:
	cd ../GSSDb_Core && ${MAKE}  -f Makefile CONF=Debug
	cd ../DbBase && ${MAKE}  -f Makefile CONF=Debug
	cd ../GSSConfig && ${MAKE}  -f Makefile CONF=Debug
	cd ../TRANSDb_Core && ${MAKE}  -f Makefile CONF=Debug
	cd ../XmlBase && ${MAKE}  -f Makefile CONF=Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtect_01_snplist

# Subprojects
.clean-subprojects:
	cd ../GSSDb_Core && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../DbBase && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../GSSConfig && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../TRANSDb_Core && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../XmlBase && ${MAKE}  -f Makefile CONF=Debug clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
