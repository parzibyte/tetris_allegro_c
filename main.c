#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <stdbool.h>
#include <stdio.h>

#define ALTURA 500
#define ANCHURA 500
#define MEDIDA_CUADRO 15

struct Tetrimino
{
    // medio byte por línea
    unsigned char cuadricula[2];
    int x, y;
};

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
    int ancho = 4;
    int alto = 2;

    unsigned char otraCuadricula[2][4] = {
        {129, 0, 129, 0},
        {0, 129, 0, 129},
    };

    struct Tetrimino linea;
    // Linea acostada es 240 porque necesitamos encendidos los primeros 4 bits
    // Línea vertical sería 136 y 136 porque necesitamos el bit 1 y 4 (128 y 8)
    // encendidos en cada bit
    linea.cuadricula[0] = 136;
    linea.cuadricula[1] = 136;
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
                linea.y--;
            }
            else if (teclaPresionada == ALLEGRO_KEY_DOWN)
            {
                printf("Abajo");
                linea.y++;
            }
            else if (teclaPresionada == ALLEGRO_KEY_LEFT)
            {
                printf("Izquierda");
                linea.x--;
            }
            else if (teclaPresionada == ALLEGRO_KEY_RIGHT)
            {
                linea.x++;
                printf("Derecha");
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
            for (int y = 0; y < alto; y++)
            {
                for (int x = 0; x < ancho; x++)
                {
                    for (int i = 0; i < 8; i++)
                    {
                        // int encendido = (otraCuadricula[y][x] >> i) & 1;
                        int encendido = (otraCuadricula[y][x] >> (7 - i)) & 1;
                        // printf("x=%d,y=%d. Estamos en %d con bit %d. Encendido? %d. Quiero %d\n",
                        // x, y, otraCuadricula[y][x], i, encendido, verdaderoX);
                        al_draw_filled_rectangle(xCoordenada, yCoordenada, xCoordenada + MEDIDA_CUADRO, yCoordenada + MEDIDA_CUADRO, encendido == 0 ? blanco : rojo);
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
            for (int indiceChar = 0; indiceChar < 2; indiceChar++)
            {
                for (int indiceBit = 0; indiceBit < 8; indiceBit++)
                {
                    // El índice del char me lo da xd
                    // linea.cuadricula[xd]
                    // Como ya tengo un int solo necesito saber la posición de su bit
                    int encendidoLocal = (linea.cuadricula[indiceChar] >> (7 - indiceBit)) & 1;
                    // Y ya tengo el valor, pero necesito saber en cuál de 4x4 estoy
                    // 0,1,2,3
                    // 4,5,6,7
                    // 8,9,10,11
                    // 12,13,14,15
                    int sumaX = linea.x + (indiceBit % 4);
                    int sumaY = linea.y + (indiceChar * 2) + (indiceBit / 4);
                    // printf("Char %d Bit %d sumaX %d sumaY %d encendido? %d\n", indiceChar, indiceBit, sumaX, sumaY, encendidoLocal);
                    if (encendidoLocal)
                    {
                        al_draw_filled_rectangle(sumaX * MEDIDA_CUADRO, sumaY * MEDIDA_CUADRO, (sumaX * MEDIDA_CUADRO) + MEDIDA_CUADRO, (sumaY * MEDIDA_CUADRO) + MEDIDA_CUADRO, azul);
                    }
                    else
                    {
                        al_draw_filled_rectangle(sumaX * MEDIDA_CUADRO, sumaY * MEDIDA_CUADRO, (sumaX * MEDIDA_CUADRO) + MEDIDA_CUADRO, (sumaY * MEDIDA_CUADRO) + MEDIDA_CUADRO, verde);
                    }
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