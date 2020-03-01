Lo script Banda-Latenza.bash va lanciato senza fornire alcun parametro in input.
> ./Banda-Latenza.bash

Lo script RTT.bash dev’essere lanciato assieme a 4 parametri interi (corrispondenti ai file presenti nella cartella data): 
1. Il numero di byte del primo messaggio di piccole dimensioni (es. 16 byte)  
2. Il numero di byte del secondo messaggio di piccole dimensioni 
3. Il numero di byte del primo messaggio di grandi dimensioni (es. 1024 byte) 
4. Il numero di byte del secondo messaggio di grandi dimensioni 
Esso produrrà 4 file di output che saranno usati per generare i 4 grafici corrispondenti; ad ogni nuovo lancio i file precedenti saranno cancellati.
Esempio: > ./RTT.bash 32 48 512 32768 