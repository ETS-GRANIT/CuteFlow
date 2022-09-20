module source_terms
  implicit none
contains
  subroutine compute_source_terms

    use precision_kind
    use global_data
    implicit none

    real(fp_kind)                         :: chezy2, vitu, vitv, normvit, area
    real(fp_kind)                         :: x1, y1, x2, y2, x3, y3, ln, orient
    real(fp_kind)                         :: manning, dtgn2, dtgn2h, detaf, q1, q2, q3, normQ, beta, zij, hijet
    real(fp_kind), dimension(3,3)         :: af
    real(fp_kind), dimension(2)           :: norm
    integer                               :: nc1, nc2, nc3, kvol, kvoisin
    integer :: cond
    integer :: gi, nelt_limit

    do gi=1,nelt
      sourcfric(:,gi) = 0.d0
      q1 = vdlg0(1,gi)
      q2 = vdlg0(2,gi)
      q3 = vdlg0(3,gi)
      area = surf(gi)
      normQ   = sqrt( q2**2 + q3**2 )

      if(is_override_manning == 1) then
        manning = override_manning
      else
        manning = manning_nelt(gi)
      end if

      !       ++++++++++++++  SLOPE  +++++++++++++++
      ! Eq 30 de A Weighted Average Flux (WAF) scheme applied to shallow waterequations for real-life applications (Riadh et al.)

      sourcbath(1,gi) = 0.
      sourcbath(2,gi) = 0.
      sourcbath(3,gi) = 0.

      do kvol=1,ns

        kvoisin = boundary(gi,kvol,1)

        ! Uniquement si pas limite
        if(kvoisin > 0) then

          ! Recuperation des indices des sommets et orientation de la normale comme dans cotes_shared.cuf
          nc1 = kvol

          cond = merge(1,0, nc1 == ns)
          nc2 = int((nc1 - ns + 1)*cond + (nc1 + 1)*(1-cond))

          cond = merge(1,0, nc2 == ns)
          nc3 = int((nc2 - ns + 1)*cond + (nc2 + 1)*(1-cond))

          x1 = coo_table_elemwise(gi, 3*nc1-2)
          y1 = coo_table_elemwise(gi, 3*nc1-1)

          x2 = coo_table_elemwise(gi, 3*nc2-2)
          y2 = coo_table_elemwise(gi, 3*nc2-1) 

          x3 = coo_table_elemwise(gi, 3*nc3-2)
          y3 = coo_table_elemwise(gi, 3*nc3-1) 

          ln = sqrt( (x2 - x1)**2 + (y2 - y1)**2 )

          norm(1) = ( y2 - y1 ) / ln
          norm(2) = ( x1 - x2 ) / ln

          orient = norm(1)*((x1+x2)/2.-(x1+x2+x3)/3.) + norm(2)*((y1+y2)/2.-(y1+y2+y3)/3.)
          if(orient < 0.) then
            norm(1) = ( y1 - y2 ) / ln
            norm(2) = ( x2 - x1 ) / ln
          end if

          zij = max(zm(gi),zm(kvoisin))

          hijet = q1 + zm(gi) - zij

          hijet = max(0.0D0,hijet)

          sourcbath(2,gi) = sourcbath(2,gi) + gp/2.0D0 * (hijet**2 - q1**2) * ln * norm(1)
          sourcbath(3,gi) = sourcbath(3,gi) + gp/2.0D0 * (hijet**2 - q1**2) * ln * norm(2)
        end if

      end do

      !       ++++++++++++++  FRICTION  +++++++++++++++
      if (friction == 1) then

        if (  q1 <= tolisec) then
          !		if (  q1 <= tol ) then              

          sourcfric( 2,gi) = 0.d0
          sourcfric( 3,gi) = 0.d0
        else                                                
          vitu = q2 / q1
          vitv = q3 / q1

          chezy2  = q1**(1.d0/3.d0) / manning**2
          normvit = sqrt( vitu**2 + vitv**2 )

          if ( normvit < tol ) then
            normvit= tol
          endif

          sourcfric( 2,gi) = - gp * manning*manning * vitu * normvit * area*q1**(-1.d0/3.d0)
          sourcfric( 3,gi) = - gp * manning*manning * vitv * normvit * area*q1**(-1.d0/3.d0)
        endif

        !       ++++++++++++++  IMPLICITATION  +++++++++++++++

        if ( fricimplic == 1 ) then 

          if ( normvit > tol .and. q1 > tolisec ) then
            !			if ( normvit > tol .and. q1 > tol ) then
            !
            ! tempa  = real(q1**(-4.d0/3.d0))
            ! dtgn2h = dt / 2.d0 * gp * manning*manning*tempa
            !

            dtgn2h = dt / 2.d0 * gp * manning*manning*q1**(-4.d0/3.d0)

            !! Matrice Id-dt/2 * J avec J la matrice jacobienne du terme de friction
            af(1,1) = 1.d0
            af(1,2) = 0.d0
            af(1,3) = 0.d0
            af(2,1) = - 7.d0/3.d0*dtgn2h * vitu * normvit
            ! af(2,2) = 1.d0 + (dtgn2h/q1) * ( 2*vitu**2 + vitv**2 ) / normvit
            af(2,2) = 1.d0 + dtgn2h * ( 2*vitu**2 + vitv**2 ) / normvit
            ! af(2,3) = (dtgn2h/q1) * vitu * vitv / normvit
            af(2,3) = dtgn2h * vitu * vitv / normvit
            af(3,1) = - 7.d0/3.d0*dtgn2h * vitv * normvit
            af(3,2) = af(2,3)
            ! af(3,3) = 1.d0 + (dtgn2h/q1) * ( vitu**2 + 2*vitv**2 ) / normvit
            af(3,3) = 1.d0 + dtgn2h * ( vitu**2 + 2*vitv**2 ) / normvit

            !! Determinant de Id-dt/2 * J
            detaf = af(2,2)*af(3,3)-af(2,3)*af(3,2)

            !! Matrice inverse de Id-dt/2 * J
            afm1(gi,1) = 1
            afm1(gi,2) = 0.d0
            afm1(gi,3) = 0.d0
            afm1(gi,4) = -(af(2,1)*af(3,3) - af(2,3)*af(3,1))/detaf 
            afm1(gi,5) = af(3,3)/detaf
            afm1(gi,6) = -af(2,3)/detaf 
            afm1(gi,7) =  (af(2,1)*af(3,2) - af(2,2)*af(3,1))/detaf
            afm1(gi,8) = -af(3,2)/detaf
            afm1(gi,9) = af(2,2)/detaf

          else
            afm1(gi,1) = 1.d0
            afm1(gi,2) = 0.d0
            afm1(gi,3) = 0.d0
            afm1(gi,4) = 0.d0
            afm1(gi,5) = 1.d0
            afm1(gi,6) = 0.d0
            afm1(gi,7) = 0.d0
            afm1(gi,8) = 0.d0
            afm1(gi,9) = 1.d0
          endif

        elseif ( fricimplic == 2 ) then

          if ( normvit > tol .and. q1 > tolisec ) then
            !	if ( normvit > tol .and. q1 > tol ) then
            !
            ! tempa  = real(q1**(-4.d0/3.d0))
            ! beta = - dt* gp * manning*manning * normvit * tempa

            beta = - dt* gp * manning*manning * normvit * q1**(-4.d0/3.d0)

            !! Matrice Id-dt * B
            af(1,1) = 1.d0
            af(1,2) = 0.d0
            af(1,3) = 0.d0
            af(2,1) = 0.d0
            af(2,2) = 1.d0 - beta
            af(2,3) = 0.d0
            af(3,1) = 0.d0
            af(3,2) = 0.d0
            af(3,3) = 1.d0 - beta

            !! Determinant de  Id-dt * B
            detaf = (1-beta)**2

            !! Matrice inverse Id-dt * B
            afm1(gi,1) = 1.d0
            afm1(gi,2) = 0.d0
            afm1(gi,3) = 0.d0
            afm1(gi,4) = 0.d0
            afm1(gi,5) = (1-beta)/detaf
            afm1(gi,6) = 0.d0
            afm1(gi,7) = 0.d0
            afm1(gi,8) = 0.d0
            afm1(gi,9) = (1-beta)/detaf

          else

            afm1(gi,1) = 1.d0
            afm1(gi,2) = 0.d0
            afm1(gi,3) = 0.d0
            afm1(gi,4) = 0.d0
            afm1(gi,5) = 1.d0
            afm1(gi,6) = 0.d0
            afm1(gi,7) = 0.d0
            afm1(gi,8) = 0.d0
            afm1(gi,9) = 1.d0

          endif
        endif
      endif
    end do
  end subroutine compute_source_terms
end module source_terms
