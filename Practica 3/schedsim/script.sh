#!/bin/bash

DISPSCHE=`./schedsim -L | grep -v Avail`

echo ""
echo "Script test schedsim"
  
echo "Cleaning workspace"
if [ -d "results/" ]; then 
rm -rf results/
fi

rm CPU*


echo "Loading files"
if [ -s "schedsim" ] && [ -x "schedsim" ]; then
  echo "schedsim files found"
else 
  echo "schedsim files not found"
  exit 1
fi


echo -n "file to launch (name): "
read Nomfile

file="./examples/$Nomfile"

while [ ! -f "$file" ] 
do
  echo "ERROR read file"
  echo -n "file to launch (name):"
  read file
done


echo -n "Max CPUs?: "
read cpu

#while maxcpu > 8
while [ $cpu -gt 8 ] 
do
  echo "ERROR CPUs number exceed"
  echo -n "Max CPUs?: "
  read maxcpu
done

echo "file = $file"
echo "cpu = $cpu"

mkdir results

echo "Loading schedsim..."
for nameSched in $DISPSCHE 
do
  
    #mostrar la salida de los comandos??*********************
   ./schedsim -n $cpu -i $file -s $nameSched

    for ((i=0;i<cpu;i++))
    do
      mv "CPU_$i.log" "results/$nameSched-$cpu-$i.log"
      # ver donde se ponen estos archivos????****************+
      cd ../gantt-gplot/
      ./generate_gantt_chart ../schedsim/results/$nameSched-$cpu-$i.log
      cd ../schedsim/
    done



done

echo "Terminated"
exit 1;


