#
# This file is part of the course materials for AMATH 483/583 at the University of Washington,
# Spring 2019
#
# Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License
# https://creativecommons.org/licenses/by-nc-sa/4.0/
#
# Author: Andrew Lumsdaine
# 


# Choose features here with yes or no
OMP	:= no
THREADS	:= no
PYTHON  := no
CIMG    := no


# These must be set to some value or make will pick defaults (which will be wrong)
CXX	:= mpic++
LANG	:= -std=c++11

TARGETS := grid.exe

grid.exe: grid.o Grid.o Stencil.o cg.o ir.o jacobi.o Final.o mpiStencil.o 

.PHONY : all

include ../include/Makefile.inc

all    : $(TARGETS)

OTHERCLEAN   := $(TARGETS:.exe=.txt) Grid.o Stencil.o cg.o ir.o jacobi.o Final.o mpiStencil.o \
  		*.out*
OTHERSOURCES := ../include/Grid.cpp ../include/Stencil.cpp  ../include/cg.cpp  \
	        ../include/ir.cpp   ../include/jacobi.cpp   Final.cpp mpiStencil.cpp

# The following 4 (yes 4) lines must be in all the subdirectory makefiles
#-----------------------------------------------------------------
# DO NOT DELETE THIS LINE -- make depend uses it
# DEPENDENCIES MUST END AT END OF FILE
grid.o: grid.cpp \
  ../include/Grid.hpp ../include/Stencil.hpp ../include/Timer.hpp \
  ../include/cg.hpp ../include/ir.hpp ../include/jacobi.hpp \
  mpiStencil.hpp Final.hpp
Grid.o: ../include/Grid.cpp ../include/Grid.hpp
Stencil.o: ../include/Stencil.cpp ../include/Stencil.hpp \
  ../include/Grid.hpp
cg.o: ../include/cg.cpp ../include/cg.hpp ../include/Grid.hpp \
  ../include/Stencil.hpp
ir.o: ../include/ir.cpp ../include/ir.hpp ../include/Grid.hpp \
  ../include/Stencil.hpp
jacobi.o: ../include/jacobi.cpp ../include/ir.hpp ../include/Grid.hpp \
  ../include/Stencil.hpp
Final.o: Final.cpp Final.hpp \
  ../include/Grid.hpp mpiStencil.hpp ../include/Stencil.hpp
mpiStencil.o: mpiStencil.cpp \
  ../include/Grid.hpp mpiStencil.hpp
# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
