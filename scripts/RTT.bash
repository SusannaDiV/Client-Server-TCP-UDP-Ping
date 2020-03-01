#!/bin/bash

#Controllo del numero dei parametri
if [[ $# != 4 ]]; then
	printf "\nIncorrect parameter provided. Use RTT.bash UDP_min1 UDP_max1 UDP_min2 UDP_max2\n\n"
	exit 1
fi

#Controllo se i file in input nei parametri sono corretti
if [ ! -f ../data/udp_${1}.out ]; then
    printf "\nIncorrect parameter provided. Unable to find UDP_min1 source file\n\n"
    exit 1
fi

if [ ! -f ../data/udp_${2}.out ]; then
    printf "\nIncorrect parameter provided. Unable to find UDP_min2 source file\n\n"
    exit 1
fi

if [ ! -f ../data/udp_${3}.out ]; then
    printf "\nIncorrect parameter provided. Unable to find UDP_max1 source file\n\n"
    exit 1
fi

if [ ! -f ../data/udp_${4}.out ]; then
    printf "\nIncorrect parameter provided. Unable to find UDP_max2 source file\n\n"
    exit 1
fi

#Cancellazione vecchie versioni
rm ../data/RTT_*.png

#Dichiarazioni delle variabili dello script
RTT1=0;RTT2=0;RTT3=0;RTT4=0
EstRTT1=0;EstRTT2=0;EstRTT3=0;EstRTT4=0
VarRTT1=0;VarRTT2=0;VarRTT3=0;VarRTT4=0
DiffRTT1=0;DiffRTT2=0;DiffRTT3=0;DiffRTT4=0
UDP_min1=$1;UDP_max1=$2;UDP_min2=$3;UDP_max2=$4;

#Acquisire il numero di ripetizioni nell'iterazione corrente
line=$(awk '/Round/{print $9}' ../data/udp_${UDP_min1}.out | tail -n 1) 


for(( i=1; i<=$line; i++))
do
	#Acquisire tutte le linee che iniziano con "Round"
	RTT1=$(grep "Round" ../data/udp_${UDP_min1}.out | awk 'NR=='$i'{print $5}')
	RTT2=$(grep "Round" ../data/udp_${UDP_min2}.out | awk 'NR=='$i'{print $5}')
	RTT3=$(grep "Round" ../data/udp_${UDP_max1}.out | awk 'NR=='$i'{print $5}')
	RTT4=$(grep "Round" ../data/udp_${UDP_max2}.out | awk 'NR=='$i'{print $5}')
	
	#Nella prima iterazione il valore di Estimated RTT e' lo stesso di Sample RTT e il delay e' 0
	if [[ $i == 1 ]]; then
		EstRTT1=$RTT1
		EstRTT2=$RTT2
		EstRTT3=$RTT3
		EstRTT4=$RTT4

		#Scrivere i dati ottenuti sul file di output
		echo $(grep "Round" ../data/udp_${UDP_min1}.out | awk 'NR=='$i'{print $9,$5}') " " $EstRTT1 " " $VarRTT1 >> ../data/RTT_UDP_${UDP_min1}.out
		echo $(grep "Round" ../data/udp_${UDP_min2}.out | awk 'NR=='$i'{print $9,$5}') " " $EstRTT2 " " $VarRTT2 >> ../data/RTT_UDP_${UDP_min2}.out
		echo $(grep "Round" ../data/udp_${UDP_max1}.out | awk 'NR=='$i'{print $9,$5}') " " $EstRTT3 " " $VarRTT3 >> ../data/RTT_UDP_${UDP_max1}.out
		echo $(grep "Round" ../data/udp_${UDP_max2}.out | awk 'NR=='$i'{print $9,$5}') " " $EstRTT4 " " $VarRTT4 >> ../data/RTT_UDP_${UDP_max2}.out
	else 
		#Calcolare di Estimated RTT
		EstRTT1=$(bc -l <<< "(0.875*$EstRTT1)+(0.125*$RTT1)")
		EstRTT2=$(bc -l <<< "(0.875*$EstRTT2)+(0.125*$RTT2)")
		EstRTT3=$(bc -l <<< "(0.875*$EstRTT3)+(0.125*$RTT3)")
		EstRTT4=$(bc -l <<< "(0.875*$EstRTT4)+(0.125*$RTT4)")

		#Calcolare il delay tra Estimated RTT e Sample RTT
		DiffRTT1=$(bc -l <<< "($RTT1-$EstRTT1)")
		DiffRTT2=$(bc -l <<< "($RTT2-$EstRTT2)")
		DiffRTT3=$(bc -l <<< "($RTT3-$EstRTT3)")
		DiffRTT4=$(bc -l <<< "($RTT4-$EstRTT4)")
		
		#Valore assoluto di delay
		DiffRTT1=$(echo $DiffRTT1 | sed 's/-//')
		DiffRTT2=$(echo $DiffRTT2 | sed 's/-//')
		DiffRTT3=$(echo $DiffRTT3 | sed 's/-//')
		DiffRTT4=$(echo $DiffRTT4 | sed 's/-//')

		#Calcolare il valore finale di delay
		VarRTT1=$(bc -l <<< "(0.75*$VarRTT1)+(0.125*$DiffRTT1)")
		VarRTT2=$(bc -l <<< "(0.75*$VarRTT2)+(0.125*$DiffRTT2)")
		VarRTT3=$(bc -l <<< "(0.75*$VarRTT3)+(0.125*$DiffRTT3)")
		VarRTT4=$(bc -l <<< "(0.75*$VarRTT4)+(0.125*$DiffRTT4)")

		#Scrivere i dati sul file di output
		echo $(grep "Round" ../data/udp_${UDP_min1}.out | awk 'NR=='$i'{print $9,$5}') " " $EstRTT1 " " $VarRTT1 >> ../data/RTT_UDP_${UDP_min1}.out
		echo $(grep "Round" ../data/udp_${UDP_min2}.out | awk 'NR=='$i'{print $9,$5}') " " $EstRTT2 " " $VarRTT2 >> ../data/RTT_UDP_${UDP_min2}.out
		echo $(grep "Round" ../data/udp_${UDP_max1}.out | awk 'NR=='$i'{print $9,$5}') " " $EstRTT3 " " $VarRTT3 >> ../data/RTT_UDP_${UDP_max1}.out
		echo $(grep "Round" ../data/udp_${UDP_max2}.out | awk 'NR=='$i'{print $9,$5}') " " $EstRTT4 " " $VarRTT4 >> ../data/RTT_UDP_${UDP_max2}.out
	fi
done
 
#Creazione dei 4 grafici
gnuplot <<-eNDgNUPLOTcOMMAND
	set term png size 900, 700
	set output "../data/RTT_UDP_${UDP_min1}.png"
	set logscale y 10
	set xlabel "Repetition"
	set ylabel "RTT(ms)"
	plot "../data/RTT_UDP_${UDP_min1}.out" using 1:2 title "UDP sample" \
			with linespoints, \
		"../data/RTT_UDP_${UDP_min1}.out" using 1:3 title "UDP estimated" \
			with linespoints, \
		"../data/RTT_UDP_${UDP_min1}.out" using 1:4 title "UDP variability" \
			with linespoints
	clear

	set term png size 900, 700
	set output "../data/RTT_UDP_${UDP_min2}.png"
	set logscale y 10
	set xlabel "Repetition"
	set ylabel "RTT(ms)"
	plot "../data/RTT_UDP_${UDP_min2}.out" using 1:2 title "UDP sample" \
			with linespoints, \
		"../data/RTT_UDP_${UDP_min2}.out" using 1:3 title "UDP estimated" \
			with linespoints, \
		"../data/RTT_UDP_${UDP_min2}.out" using 1:4 title "UDP variability" \
			with linespoints
	clear
	
	set term png size 900, 700
	set output "../data/RTT_UDP_${UDP_max1}.png"
	set logscale y 10
	set xlabel "Repetition"
	set ylabel "RTT(ms)"
	plot "../data/RTT_UDP_${UDP_max1}.out" using 1:2 title "UDP sample" \
			with linespoints, \
		"../data/RTT_UDP_${UDP_max1}.out" using 1:3 title "UDP estimated" \
			with linespoints, \
		"../data/RTT_UDP_${UDP_max1}.out" using 1:4 title "UDP variability" \
			with linespoints
	clear
	set term png size 900, 700
	set output "../data/RTT_UDP_${UDP_max2}.png"
	set logscale y 10
	set xlabel "Repetition"
	set ylabel "RTT(ms)"
	plot "../data/RTT_UDP_${UDP_max2}.out" using 1:2 title "UDP sample" \
			with linespoints, \
		"../data/RTT_UDP_${UDP_max2}.out" using 1:3 title "UDP estimated" \
			with linespoints, \
		"../data/RTT_UDP_${UDP_max2}.out" using 1:4 title "UDP variability" \
			with linespoints
	clear
eNDgNUPLOTcOMMAND

