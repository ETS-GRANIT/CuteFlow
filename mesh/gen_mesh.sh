BASE_DIR=`pwd`
WORKING_DIR=`echo $1 | cut -d "_" -f4`
mkdir $WORKING_DIR;cd $WORKING_DIR

mkdir "1gpu";cd "1gpu"
cp "../../$1" "0_$1"
cd ..

for i in 2 4 8 16 32
do
  mkdir "${i}gpu";cd "${i}gpu"
  cp ../../main .
  cp ../../job.sh .
  cp "../../$1" .
    
  echo "#!/bin/bash
#SBATCH --ntasks=1
#SBATCH --mem=16000M
#SBATCH --time=0-0$2:00
#SBATCH --account=def-soulaima
#SBATCH --output=outfile.out

./main $1 $i
" > job.sh
  sbatch job.sh
  cd ..
done
