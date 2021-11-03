SHELL:=/bin/bash
SRC = ../src
BIN = ../bin

CC = 60
# CUTEFLAGS = -Mcuda=cc${CC} -O3 -I/cvmfs/soft.computecanada.ca/easybuild/software/2020/avx2/MPI/nvhpc20/cuda11.0/openmpi4/cgns/4.1.2/include/
CUTEFLAGS = -Mcuda=cc60 -O0 -g -Mfptrap=zero,overflow,underflow -Mbounds -Mdclchk -Mchkptr -Mchkstk -Meh_frame, -Minform,inform -I/cvmfs/soft.computecanada.ca/easybuild/software/2020/avx2/MPI/nvhpc20/cuda11.0/openmpi4/cgns/4.1.2/include/
CUTECOMPILER = mpif90 

CUTEFILES = precision_kind.cuf mpi_select_gpu.cuf file_id.cuf global_data.cuf global_data_device.cuf data_transfer.cuf maillage.cuf cfl_condition.cuf initial_condition.cuf pre_post_traitement.cuf source_terms.cuf flux_riemann.cuf finite_volumes_method.cuf main.cuf
# CUTEFILES = cfl_condition.cuf file_id.cuf flux_riemann.cuf global_data_device.cuf maillage.cuf mpi_select_gpu.cuf  pre_post_traitement.cuf data_transfer.cuf  finite_volumes_method.cuf global_data.cuf initial_condition.cuf main.cuf precision_kind.cuf source_terms.cuf


CUTEFILESPREF = $(addprefix ${SRC}/cuteflow/, ${CUTEFILES})

# BINFILES = cuteflow conversion_maillage_JM conversion_maillagePD refine_mesh merge_bluekenue split_mesh split_solution merge_solutions
BINFILES = cuteflow conversion_maillage_PD refine_mesh merge_bluekenue split_mesh split_solution merge_solutions
BINFILESPREF = $(addprefix ${BIN}/, ${BINFILES})

all: ${BINFILESPREF}

${BIN}/cuteflow : ${CUTEFILESPREF}
	module load StdEnv/2020 nvhpc/20.7 cuda/11.0 openmpi/4 cgns/4.1.2;\
	${CUTECOMPILER} ${CUTEFLAGS} ${CUTEFILESPREF} -o $@ -lcgns

# ${BIN}/conversion_maillage_JM : ${SRC}/conversion_maillage_JM/conversion_maillage_JM.f90
# 	module load gfortran;\
# 	gfortran -o $@ ${SRC}/conversion_maillage_JM/conversion_maillage_JM.f90

${BIN}/conversion_maillage_PD : ${SRC}/conversion_maillage_PD/conversion_maillage_PD.cpp
	module load gcc;\
	g++ -std=c++11 -o $@ ${SRC}/conversion_maillage_PD/conversion_maillage_PD.cpp

${BIN}/refine_mesh : ${SRC}/refine_mesh/refine_mesh.cpp
	module load gcc;\
	g++ -std=c++11 -o $@ ${SRC}/refine_mesh/refine_mesh.cpp

${BIN}/merge_bluekenue : ${SRC}/merge_bluekenue/merge_bluekenue.cpp
	module load gcc;\
	g++ -std=c++11 -o $@ ${SRC}/merge_bluekenue/merge_bluekenue.cpp

${BIN}/split_mesh : ${SRC}/split_mesh/split_mesh.cpp
	module load gcc metis;\
	g++ -std=c++11 -I /cvmfs/soft.computecanada.ca/easybuild/software/2017/avx2/Compiler/gcc5.4/metis/5.1.0/include -L libmetis.a -lmetis -o $@ ${SRC}/split_mesh/split_mesh.cpp

${BIN}/split_solution : ${SRC}/split_solution/split_solution.cpp
	module load gcc;\
	g++ -std=c++11 -o $@ ${SRC}/split_solution/split_solution.cpp

${BIN}/merge_solutions : ${SRC}/merge_solutions/merge_solutions.cpp
	module load gcc;\
	g++ -std=c++11 -o $@ ${SRC}/merge_solutions/merge_solutions.cpp

clean:
	-rm -f *.o *.mod *~
