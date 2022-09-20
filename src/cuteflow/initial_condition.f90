module initial_condition
  implicit none
contains
  subroutine set_initial_condition

    use precision_kind
    use global_data

    implicit none

    real(fp_kind) :: x1, x2 ,x3, x4, y1, y2, y3, y4, rayvol, vdlg0_dgi1, v1, v2, zmd
    real(fp_kind) :: alpha, beta, X, Y, r_sup, omega, dist_min, x1coupe, x2coupe, y1coupe, y2coupe, dO_P1, dO_P2
    integer :: ti,gi 

    do gi=1,nelt

      zmd = zm(gi)

      x1 = coo_table_elemwise(gi,1)
      y1 = coo_table_elemwise(gi,2)

      x2 = coo_table_elemwise(gi,4)
      y2 = coo_table_elemwise(gi,5)

      x3 = coo_table_elemwise(gi,7)
      y3 = coo_table_elemwise(gi,8)

      v1 = ( x1 + x2 + x3 ) / 3
      v2 = ( y1 + y2 + y3 ) / 3

      if (eqlin_barrage==1) then
        alpha = (y1eqbar - y2eqbar)/(x1eqbar - x2eqbar)
        beta = y1eqbar - alpha*x1eqbar

        if ( alpha*v1 - v2 + beta >= 0.00) then        
          vdlg0_dgi1    = h_amont - zmd
          vdlg0(1,gi) = vdlg0_dgi1

          if(vdlg0_dgi1 > tolisec) then
            vdlg0(2,gi) = vdlg0_dgi1 * u_amont
            vdlg0(3,gi) = vdlg0_dgi1 * v_amont   
          else
            vdlg0(1,gi) = tolisec
            vdlg0(2,gi) = 0.
            vdlg0(3,gi) = 0.
          end if

        else
          vdlg0_dgi1 = h_aval - zmd
          vdlg0(1,gi) = vdlg0_dgi1

          if(vdlg0_dgi1 > tolisec) then
            vdlg0(2,gi) = vdlg0_dgi1 * u_aval
            vdlg0(3,gi) = vdlg0_dgi1 * v_aval
          else
            vdlg0(1,gi) = tolisec
            vdlg0(2,gi) = 0.
            vdlg0(3,gi) = 0.
          end if
        endif
      endif

      if (plan==1) then
        vdlg0_dgi1 = (-xplan*(xpoint-v1)-yplan*(ypoint-v2))/zplan + zpoint - zmd
        vdlg0(1,gi) = vdlg0_dgi1

        if(vdlg0_dgi1 > tolisec) then
          vdlg0(2,gi) = vdlg0_dgi1 * u_aval
          vdlg0(3,gi) = vdlg0_dgi1 * v_aval
        else
          vdlg0(1,gi) = tolisec
          vdlg0(2,gi) = 0.
          vdlg0(3,gi) = 0.
        end if
      endif
    end do
  end subroutine set_initial_condition

  subroutine read_initial_condition
    use precision_kind
    use global_data
    use file_id

    implicit none

    integer :: iel

    open(390,file=fich_sol_init,status='old')
    read (390,*) tc_init

    do iel=1,nelt
      read (390,*) vdlg0(1,iel), vdlg0(2,iel), vdlg0(3,iel)
    enddo

    close (390) 
  end subroutine read_initial_condition
end module initial_condition
