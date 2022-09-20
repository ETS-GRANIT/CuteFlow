module memory_exchange_functions
  implicit none
contains
  subroutine gpu_echange_fant_bloc_async_not_cuda_aware(a, nd)
    use precision_kind
    use global_data
    use mpi

    integer, intent(in) :: nd
    real(fp_kind), intent(inout) :: a(nd,nelt)
    integer                     :: stat(MPI_STATUS_SIZE)
    integer :: j
    integer :: jloc
    integer :: id
    integer :: p, k
    integer :: nsbloc, nreste

    k=1
    do j=1,nelt_fant_envoi_bloc
      nsbloc = ndln*elt_fant_envoi_bloc(j,2)/768 + 1
      nreste = ndln*elt_fant_envoi_bloc(j,2)-(nsbloc-1)*768
      do p=0,nsbloc-2
        call MPI_ISEND(a(1,elt_fant_envoi_bloc(j,1)+p*768/nd),768,fp_kind_mpi,elt_fant_envoi_bloc(j,3),p,MPI_COMM_WORLD,reqsend(k),mpi_ierr)
        k=k+1
      end do
      call MPI_ISEND(a(1,elt_fant_envoi_bloc(j,1)+(nsbloc-1)*768/nd),nreste,fp_kind_mpi,elt_fant_envoi_bloc(j,3),nsbloc-1,MPI_COMM_WORLD,reqsend(k),mpi_ierr)
      k=k+1
    end do

    k=1
    do j=1,nelt_fant_recep_bloc
      nsbloc = ndln*elt_fant_recep_bloc(j,2)/768 + 1
      nreste = ndln*elt_fant_recep_bloc(j,2)-(nsbloc-1)*768
      do p=0,nsbloc-2
        call MPI_IRECV(a(1,elt_fant_recep_bloc(j,1)+p*768/nd),768,fp_kind_mpi,elt_fant_recep_bloc(j,3),p,MPI_COMM_WORLD,reqrecv(k),mpi_ierr)
        k=k+1
      end do
      call MPI_IRECV(a(1,elt_fant_recep_bloc(j,1)+(nsbloc-1)*768/nd),nreste,fp_kind_mpi,elt_fant_recep_bloc(j,3),nsbloc-1,MPI_COMM_WORLD,reqrecv(k),mpi_ierr)
      k=k+1
    end do

  end subroutine gpu_echange_fant_bloc_async_not_cuda_aware

  subroutine gpu_echange_fant_bloc_async(a, nd)
    use precision_kind
    use global_data
    use mpi

    integer, intent(in) :: nd
    real(fp_kind), intent(inout) :: a(nd,nelt)
    integer                     :: stat(MPI_STATUS_SIZE)
    integer :: j
    integer :: jloc
    integer :: id
    integer :: p, k
    integer :: nsbloc, nreste

    k=1
    do j=1,nelt_fant_envoi_bloc
      nsbloc = ndln*elt_fant_envoi_bloc(j,2)/768 + 1
      nreste = ndln*elt_fant_envoi_bloc(j,2)-(nsbloc-1)*768
      do p=0,nsbloc-2
        call MPI_ISEND(a(1,elt_fant_envoi_bloc(j,1)+p*768/nd),768,fp_kind_mpi,elt_fant_envoi_bloc(j,3),p,MPI_COMM_WORLD,reqsend(k),mpi_ierr)
        k=k+1
      end do
      call MPI_ISEND(a(1,elt_fant_envoi_bloc(j,1)+(nsbloc-1)*768/nd),nreste,fp_kind_mpi,elt_fant_envoi_bloc(j,3),nsbloc-1,MPI_COMM_WORLD,reqsend(k),mpi_ierr)
      k=k+1
    end do

    k=1
    do j=1,nelt_fant_recep_bloc
      nsbloc = ndln*elt_fant_recep_bloc(j,2)/768 + 1
      nreste = ndln*elt_fant_recep_bloc(j,2)-(nsbloc-1)*768
      do p=0,nsbloc-2
        call MPI_IRECV(a(1,elt_fant_recep_bloc(j,1)+p*768/nd),768,fp_kind_mpi,elt_fant_recep_bloc(j,3),p,MPI_COMM_WORLD,reqrecv(k),mpi_ierr)
        k=k+1
      end do
      call MPI_IRECV(a(1,elt_fant_recep_bloc(j,1)+(nsbloc-1)*768/nd),nreste,fp_kind_mpi,elt_fant_recep_bloc(j,3),nsbloc-1,MPI_COMM_WORLD,reqrecv(k),mpi_ierr)
      k=k+1
    end do

  end subroutine gpu_echange_fant_bloc_async

  subroutine gpu_echange_fant_bloc(a, nd)
    use precision_kind
    use global_data
    use mpi

    integer, intent(in) :: nd
    real(fp_kind), intent(inout) :: a(nd,nelt)
    integer                     :: stat(MPI_STATUS_SIZE)
    integer :: j
    integer :: jloc
    integer :: id
    integer :: p
    integer :: nsbloc, nreste

    call MPI_BARRIER(MPI_COMM_WORLD,mpi_ierr)
    do id=0,num_mpi_process-1
      if(mpi_process_id==id) then
        do j=1,nelt_fant_envoi_bloc
          nsbloc = nd*elt_fant_envoi_bloc(j,2)/768 + 1
          nreste = nd*elt_fant_envoi_bloc(j,2)-(nsbloc-1)*768
          do p=0,nsbloc-2
            call MPI_SEND(a(1,elt_fant_envoi_bloc(j,1)+p*768/nd),768,fp_kind_mpi,elt_fant_envoi_bloc(j,3),p,MPI_COMM_WORLD,mpi_ierr)
          end do
          call MPI_SEND(a(1,elt_fant_envoi_bloc(j,1)+(nsbloc-1)*768/nd),nreste,fp_kind_mpi,elt_fant_envoi_bloc(j,3),nsbloc-1,MPI_COMM_WORLD,mpi_ierr)
        end do
      else
        do j=1,nelt_fant_recep_bloc
          if(elt_fant_recep_bloc(j,3)==id) then
            nsbloc = nd*elt_fant_recep_bloc(j,2)/768 + 1
            nreste = nd*elt_fant_recep_bloc(j,2)-(nsbloc-1)*768
            do p=0,nsbloc-2
              call MPI_RECV(a(1,elt_fant_recep_bloc(j,1)+p*768/nd),768,fp_kind_mpi,elt_fant_recep_bloc(j,3),p,MPI_COMM_WORLD,stat,mpi_ierr)
            end do
            call MPI_RECV(a(1,elt_fant_recep_bloc(j,1)+(nsbloc-1)*768/nd),nreste,fp_kind_mpi,elt_fant_recep_bloc(j,3),nsbloc-1,MPI_COMM_WORLD,stat,mpi_ierr)
          end if
        end do
      end if
      call MPI_BARRIER(MPI_COMM_WORLD,mpi_ierr)
    end do

  end subroutine gpu_echange_fant_bloc

  subroutine update_send_cells
    use precision_kind
    use global_data

    implicit none

    integer :: gi, gj

    do gj=1,nelt_fant_envoi_bloc
      do gi=1,elt_fant_envoi_length(gj)
        elt_fant_envoi_sol(:,gi,gj) = vdlg(:,elt_fant_envoi_id(gi,gj))
      end do
    end do

  end subroutine update_send_cells

  subroutine gpu_echange_cgns(a,nd)
    use mpi
    use precision_kind 
    use global_data

    implicit none
    integer, intent(in) :: nd
    real(fp_kind), intent(inout) :: a(nd,nelt)
    integer :: i, j, tag
    integer :: nsbloc, nsreste, k

    k=1
    do i=1,nelt_fant_envoi_bloc
      tag = mpi_process_id
      call MPI_ISend(elt_fant_envoi_sol(1,1,i), nd*elt_fant_envoi_list(i)%length, fp_kind_mpi, &
        &elt_fant_envoi_list(i)%proc, tag, MPI_COMM_WORLD, reqsend(i), mpi_ierr)
    end do

    do i=1,nelt_fant_recep_bloc
      call MPI_IRecv(a(1,elt_fant_recep_range(i)%start), nd*elt_fant_recep_range(i)%length, fp_kind_mpi, &
        &elt_fant_recep_range(i)%proc, elt_fant_recep_range(i)%proc, MPI_COMM_WORLD, reqrecv(i), mpi_ierr)
    end do

  end subroutine gpu_echange_cgns
end module memory_exchange_functions
