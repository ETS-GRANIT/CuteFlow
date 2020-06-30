SHELL:=/bin/bash
SRC = ../src
BIN = ../bin

CUTEFLAGS = -Mcuda=cc60 -O3
#CUTEFLAGS = -Mcuda=cc60 -O0 -g -Mfptrap=zero,overflow,underflow -Mbounds -Mdclchk -Mchkptr -Mchkstk -Meh_frame, -Minform,inform
CUTECOMPILER = mpif90 

CUTEFILES = mpiDeviceUtil.cuf precision.cuf global_data.cuf global_data_constant_device.cuf data_transfer.cuf  m_param.cuf main_prog_variables.cuf \
maillage.cuf cond_initial_cudaf.cuf  pre_post_traitement.cuf cflcond_cudaf.cuf \
cotes_cudaf_shared.cuf source_cudaf.cuf Full_FV_cudaf_shared.cuf general_cudaf.cuf

CUTEFILESPREF = $(addprefix ${SRC}/cuteflow/, ${CUTEFILES})

cuteflow : ${CUTEFILESPREF}
	module load pgi/19.4 cuda/10.0.130 openmpi/3.1.2;\
	${CUTECOMPILER} ${CUTEFLAGS} ${CUTEFILESPREF} -c ;\
	${CUTECOMPILER} ${CUTEFLAGS} ${CUTEFILESPREF} -o ${BIN}/$@_bin

conversion_maillage_JM : ${SRC}/conversion_maillage_JM/conversion_maillage_JM.f90
	module load gfortran
	gfortran -o ${BIN}/$@_bin conversion_maillage_JM.f90

conversion_maillage_PD : ${SRC}/conversion_maillage_PD/conversion_maillage_PD.cpp
	module load gcc
	g++ -std=c++11 -o ${BIN}/$@_bin conversion_maillage_PD.cpp

refine_mesh : ${SRC}/refine_mesh/refine_mesh.cpp
	module load gcc
	g++ -std=c++11 -o ${BIN}/$@_bin refine_mesh.cpp

merge_bluekenue : ${SRC}/merge_bluekenue/merge_bluekenue.cpp
	module load gcc
	g++ -std=c++11 -o ${BIN}/$@_bin merge_bluekenue.cpp

split_mesh : ${SRC}/split_mesh/split_mesh.cpp
	module load gcc metis
	g++ -std=c++11 -I /cvmfs/soft.computecanada.ca/easybuild/software/2017/avx2/Compiler/gcc5.4/metis/5.1.0/include -L libmetis.a -lmetis -o ${BIN}/$@_bin split_mesh.cpp

split_solution : ${SRC}/split_solution/split_solution.cpp
	module load gcc
	g++ -std=c++11 -o ${BIN}/$@_bin split_solution.cpp


clean:
	-rm -f *.o *.mod *~ *_bin
