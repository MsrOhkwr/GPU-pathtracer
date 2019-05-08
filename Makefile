CC         = gcc -std=c11
CXX        = g++ -std=c++14
CFLAGS     = -Wall -Wpedantic -O2 -pthread
INCLUDE    = -I./include
LIBRARY    = 
LIB        = -lglfw -ldl -lopencv_core -lopencv_imgcodecs -lopencv_imgproc
BINDIR     = ./bin
SRCDIR     = ./src
OBJDIR     = ./obj
OUTDIR     = ./output_images
TARGET     = ${BINDIR}/GPU-pathtracer
CSORCES    = ${wildcard ${SRCDIR}/*.c}
CPPSORCES  = ${wildcard ${SRCDIR}/*.cpp}
COBJECTS   = ${addprefix ${OBJDIR}/, ${notdir ${CSORCES:.c=.o}}}
CPPOBJECTS = ${addprefix ${OBJDIR}/, ${notdir ${CPPSORCES:.cpp=.o}}}

${TARGET}: ${BINDIR} ${OBJDIR} ${OUTDIR} ${COBJECTS} ${CPPOBJECTS}
	${CXX} ${CFLAGS} ${LIBRARY} ${COBJECTS} ${CPPOBJECTS} -o ${TARGET} ${LIB}

${BINDIR}:
	mkdir ${BINDIR}

${OBJDIR}:
	mkdir ${OBJDIR}

${OUTDIR}:
	mkdir ${OUTDIR}

${OBJDIR}/%.o: ${SRCDIR}/%.c
	${CC} ${CFLAGS} ${INCLUDE} -o $@ -c $< ${LIB}

${OBJDIR}/%.o: ${SRCDIR}/%.cpp
	${CXX} ${CFLAGS} ${INCLUDE} -o $@ -c $< ${LIB}

clean: 
	rm -rf ${TARGET} ${OBJDIR}/*