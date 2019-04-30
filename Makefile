CC      = gcc
CXX     = g++
CFLAGS  = -Wall -Wpedantic -O2 
INCLUDE = -I./include
LIBRARY = 
LIB     = -lglfw -ldl -lopencv_core -lopencv_imgcodecs -lopencv_imgproc
BINDIR  = ./bin
SRCDIR  = ./src
OBJDIR  = ./obj
TARGET  = ${BINDIR}/GPU-pathtracer
CSORCES = ${wildcard ${SRCDIR}/*.c}
CPPSORCES  = ${wildcard ${SRCDIR}/*.cpp}
COBJECTS = ${addprefix ${OBJDIR}/, ${notdir ${CSORCES:.c=.o}}}
CPPOBJECTS = ${addprefix ${OBJDIR}/, ${notdir ${CPPSORCES:.cpp=.o}}}

${TARGET}: ${BINDIR} ${OBJDIR} ${COBJECTS} ${CPPOBJECTS}
	${CXX} ${CFLAGS} ${LIBRARY} ${COBJECTS} ${CPPOBJECTS} -o ${TARGET} ${LIB}

${BINDIR}:
	mkdir ${BINDIR}

${OBJDIR}:
	mkdir ${OBJDIR}

${OBJDIR}/%.o: ${SRCDIR}/%.c
	${CC} ${CFLAGS} ${INCLUDE} -o $@ -c $< ${LIB}

${OBJDIR}/%.o: ${SRCDIR}/%.cpp
	${CXX} ${CFLAGS} ${INCLUDE} -o $@ -c $< ${LIB}

clean: 
	rm -rf ${TARGET} ${OBJDIR}/*