# tetris_allegro_c
 Juego de Tetris en ANSI C (obviamente compatible con C++) usando Allegro (Trabajo en pogreso)

# Compilando

Ejecuta `make` o revisa el makefile. Necesitas **arial.ttf** en la ubicación
del proyecto, si no:

```
gcc -Wall main.c -o main -lallegro -lallegro_font -lallegro_primitives -lallegro_ttf
./main
make: *** [makefile:5: main] Segmentation fault
make: *** Se borra el archivo 'main'
```

# Bugs conocidos que anoto antes de que se me olviden
- A veces el fantasma se muestra encima de la verdadera pieza cuando uno la intenta meter en un hueco
- Siguiendo el punto de arriba, si bajaste tu pieza y la vas a meter a la izquierda o derecha y presionas Arriba (que baja la pieza al instante) se va a poner donde está el fantasma, es decir, arriba, cosa errónea