# comp-paralela-distribuida
Atividades da Disciplina Computação Paralela e Distribuída 

 cd C:\Users\Lenovo\Documents\atividadesparalela
 cl.exe atividade3.c /I"C:\Program Files (x86)\Microsoft SDKs\MPI\Include" /link /LIBPATH:"C:\Program Files (x86)\Microsoft SDKs\MPI\Lib\x64" msmpi.lib
 mpiexec -n 4 atividade3.exe

 cd C:\Users\Lenovo\Documents\atividadesparalela
 cl.exe atividade4.c /I"C:\Program Files (x86)\Microsoft SDKs\MPI\Include" /link /LIBPATH:"C:\Program Files (x86)\Microsoft SDKs\MPI\Lib\x64" msmpi.lib
 mpiexec -n 4 atividade4.exe estrelas1.pgm

 cd C:\Users\Lenovo\Documents\atividadesparalela\atividade5
javac *.java
java Servidor
varios: java Cliente