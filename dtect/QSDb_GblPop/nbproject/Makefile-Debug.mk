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
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/GblPopDownloader.o


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
LDLIBSOPTIONS=../QSDb_Core/dist/Debug/GNU-Linux-x86/libqsdb_core.a ../GSSDb_Core/dist/Debug/GNU-Linux-x86/libgssdb_core.a /usr/local/chilkat-9.3.1/lib/libchilkat-9.3.1.so -lmysqlpp -lxerces-c ../GSSConfig/dist/Debug/GNU-Linux-x86/libgssconfig.a ../DbBase/dist/Debug/GNU-Linux-x86/libdbbase.a ../XmlBase/dist/Debug/GNU-Linux-x86/libxmlbase.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/qsdb_gblpop

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/qsdb_gblpop: ../QSDb_Core/dist/Debug/GNU-Linux-x86/libqsdb_core.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/qsdb_gblpop: ../GSSDb_Core/dist/Debug/GNU-Linux-x86/libgssdb_core.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/qsdb_gblpop: /usr/local/chilkat-9.3.1/lib/libchilkat-9.3.1.so

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/qsdb_gblpop: ../GSSConfig/dist/Debug/GNU-Linux-x86/libgssconfig.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/qsdb_gblpop: ../DbBase/dist/Debug/GNU-Linux-x86/libdbbase.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/qsdb_gblpop: ../XmlBase/dist/Debug/GNU-Linux-x86/libxmlbase.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/qsdb_gblpop: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	g++ -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/qsdb_gblpop ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../QSDb_Core -I../GSSDb_Core -I/usr/include/mysql -I/usr/local/chilkat-9.3.1/include -I../DbBase -I../GSSConfig -I../XmlBase -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/GblPopDownloader.o: GblPopDownloader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../QSDb_Core -I../GSSDb_Core -I/usr/include/mysql -I/usr/local/chilkat-9.3.1/include -I../DbBase -I../GSSConfig -I../XmlBase -MMD -MP -MF $@.d -o ${OBJECTDIR}/GblPopDownloader.o GblPopDownloader.cpp

# Subprojects
.build-subprojects:
	cd ../QSDb_Core && ${MAKE}  -f Makefile CONF=Debug
	cd ../GSSDb_Core && ${MAKE}  -f Makefile CONF=Debug
	cd ../GSSConfig && ${MAKE}  -f Makefile CONF=Debug
	cd ../DbBase && ${MAKE}  -f Makefile CONF=Debug
	cd ../XmlBase && ${MAKE}  -f Makefile CONF=Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/qsdb_gblpop

# Subprojects
.clean-subprojects:
	cd ../QSDb_Core && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../GSSDb_Core && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../GSSConfig && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../DbBase && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../XmlBase && ${MAKE}  -f Makefile CONF=Debug clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
