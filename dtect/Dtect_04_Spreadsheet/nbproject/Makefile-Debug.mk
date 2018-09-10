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
	${OBJECTDIR}/atom_helper.o \
	${OBJECTDIR}/client/doc_list_service.o \
	${OBJECTDIR}/client/service.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/util/string_utils.o


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
LDLIBSOPTIONS=../GSSDb_Core/dist/Debug/GNU-Linux-x86/libgssdb_core.a -lmysqlpp -lxerces-c -lmysqlclient -lxml++-2.6 -lcurl `pkg-config --libs libxml++-2.6` ../TRANSDb_Core/dist/Debug/GNU-Linux-x86/libtransdb_core.a ../GSSConfig/dist/Debug/GNU-Linux-x86/libgssconfig.a ../DbBase/dist/Debug/GNU-Linux-x86/libdbbase.a ../XmlBase/dist/Debug/GNU-Linux-x86/libxmlbase.a  

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtect_04_spreadsheet

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtect_04_spreadsheet: ../GSSDb_Core/dist/Debug/GNU-Linux-x86/libgssdb_core.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtect_04_spreadsheet: ../TRANSDb_Core/dist/Debug/GNU-Linux-x86/libtransdb_core.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtect_04_spreadsheet: ../GSSConfig/dist/Debug/GNU-Linux-x86/libgssconfig.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtect_04_spreadsheet: ../DbBase/dist/Debug/GNU-Linux-x86/libdbbase.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtect_04_spreadsheet: ../XmlBase/dist/Debug/GNU-Linux-x86/libxmlbase.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtect_04_spreadsheet: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtect_04_spreadsheet ${OBJECTFILES} ${LDLIBSOPTIONS} -L/usr/lib64/mysql

${OBJECTDIR}/atom_helper.o: atom_helper.cc 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../GSSDb_Core -I/usr/include/mysql -I/usr/include/libxml++-2.6 -I/usr/include/glibmm-2.4 -I/usr/lib64/glibmm-2.4/include -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/lib64/libxml++-2.6/include -I../TRANSDb_Core -I../DbBase -I../GSSConfig -I../XmlBase `pkg-config --cflags libxml++-2.6`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/atom_helper.o atom_helper.cc

${OBJECTDIR}/client/doc_list_service.o: client/doc_list_service.cc 
	${MKDIR} -p ${OBJECTDIR}/client
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../GSSDb_Core -I/usr/include/mysql -I/usr/include/libxml++-2.6 -I/usr/include/glibmm-2.4 -I/usr/lib64/glibmm-2.4/include -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/lib64/libxml++-2.6/include -I../TRANSDb_Core -I../DbBase -I../GSSConfig -I../XmlBase `pkg-config --cflags libxml++-2.6`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/client/doc_list_service.o client/doc_list_service.cc

${OBJECTDIR}/client/service.o: client/service.cc 
	${MKDIR} -p ${OBJECTDIR}/client
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../GSSDb_Core -I/usr/include/mysql -I/usr/include/libxml++-2.6 -I/usr/include/glibmm-2.4 -I/usr/lib64/glibmm-2.4/include -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/lib64/libxml++-2.6/include -I../TRANSDb_Core -I../DbBase -I../GSSConfig -I../XmlBase `pkg-config --cflags libxml++-2.6`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/client/service.o client/service.cc

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../GSSDb_Core -I/usr/include/mysql -I/usr/include/libxml++-2.6 -I/usr/include/glibmm-2.4 -I/usr/lib64/glibmm-2.4/include -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/lib64/libxml++-2.6/include -I../TRANSDb_Core -I../DbBase -I../GSSConfig -I../XmlBase `pkg-config --cflags libxml++-2.6`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/util/string_utils.o: util/string_utils.cc 
	${MKDIR} -p ${OBJECTDIR}/util
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../GSSDb_Core -I/usr/include/mysql -I/usr/include/libxml++-2.6 -I/usr/include/glibmm-2.4 -I/usr/lib64/glibmm-2.4/include -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/lib64/libxml++-2.6/include -I../TRANSDb_Core -I../DbBase -I../GSSConfig -I../XmlBase `pkg-config --cflags libxml++-2.6`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/util/string_utils.o util/string_utils.cc

# Subprojects
.build-subprojects:
	cd ../GSSDb_Core && ${MAKE}  -f Makefile CONF=Debug
	cd ../TRANSDb_Core && ${MAKE}  -f Makefile CONF=Debug
	cd ../GSSConfig && ${MAKE}  -f Makefile CONF=Debug
	cd ../DbBase && ${MAKE}  -f Makefile CONF=Debug
	cd ../XmlBase && ${MAKE}  -f Makefile CONF=Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dtect_04_spreadsheet

# Subprojects
.clean-subprojects:
	cd ../GSSDb_Core && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../TRANSDb_Core && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../GSSConfig && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../DbBase && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../XmlBase && ${MAKE}  -f Makefile CONF=Debug clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
