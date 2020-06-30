mkdir "1gpu";cd "1gpu"
cp "../$1" "0_$1"
cd ..

for i in 2 4 8 12 16 20 24 28 32
do
  mkdir "${i}gpu";cd "${i}gpu"
  cp ../../split_mesh .
  cp "../$1" .
  cd ..
done
