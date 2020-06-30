echo "1 $(tail -n 3 1gpu/RAPPORT_SIMULATION.txt | head -n 1 | cut -d ":" -f 2 | cut -d "S" -f 1)"
echo "1 $(tail -n 3 1gpu/RAPPORT_SIMULATION.txt | head -n 1 | cut -d ":" -f 2 | cut -d "S" -f 1)" > temps
for i in 2 4 8 12 16 20 24 28 32;do
  echo "$i $(tail -n 3 ${i}gpu/RAPPORT_SIMULATION.txt | head -n 1 | cut -d ":" -f 2 | cut -d "S" -f 1)"
  echo "$i $(tail -n 3 ${i}gpu/RAPPORT_SIMULATION.txt | head -n 1 | cut -d ":" -f 2 | cut -d "S" -f 1)" >> temps
done
