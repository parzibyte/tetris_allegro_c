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

# Créditos de música

- Música de fondo: Super Mario Bros. Wonder 8 Bit VRC6 de Bulby https://www.youtube.com/@Bulby
- Cuando eliminas una línea: https://freesound.org/people/swordmaster767/sounds/397819/ Powerup? by swordmaster767 -- https://freesound.org/s/397819/ -- License: Creative Commons 0
- Cuando eliminas cuatro: Level Up / Mission Complete by Beetlemuse -- https://freesound.org/s/528958/ -- License: Creative Commons 0 https://freesound.org/people/Beetlemuse/sounds/528958/?