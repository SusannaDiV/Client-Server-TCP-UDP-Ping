#!/bin/bash

declare -a nomipossibili=("tcp" "udp")

#Cancellazione vecchie versioni
rm ../data/BandaLatenza*.png

for Protocol in "${nomipossibili[@]}"
do  
    #Controllo esistenza file in input
    if [ ! -f ../data/${Protocol}_throughput.dat ]; then
        printf "\nUnable to find ${Protocol}_throughput source file\n\n"
        exit 1
    fi

    #Acquisizione della prima e ultima riga (per il modello banda-latenza)
    head=$(head -n 1 ../data/${Protocol}_throughput.dat)
    tail=$(tail -n 1 ../data/${Protocol}_throughput.dat)

    #Estrapolazione variabili locali
    N1=$(echo $head| cut -d' ' -f 1)
    N2=$(echo $tail| cut -d' ' -f 1)
    TM1=$(echo $head| cut -d' ' -f 2)
    TM2=$(echo $tail| cut -d' ' -f 2)

    #Calcolo del delay
    DN1=$(bc -l <<< "$N1/$TM1")
    DN2=$(bc -l <<< "$N2/$TM2")

    #Calcolo di B e L 
    B=$(bc -l <<< "($N2-$N1)/($DN2-$DN1)")
    L=$(bc -l <<< "(($DN1*$N2)-($DN2*$N1))/($N2-$N1)")

#Creazione dei grafici TCP e UDP
gnuplot <<-eNDgNUPLOTcOMMAND
    set term png size 900, 700
	set output "../data/BandaLatenza-${Protocol}.png"
	set logscale x 2
	set logscale y 10
	set xlabel "msg size (B)"
	set ylabel "throughput (KB/s)"
	lbf(x)=x/($L+x/$B)
	plot "../data/${Protocol}_throughput.dat" using 1:2 title "${Protocol} Throughput" with linespoints, \
	lbf(x) title "Latency-Bandwidth model with L=$L and B=$B" with linespoints
	clear
eNDgNUPLOTcOMMAND

done