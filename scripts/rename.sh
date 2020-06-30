for i in `seq 0 $1`; do
  cp ${i}_solution_elements_restart.txt ${i}_$2.txt
  cp ${i}_solution_noeuds.txt ${i}_$2_noeuds.txt
done
