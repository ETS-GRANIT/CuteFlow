CFLAGS = -Mcuda=cc60 -O3 
#CFLAGS = -Mcuda=cc60 -O0 -g -Mfptrap=zero,overflow,underflow -Mbounds -Mdclchk -Mchkptr -Mchkstk -Meh_frame
CC = mpif90 

OBJS = mpiDeviceUtil.cuf precision.cuf global_data.cuf global_data_constant_device.cuf data_transfer.cuf  m_param.cuf main_prog_variables.cuf \
mesh_construct.cuf  maillage.cuf cond_initial_cudaf.cuf  pre_post_traitement.cuf cflcond_cudaf.cuf \
cotes_cudaf_shared.cuf source_cudaf.cuf Full_FV_cudaf_shared.cuf general_cudaf.cuf


runfile : ${OBJS}
	${CC} ${CFLAGS} ${OBJS} -c
	${CC} ${CFLAGS} ${OBJS} -o $@

clean:
	-rm -f *.o *.core *.mod *T3S outfile* core* *.vtk *~ *boundary* *SOLUTION* *SIMULATION.txt
