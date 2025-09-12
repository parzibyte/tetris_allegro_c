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