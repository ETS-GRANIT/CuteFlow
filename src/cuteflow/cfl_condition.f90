module cfl_condition
  implicit none
contains
  subroutine cflcond(solut)
    ! ======================================================================
    !     cfl_dcond = calculation of the time step following the cfl_d
    !     auteur : Youssef Loukili  GRANIT ETSMTL
    !     version : 1.0  ;  May 12 2003
    ! ======================================================================

    use precision_kind
    use global_data

    implicit none

    real(fp_kind), dimension(ndln,nelt), intent(in)     :: solut

    real(fp_kind)               :: hauteur, vitessu, vitessv, normvites, vitessmax, dt_tmp
    integer                     :: gi

    dt = 1000000.0D0
    do gi=1,nelt-nelt_fant_recep
      hauteur = solut(1,gi)
      if ( hauteur > tolisec ) then
        vitessu = solut(2,gi) / hauteur
        vitessv = solut(3,gi) / hauteur
        normvites = sqrt(vitessu**2 + vitessv**2)
        vitessmax = max( 0.01D0, (normvites + sqrt(gp * abs(hauteur))))
        dt_tmp = min( cfl * cotemin_arr(gi)/vitessmax, 1000000.0D0)
      else
        dt_tmp = 1000000.0D0
      endif
      if(dt_tmp<dt) dt = dt_tmp
    end do

    if(is_dt_constant==1) then
      dt = constant_dt
    end if
  end subroutine cflcond

  subroutine compute_time_step
    use global_data
    use precision_kind
    implicit none

    integer :: ierr

    ! Calcul du pas de temps via la CFL
    call cflcond(vdlg)

    !!Take the smallest dt across all GPUs
    if(num_mpi_process>1) then
      call MPI_ALLREDUCE(MPI_IN_PLACE,dt,1,fp_kind_mpi,MPI_MIN,MPI_COMM_WORLD, mpi_ierr)
    end if

    if(dt>9999 .or. dt < 1e-12) then
      print*, "Erreur sur le pas de temps dt=", dt
      tc = tc + 1000000000000.
    end if
  end subroutine compute_time_step

end module cfl_condition
