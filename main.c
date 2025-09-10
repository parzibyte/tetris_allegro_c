#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <stdbool.h>
#include <stdio.h>
#include <allegro5/allegro_ttf.h>
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
    unsigned char cuadricula[CANTIDAD_BYTES_TETRIMINO];
    int x, y;
};

int colapsaTetriminoConCuadricula(struct Tetrimino *linea, unsigned char cuad[ALTO_CUADRICULA][ANCHO_CUADRICULA])
{
    for (int indiceChar = 0; indiceChar < CANTIDAD_BYTES_TETRIMINO; indiceChar++)
    {
        for (int indiceBit = 0; indiceBit < BITS_EN_UN_BYTE; indiceBit++)
        {
            // El índice del char me lo da xd
            // linea.cuadricula[xd]
            // Como ya tengo un int solo necesito saber la posición de su bit
            int encendidoLocal = (linea->cuadricula[indiceChar] >> (MAXIMO_INDICE_BIT_EN_BYTE - indiceBit)) & 1;
            // Y ya tengo el valor, pero necesito saber en cuál de 4x4 estoy
            // 0,1,2,3
            // 4,5,6,7
            // 8,9,10,11
            // 12,13,14,15
            int sumaX = linea->x + (indiceBit % 4);
            int sumaY = linea->y + (indiceChar * CANTIDAD_BYTES_TETRIMINO) + (indiceBit / 4);

            if (encendidoLocal)
            {
                /**
                 * Hasta este punto sumaX y sumaY ya tienen la coordenada completa sumando
                 * el origen del tetrimino hasta el bit actual
                 */
                int xParaAcceder = sumaX / BITS_EN_UN_BYTE;
                int indiceBit2 = sumaX % BITS_EN_UN_BYTE;
                printf("En la cuad[%d][%d] hay un %d\n", sumaY, xParaAcceder, cuad[sumaY][xParaAcceder]);
                if ((cuad[sumaY][xParaAcceder] >> (MAXIMO_INDICE_BIT_EN_BYTE - indiceBit2)) & 1)
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}

int main()
{
    al_init();
    al_init_primitives_addon();
    al_install_keyboard();

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 30.0);
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();

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

    unsigned char otraCuadricula[ALTO_CUADRICULA][ANCHO_CUADRICULA] = {
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
    linea.cuadricula[0] = 204;
    linea.cuadricula[1] = 0;
    linea.x = 0;
    linea.y = 0;

    while (1)
    {
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_TIMER)
        {
            redraw = true;
        }
        else if (event.type == ALLEGRO_EVENT_KEY_CHAR)
        {
            int teclaPresionada = event.keyboard.keycode;
            if (teclaPresionada == ALLEGRO_KEY_UP)
            {
                printf("Arriba");
                if (!colapsaTetriminoConCuadricula(&linea, otraCuadricula))
                {
                    linea.y--;
                }
            }
            else if (teclaPresionada == ALLEGRO_KEY_DOWN)
            {
                printf("Abajo");
                if (!colapsaTetriminoConCuadricula(&linea, otraCuadricula))
                {
                    linea.y++;
                }
            }
            else if (teclaPresionada == ALLEGRO_KEY_LEFT)
            {
                if (!colapsaTetriminoConCuadricula(&linea, otraCuadricula))
                {
                    printf("Izquierda");
                    linea.x--;
                }
            }
            else if (teclaPresionada == ALLEGRO_KEY_RIGHT)
            {

                if (!colapsaTetriminoConCuadricula(&linea, otraCuadricula))
                {
                    linea.x++;
                    printf("Derecha");
                }else{
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
                        sumaX*MEDIDA_CUADRO,
                        sumaY*MEDIDA_CUADRO,
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