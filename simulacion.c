// RUBÉN BALLESTEROS JIMÉNEZ, EDUARDO BONNÍN NARVÁEZ, VICENÇ SERVERA FERRER

//  En el main() asociar la señal SIGCHLD al enterrador.
//  Comprobar la sintaxis del comando.  // uso: ./simulacion <disco>
//  Montar el dispositivo virtual.  //padre
//  Crear el directorio de simulación: /simul_aaaammddhhmmss/
//  Para proceso:=1 hasta proceso<=NUMPROCESOS hacer
//      pid:=fork()
//      Si es el hijo entonces //pid = 0
//         Montar el dispositivo. //hijo
//             Crear el directorio del proceso hijo añadiendo el PID al nombre.
//             Crear el fichero prueba.dat dentro del directorio anterior.