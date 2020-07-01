for i in 1 2 4 8 12 16 20 24 28 32;do
  mkdir ${i}gpu
  cd ${i}gpu
  #cp ../../split_mesh/cas1/${i}gpu/*_cas* .
  # cp ../../11M_all_opt3/${i}gpu/${i}_gpu.sh .
  cp ../donnees.f .
  rm *RAPPORT*
  # cp ../../build/cuteflow .
  cp ../cuteflow .
  sbatch ${i}_gpu.sh
  cd ..
done
