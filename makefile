SHELL:=/bin/bash
SRC = ../src
BIN = ../bin

CC = 60
# CUTEFLAGS = -O3 -I/cvmfs/soft.computecanada.ca/easybuild/software/2020/avx2/MPI/nvhpc20/cuda11.0/openmpi4/cgns/4.1.2/include/
CUTEFLAGS = -O3 -ffree-line-length-512 -I/cvmfs/soft.computecanada.ca/easybuild/software/2020/avx2/MPI/gcc9/openmpi4/cgns/4.1.2/include/
# CUTEFLAGS = -Mcuda=cc60 -O0 -g -Mfptrap=zero,overflow,underflow -Mwall -Mbounds -Mdclchk -Mchkptr -Mchkstk -Meh_frame, -Minform,inform -I/cvmfs/soft.computecanada.ca/easybuild/software/2020/avx2/MPI/nvhpc20/cuda11.0/openmpi4/cgns/4.1.2/include/
CUTECOMPILER = mpif90 

# CUTEFILES = precision_kind.cuf mpi_select_gpu.cuf file_id.cuf global_data.cuf global_data_device.cuf data_transfer.cuf maillage.cuf cfl_condition.cuf initial_condition.cuf pre_post_traitement.cuf source_terms.cuf flux_riemann.cuf memory_exchange_functions.cuf finite_volumes_method.cuf main.cuf
CUTEFILES = global_data.f90 file_id.f90 initial_condition.f90 source_terms.f90 memory_exchange_functions.f90 flux_riemann.f90 precision_kind.f90 cfl_condition.f90 pre_post_traitement.f90 maillage.f90 finite_volumes_method.f90 main.f90

CUTEFILESPREF = $(addprefix ${SRC}/cuteflow/, ${CUTEFILES})

BINFILES = cuteflow conversion_maillage_PD refine_mesh merge_bluekenue split_mesh split_solution merge_solutions cute_to_cgns
BINFILESPREF = $(addprefix ${BIN}/, ${BINFILES})

all: ${BINFILESPREF}

${BIN}/cuteflow : ${CUTEFILESPREF}
	module load StdEnv/2020 gcc/9.3.0 openmpi/4.0.3 cgns/4.1.2;\
	${CUTECOMPILER} ${CUTEFLAGS} ${CUTEFILESPREF} -o $@ -lcgns

# ${BIN}/cute_to_cgns : ${SRC}/cute_to_cgns/cute_to_cgns.cpp
# 	module load gcc cgns/4.1.2;\
# 	g++ -std=c++11 -O3 -o $@ ${SRC}/cute_to_cgns/cute_to_cgns.cpp -lcgns

# ${BIN}/refine_mesh : ${SRC}/refine_mesh/refine_mesh.cpp
# 	module load gcc;\
# 	g++ -std=c++11 -O3 -o $@ ${SRC}/refine_mesh/refine_mesh.cpp

clean:
	-rm -f *.o *.mod *~
