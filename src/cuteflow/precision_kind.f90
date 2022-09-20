module precision_kind
  use mpi
  integer , parameter :: singlePrecision = kind(0.0)
  integer , parameter :: doublePrecision = kind(0.0d0)

  ! Comment out one of the lines below
  ! integer , parameter :: fp_kind = singlePrecision
  ! integer , parameter :: fp_kind_mpi = MPI_REAL
  integer , parameter :: fp_kind = doublePrecision
  integer , parameter :: fp_kind_mpi = MPI_DOUBLE_PRECISION
end module precision_kind
