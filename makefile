CFLAGS = -Mcuda=cc60 -O0 
CC = mpif90 

OBJS = mpiDeviceUtil.cuf precision.cuf global_data.cuf global_data_constant_device.cuf data_transfer.cuf  m_param.cuf main_prog_variables.cuf \
mesh_construct.cuf  maillage.cuf cond_initial_cudaf.cuf  pre_post_traitement.cuf cflcond_cudaf.cuf POD_PID_ROM.cuf \
POD_SPG.cuf cotes_cudaf_shared.cuf source_cudaf.cuf Full_FV_cudaf_shared.cuf general_cudaf.cuf


runfile : ${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o $@ -I /cvmfs/restricted.computecanada.ca/easybuild/software/2017/Core/pgi/17.3/linux86-64/17.3/lib

clean:
	-rm -f *.o *.core *.mod *T3S outfile* core* *.vtk
