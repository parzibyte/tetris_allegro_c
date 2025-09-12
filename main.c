#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <stdbool.h>
#include <stdio.h>
#include <allegro5/allegro_ttf.h>
#include <stdbool.h>
#define ALTURA 800
#define ANCHURA 1000
#define MEDIDA_CUADRO 28
#define CANTIDAD_BYTES_TETRIMINO 2
#define ANCHO_CUADRICULA 4
#define ALTO_CUADRICULA 4
#define BITS_EN_UN_BYTE 8
#define MAXIMO_INDICE_BIT_EN_BYTE 7
#define CUADRICULA_TETRIMINO 4

struct Tetrimino
{
    // medio byte por línea
    uint8_t cuadricula[CANTIDAD_BYTES_TETRIMINO];
    int x, y;
};

/*
Recibe un apuntador al tetrimino, la cuadrícula del tetris y dos modificadores x e y.
La función aumentará las coordenadas del tetrimino a partir de los modificadores simulando un avance y
después va a devolver true si el tetrimino colisiona con una pared, suelo u otras piezas
*/
bool tetriminoColisionaConCuadriculaAlAvanzar(struct Tetrimino *tetrimino, uint8_t cuadricula[ALTO_CUADRICULA][ANCHO_CUADRICULA], int8_t modificadorX, int8_t modificadorY)
{

    // Recorremos los 2 bytes del tetrimino
    for (int indiceChar = 0; indiceChar < CANTIDAD_BYTES_TETRIMINO; indiceChar++)
    {
        // Dentro recorremos cada bit de cada byte del tetrimino
        for (int indiceBitTetrimino = 0; indiceBitTetrimino < BITS_EN_UN_BYTE; indiceBitTetrimino++)
        {
            // Comprobamos si hay un cuadro del tetrimino, ya que la cuadrícula de 4x4 no siempre está llena en su totalidad
            bool hayUnCuadroDeTetriminoEnLaCoordenadaActual = (tetrimino->cuadricula[indiceChar] >> (MAXIMO_INDICE_BIT_EN_BYTE - indiceBitTetrimino)) & 1;
            if (hayUnCuadroDeTetriminoEnLaCoordenadaActual)
            {
                // Coordenadas sobre la cuadrícula después de aplicar los modificadores
                int xEnCuadriculaDespuesDeModificar = tetrimino->x + (indiceBitTetrimino % 4) + modificadorX;
                int yEnCuadriculaDespuesDeModificar = tetrimino->y + (indiceChar * CANTIDAD_BYTES_TETRIMINO) + (indiceBitTetrimino / 4) + modificadorY;
                // Límites con anchos y altos de la cuadrícula
                if (xEnCuadriculaDespuesDeModificar > ANCHO_CUADRICULA * BITS_EN_UN_BYTE - 1)
                {
                    return true;
                }
                if (xEnCuadriculaDespuesDeModificar < 0)
                {
                    return true;
                }

                if (yEnCuadriculaDespuesDeModificar < 0)
                {
                    return true;
                }

                if (yEnCuadriculaDespuesDeModificar > ALTO_CUADRICULA - 1)
                {
                    return true;
                }

                /*
                Hasta este punto ya sé que sí hay un cuadro del tetrimino en la posición actual.
                También sabemos que las coordenadas son seguras pues no se salen de ningún límite.

                Falta ver si hay un cuadro lleno en la cuadrícula (matriz). Recordemos que usamos un bit
                por cada cuadro así que primero necesitamos la posición Y, eso lo tenemos
                en "yEnCuadriculaDespuesDeModificar" así que para acceder a esa línea hacemos un:
                cuadricula[yEnCuadriculaDespuesDeModificar]

                Eso nos dará la posición del arreglo lineal. Ahora necesitamos saber en cuál de esos valores se
                encuentra el byte que nos interesa. Para ello dividimos la coordenadaX entre 8 y se redondea
                automáticamente hacia abajo.

                Si quisiéramos acceder al byte completo hasta ahora haríamos un:
                cuadricula[yEnCuadriculaDespuesDeModificar][xEnByteDeCuadricula]

                Ya tenemos el byte que nos interesa pero nos falta el bit dentro de ese byte.
                Para ello hacemos un
                xEnCuadriculaDespuesDeModificar % 8

                Y ya nos dará el índice del bit. Ya tenemos índice de arreglo dentro de la matriz,
                índice del elemento dentro del arreglo y el índice bit pero no podemos hacer lo siguiente:
                cuadricula[yEnCuadriculaDespuesDeModificar][xEnByteDeCuadricula][indiceBitDeByteEnCuadricula]

                Así que tenemos que hacer un desplazamiento de bits.
                Al bit que nos interesa y que está en el byte dentro de
                cuadricula[yEnCuadriculaDespuesDeModificar][xEnByteDeCuadricula] lo empujamos hasta
                que esté en el LSB. Luego le hacemos un & con el 1 que también tiene un 1 en su LSB y si el resultado es
                1 entonces significa que en esa posición sí hay un cuadro de la cuadrícula

                Por ejemplo, supongamos que en cuadricula[yEnCuadriculaDespuesDeModificar][xEnByteDeCuadricula] hay un
                byte con el valor 200.
                Su representación es:
                11001000
                     ^-----------------------------------------------------------¬
                Y ahora supongamos que el índiceBitDeByteEnCuadricula es 4       |
                Lo vamos a desplazar (7 - 4) veces hacia la derecha. Queremos    |
                el bit número 3 =================================================╝

                Al desplazarlo será:
                00011001
                       ^
                       |___________________________________
                                                          |
                Ya tenemos el bit que nos importa en el LSB. Ahora hacemos un AND con
                el 1 entero que solo tiene un 1 en el LSB y se ve así:
                00000001

                Hagamos el and

                00000001
                &
                00011001
                ________
                00000001

                Recordemos que el AND tiene todos los bits en cero excepto por el LSB. Entonces nos dará siempre
                un 1 decimal o un 0 decimal. En este caso el resultado fue 1, porque en el índice 4 sí hay un 1.

                Otro ejemplo con el 200 pero con el índice en 5. Vamos a desplazarlo (7-5) veces. Originalmente:

                11001000
                Lo movemos 2 veces a la derecha:
                00110010
                En el LSB hay un 0. Al hacer el AND:
                00110010
                &
                00000001
                _______
                00000000

                Y da 0 decimal porque ahí no hay un cuadro que colisione
                 */

                int xEnByteDeCuadricula = xEnCuadriculaDespuesDeModificar / BITS_EN_UN_BYTE;
                int indiceBitDeByteEnCuadricula = xEnCuadriculaDespuesDeModificar % BITS_EN_UN_BYTE;
                if ((cuadricula[yEnCuadriculaDespuesDeModificar][xEnByteDeCuadricula] >> (MAXIMO_INDICE_BIT_EN_BYTE - indiceBitDeByteEnCuadricula)) & 1)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

int main()
{
    al_init();
    al_init_primitives_addon();
    al_install_keyboard();

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 30.0);
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();

    /*Timers para bajar pieza automáticamente*/
    ALLEGRO_TIMER *timer_bajar_pieza = al_create_timer(0.5);
    al_register_event_source(queue, al_get_timer_event_source(timer_bajar_pieza));
    al_start_timer(timer_bajar_pieza);
    /*Terminan timers para bajar pieza automáticamente*/

    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
    ALLEGRO_DISPLAY *disp = al_create_display(ANCHURA, ALTURA);
    ALLEGRO_FONT *font = al_create_builtin_font();

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool redraw = true;
    ALLEGRO_EVENT event;

    al_start_timer(timer);
    ALLEGRO_COLOR blanco = al_map_rgb_f(255, 255, 255);
    ALLEGRO_COLOR rojo = al_map_rgb_f(255, 0, 0);
    ALLEGRO_COLOR azul = al_map_rgb_f(0, 0, 255);
    ALLEGRO_COLOR verde = al_map_rgb_f(0, 255, 0);

    uint8_t otraCuadricula[ALTO_CUADRICULA][ANCHO_CUADRICULA] = {
        {0, 0, 0, 0},
        {0, 0, 0, 255},
        {0, 0, 0, 255},
        {0, 0, 0, 255},
    };
    al_init_font_addon();
    al_init_ttf_addon();
    ALLEGRO_FONT *fuente = al_load_font("arial.ttf", 11, 0);
    struct Tetrimino linea;
    // Linea acostada es 240 porque necesitamos encendidos los primeros 4 bits
    // Línea vertical sería 136 y 136 porque necesitamos el bit 1 y 4 (128 y 8)
    /*
    1110
    0100
    0000
    0000
    Primer byte: 11100100
    4+128+64+32
    228

    1100
    1100
    0000
    0000
    11001100
    12+128+64
    204
    */
    // encendidos en cada bit
    linea.cuadricula[0] = 128;
    linea.cuadricula[1] = 0;
    linea.x = 0;
    linea.y = 0;

    while (1)
    {
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_TIMER)
        {
            if (event.timer.source == timer)
            {
                redraw = true;
            }
            else if (event.timer.source == timer_bajar_pieza)
            {
                if (!tetriminoColisionaConCuadriculaAlAvanzar(&linea, otraCuadricula, 0, 1))
                {
                    linea.y++;
                }
            }
        }
        else if (event.type == ALLEGRO_EVENT_KEY_CHAR)
        {
            int teclaPresionada = event.keyboard.keycode;
            if (teclaPresionada == ALLEGRO_KEY_UP)
            {
                printf("Arriba");
                if (!tetriminoColisionaConCuadriculaAlAvanzar(&linea, otraCuadricula, 0, -1))
                {
                    linea.y--;
                }
            }
            else if (teclaPresionada == ALLEGRO_KEY_DOWN)
            {
                printf("Abajo");
                if (!tetriminoColisionaConCuadriculaAlAvanzar(&linea, otraCuadricula, 0, 1))
                {
                    linea.y++;
                }
            }
            else if (teclaPresionada == ALLEGRO_KEY_LEFT)
            {
                if (!tetriminoColisionaConCuadriculaAlAvanzar(&linea, otraCuadricula, -1, 0))
                {
                    printf("Izquierda");
                    linea.x--;
                }
            }
            else if (teclaPresionada == ALLEGRO_KEY_RIGHT)
            {

                if (!tetriminoColisionaConCuadriculaAlAvanzar(&linea, otraCuadricula, 1, 0))
                {
                    linea.x++;
                    printf("Derecha");
                }
                else
                {
                    printf("Colapsa en la derecha");
                }
            }
        }
        else if ((event.type == ALLEGRO_EVENT_DISPLAY_CLOSE))
        {
            break;
        }

        if (redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            // al_draw_text(font, al_map_rgb(255, 255, 255), 0, 0, 0, "Hola mundo");

            float xCoordenada = 0, yCoordenada = 0;
            for (int y = 0; y < ALTO_CUADRICULA; y++)
            {
                for (int x = 0; x < ANCHO_CUADRICULA; x++)
                {
                    for (int i = 0; i < BITS_EN_UN_BYTE; i++)
                    {
                        // int encendido = (otraCuadricula[y][x] >> i) & 1;
                        int encendido = (otraCuadricula[y][x] >> (MAXIMO_INDICE_BIT_EN_BYTE - i)) & 1;
                        // printf("x=%d,y=%d. Estamos en %d con bit %d. Encendido? %d. Quiero %d\n",
                        // x, y, otraCuadricula[y][x], i, encendido, verdaderoX);
                        al_draw_filled_rectangle(xCoordenada, yCoordenada, xCoordenada + MEDIDA_CUADRO, yCoordenada + MEDIDA_CUADRO, encendido == 0 ? blanco : rojo);
                        al_draw_textf(
                            fuente,
                            encendido == 0 ? blanco : rojo,
                            xCoordenada,
                            yCoordenada,
                            0,
                            "(%d,%d)",
                            (x * BITS_EN_UN_BYTE) + i,
                            y);

                        xCoordenada += MEDIDA_CUADRO;
                    }
                    // printf("======\n");
                    //  printf("En esa pos hay %d", otraCuadricula[y][x]);
                    //  int indice = y * ancho + x;
                    //  printf("x=%d,y=%d. El indice es %d\n", x, y, indice);
                }
                xCoordenada = 0;
                yCoordenada += MEDIDA_CUADRO;
            }
            // Y ahora dibujamos la pieza

            // Cada figura vive en una cuadrícula de 4x4. 16 cuadros
            // en total, medio byte por línea, 2 bytes en total
            for (int indiceChar = 0; indiceChar < CANTIDAD_BYTES_TETRIMINO; indiceChar++)
            {
                for (int indiceBit = 0; indiceBit < BITS_EN_UN_BYTE; indiceBit++)
                {
                    // El índice del char me lo da xd
                    // linea.cuadricula[xd]
                    // Como ya tengo un int solo necesito saber la posición de su bit
                    int encendidoLocal = (linea.cuadricula[indiceChar] >> (MAXIMO_INDICE_BIT_EN_BYTE - indiceBit)) & 1;
                    // Y ya tengo el valor, pero necesito saber en cuál de 4x4 estoy
                    // 0,1,2,3
                    // 4,5,6,7
                    // 8,9,10,11
                    // 12,13,14,15
                    int sumaX = linea.x + (indiceBit % CUADRICULA_TETRIMINO);
                    int sumaY = linea.y + (indiceChar * CANTIDAD_BYTES_TETRIMINO) + (indiceBit / CUADRICULA_TETRIMINO);
                    // printf("Char %d Bit %d sumaX %d sumaY %d encendido? %d\n", indiceChar, indiceBit, sumaX, sumaY, encendidoLocal);
                    if (encendidoLocal)
                    {
                        al_draw_filled_rectangle(sumaX * MEDIDA_CUADRO, sumaY * MEDIDA_CUADRO, (sumaX * MEDIDA_CUADRO) + MEDIDA_CUADRO, (sumaY * MEDIDA_CUADRO) + MEDIDA_CUADRO, azul);
                    }
                    else
                    {
                        al_draw_filled_rectangle(sumaX * MEDIDA_CUADRO, sumaY * MEDIDA_CUADRO, (sumaX * MEDIDA_CUADRO) + MEDIDA_CUADRO, (sumaY * MEDIDA_CUADRO) + MEDIDA_CUADRO, verde);
                    }
                    al_draw_textf(
                        fuente,
                        blanco,
                        sumaX * MEDIDA_CUADRO,
                        sumaY * MEDIDA_CUADRO,
                        0,
                        "(%d,%d)",
                        sumaX,
                        sumaY);
                }
            }
            al_flip_display();

            redraw = false;
            /*
                        int i, j;
                        float x = 0, y = 0;
                        for (i = 0; i < 5; i++)
                        {
                            for (j = 0; j < 5; j++)
                            {
                                al_draw_filled_rectangle(x, y, x + MEDIDA_CUADRO, y + MEDIDA_CUADRO, cuadricula[i][j] == 0 ? blanco : rojo);
                                x += MEDIDA_CUADRO;
                            }
                            x = 0;
                            y += MEDIDA_CUADRO;
                        }
                        al_flip_display();

                        redraw = false;
            */
        }
    }

    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}