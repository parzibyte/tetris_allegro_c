#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <stdbool.h>
#include <stdio.h>
#include <allegro5/allegro_ttf.h>
#include <stdbool.h>
#include <allegro5/allegro_image.h>
#define OFFSET_X 200
#define MEDIDA_CUADRO 32
#define ANCHO_CUADRICULA 2 // Recuerda que se multiplicará por BITS_EN_UN_BYTE así que si es 2 en realidad es 16
#define ALTO_CUADRICULA 30
#define BITS_EN_UN_BYTE 8
#define BITS_EN_UINT16 16
#define MAXIMO_INDICE_BIT_EN_BYTE 7
#define MAXIMO_INDICE_BIT_EN_UINT16 15
#define CUADRICULA_TETRIMINO 4
#define TOTAL_TETRIMINOS_DISPONIBLES 7
#define BITS_POR_FILA_PARA_TETRIMINO 4
#define MITAD_CUADRICULA_X ANCHO_CUADRICULA *BITS_EN_UN_BYTE / 2 - (BITS_POR_FILA_PARA_TETRIMINO / 2);
#define GROSOR_BORDE 10

struct Tetrimino
{
    uint16_t cuadricula;
    uint8_t x, y;
};
struct TetriminoParaElegir
{
    uint16_t cuadricula;
};
uint8_t indiceGlobalTetrimino = 0;

void elegirPiezaAleatoria(struct Tetrimino *destino, struct TetriminoParaElegir piezasDisponibles[TOTAL_TETRIMINOS_DISPONIBLES])
{
    destino->cuadricula = piezasDisponibles[indiceGlobalTetrimino].cuadricula;
    destino->x = MITAD_CUADRICULA_X;
    destino->y = 0;
    indiceGlobalTetrimino++;
    if (indiceGlobalTetrimino > TOTAL_TETRIMINOS_DISPONIBLES - 1)
    {

        aleatorizarPiezas(piezasDisponibles);
        indiceGlobalTetrimino = 0;
    }
}
void elegirSiguientePieza(struct Tetrimino *actual, struct Tetrimino *siguiente, struct TetriminoParaElegir piezas[TOTAL_TETRIMINOS_DISPONIBLES])
{
    actual->cuadricula = siguiente->cuadricula;
    actual->x = siguiente->x = MITAD_CUADRICULA_X;
    actual->y = siguiente->y = 0;
    elegirPiezaAleatoria(siguiente, piezas);
}

void aleatorizarPiezas(struct TetriminoParaElegir piezas[TOTAL_TETRIMINOS_DISPONIBLES])
{
    /*
     Cantidad = Tamaño(Array)
     Recorrer con k desde Cantidad-1 hasta 1 Regresivamente
        az = Random(entre 0 y k)

        tmp = Array(az)
        Array(az) = Array(k)
        Array(k) = tmp
     Siguiente
     */

    for (int k = TOTAL_TETRIMINOS_DISPONIBLES - 1; k > 0; k--)
    {
        uint8_t az = rand() % (k + 1);
        struct TetriminoParaElegir tmp = piezas[az];
        piezas[az] = piezas[k];
        piezas[k] = tmp;
    }
}
void inicializarPiezas(struct TetriminoParaElegir piezas[TOTAL_TETRIMINOS_DISPONIBLES])
{
    /*
    Veamos la Z es
    1100
    0110
    0000
    0000

    Que sería C6
     */
    piezas[0].cuadricula = 0xC600;
    /*
    La L es
    1000
    1000
    1100
    0000
    Que sería 88C0
    */
    piezas[1].cuadricula = 0x88C0;
    /*
    Ahora una línea
    1111
    0000
    0000
    0000
    Solo sería F000
    */
    piezas[2].cuadricula = 0xF000;
    /*
    Ahora el cuadro
    1100
    1100
    0000
    0000
    Que sería CC00
    */
    piezas[3].cuadricula = 0xCC00;
    /*
    Ahora la T
    Esa es
    1110
    0100
    0000
    0000
    Que sería E400
    */
    piezas[4].cuadricula = 0xE400;
    /*
    La L invertida que sería
    1110
    0010
    0000
    0000
    E200
    */
    piezas[5].cuadricula = 0xe200;
    /*
    La Z invertida que sería
    0110
    1100
    0000
    0000
    6C00
     */
    piezas[6].cuadricula = 0x6c00;
}
uint16_t rotar90CW(uint16_t pieza)
{
    // (x', y') = ( y, 3 - x )
    uint16_t rotado = 0;
    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            int bit = (pieza >> (MAXIMO_INDICE_BIT_EN_UINT16 - (y * 4 + x))) & 1;
            int xPrima = y;
            int yPrima = 3 - x;
            rotado |= bit << (MAXIMO_INDICE_BIT_EN_UINT16 - (yPrima * 4 + xPrima));
        }
    }
    /*
    Recordemos que 0xf000 es
    1111
    0000
    0000
    0000

    Y le hacemos un AND con la pieza rotada. Va a devolver
    0 siempre que la pieza  tenga únicamente ceros en los primeros 4 bits

    Por ejemplo, tenemos la pieza
    0100
    0110
    0010
    0000

    Hacemos un AND:
    0100011000100000
    &
    1111000000000000
    El resultado es 0100000000000000, mismo que es distinto a 0000000000000000, es correcto
    porque no tiene filas vacías al inicio.

    Pero ahora veamos con la siguiente pieza:

    0000
    0110
    0011
    0000

    Le hacemos un AND:
    0000011000110000
    &
    1111000000000000

    El resultado es
    0000000000000000

    Lo cual es exactamente a 0

    Entonces cuando se cumple esta condición desplazamos 4 bits a la izquierda,
    lo que es como subir la pieza


     */
    while ((rotado & 0xF000) == 0)
    {
        rotado <<= 4;
    }
    /*
        0x8888 sería:
        1000
        1000
        1000
        1000

        Y hacemos lo mismo pero ahora para arrimar
        la pieza a la izquierda
    */

    while ((rotado & 0x8888) == 0)
    {
        rotado <<= 1;
    }
    return rotado;
}

/*
Recibe un apuntador al tetrimino y la cuadrícula del tetris
*/
bool tetriminoColisionaConCuadriculaAlRotar(struct Tetrimino *tetrimino, uint8_t cuadricula[ALTO_CUADRICULA][ANCHO_CUADRICULA])
{
    /*
    Nuevo código porque usamos uint16_t
    */
    for (uint8_t indiceBit = 0; indiceBit < BITS_EN_UINT16; indiceBit++)
    {
        // Primero rotamos. No usaremos tetrimino->cuadricula sino lo rotado
        uint16_t rotado = rotar90CW(tetrimino->cuadricula);

        bool hayUnCuadroDeTetriminoEnLaCoordenadaActual = (rotado >> (MAXIMO_INDICE_BIT_EN_UINT16 - indiceBit)) & 1;
        if (!hayUnCuadroDeTetriminoEnLaCoordenadaActual)
        {
            continue;
        }
        // Llegados aquí sabemos que el "continue" no se ejecutó y que SÍ hay un tetrimino

        // Coordenadas sobre la cuadrícula después de aplicar los modificadores
        uint8_t xRelativoDentroDeCuadricula = indiceBit % BITS_POR_FILA_PARA_TETRIMINO;
        uint8_t YRelativoDentroDeCuadricula = indiceBit / BITS_POR_FILA_PARA_TETRIMINO;
        int xEnCuadriculaDespuesDeModificar = tetrimino->x + xRelativoDentroDeCuadricula;
        int yEnCuadriculaDespuesDeModificar = tetrimino->y + YRelativoDentroDeCuadricula;
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
        Hasta este punto las coordenadas ya son seguras y ya las tenemos simuladas con el avance
        */
        int xEnByteDeCuadricula = xEnCuadriculaDespuesDeModificar / BITS_EN_UN_BYTE;
        int indiceBitDeByteEnCuadricula = xEnCuadriculaDespuesDeModificar % BITS_EN_UN_BYTE;
        if ((cuadricula[yEnCuadriculaDespuesDeModificar][xEnByteDeCuadricula] >> (MAXIMO_INDICE_BIT_EN_BYTE - indiceBitDeByteEnCuadricula)) & 1)
        {
            return true;
        }
    }
    return false;
}
/*
Recibe un apuntador al tetrimino, la cuadrícula del tetris y dos modificadores x e y.
La función aumentará las coordenadas del tetrimino a partir de los modificadores simulando un avance y
después va a devolver true si el tetrimino colisiona con una pared, suelo u otras piezas
*/
bool tetriminoColisionaConCuadriculaAlAvanzar(struct Tetrimino *tetrimino, uint8_t cuadricula[ALTO_CUADRICULA][ANCHO_CUADRICULA], int8_t modificadorX, int8_t modificadorY)
{
    /*
    Nuevo código porque usamos uint16_t
    */
    for (uint8_t indiceBit = 0; indiceBit < BITS_EN_UINT16; indiceBit++)
    {

        bool hayUnCuadroDeTetriminoEnLaCoordenadaActual = (tetrimino->cuadricula >> (MAXIMO_INDICE_BIT_EN_UINT16 - indiceBit)) & 1;
        if (!hayUnCuadroDeTetriminoEnLaCoordenadaActual)
        {
            continue;
        }
        // Llegados aquí sabemos que el "continue" no se ejecutó y que SÍ hay un tetrimino

        // Coordenadas sobre la cuadrícula después de aplicar los modificadores
        uint8_t xRelativoDentroDeCuadricula = indiceBit % BITS_POR_FILA_PARA_TETRIMINO;
        uint8_t YRelativoDentroDeCuadricula = indiceBit / BITS_POR_FILA_PARA_TETRIMINO;
        int xEnCuadriculaDespuesDeModificar = tetrimino->x + xRelativoDentroDeCuadricula + modificadorX;
        int yEnCuadriculaDespuesDeModificar = tetrimino->y + YRelativoDentroDeCuadricula + modificadorY;
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
        Hasta este punto las coordenadas ya son seguras y ya las tenemos simuladas con el avance
        */
        int xEnByteDeCuadricula = xEnCuadriculaDespuesDeModificar / BITS_EN_UN_BYTE;
        int indiceBitDeByteEnCuadricula = xEnCuadriculaDespuesDeModificar % BITS_EN_UN_BYTE;
        if ((cuadricula[yEnCuadriculaDespuesDeModificar][xEnByteDeCuadricula] >> (MAXIMO_INDICE_BIT_EN_BYTE - indiceBitDeByteEnCuadricula)) & 1)
        {
            return true;
        }
    }
    return false;
}

int8_t indiceYParaFantasma(struct Tetrimino *tetrimino, uint8_t cuadricula[ALTO_CUADRICULA][ANCHO_CUADRICULA])
{
    for (uint8_t y = 0; y <= ALTO_CUADRICULA; y++)
    {
        if (tetriminoColisionaConCuadriculaAlAvanzar(tetrimino, cuadricula, 0, y - tetrimino->y))
        {
            return y - 1;
        }
    }
    // TODO: tal vez no usar un uint8_t y devolver -1
    return -1;
}

/*
Devuelve el primer índice de la fila llena comenzando desde abajo (desde ALTO_CUADRICULA - 1)
Si no hay ninguna fila llena devuelve -1
*/
int8_t indiceFilaLlena(uint8_t cuadricula[ALTO_CUADRICULA][ANCHO_CUADRICULA])
{
    for (int y = ALTO_CUADRICULA - 1; y >= 0; y--)
    {
        bool filaLlena = true;
        for (int x = 0; x < ANCHO_CUADRICULA; x++)
        {
            filaLlena = filaLlena && cuadricula[y][x] == 255;
        }
        if (filaLlena)
        {
            return y;
        }
    }
    return -1;
}

void limpiarFilaYBajarFilasSuperiores(int8_t indiceFila, uint8_t cuadricula[ALTO_CUADRICULA][ANCHO_CUADRICULA])
{
    // Bajamos las superiores
    for (; indiceFila > 0; indiceFila--)
    {
        memcpy(cuadricula[indiceFila], cuadricula[indiceFila - 1], sizeof(cuadricula[indiceFila]));
    }
    // Y estamos seguros de que hasta arriba (y=0) se quedó una fila disponible, la dejamos en ceros
    memset(cuadricula[0], 0, sizeof(cuadricula[0]));
}

void bajarTetrimino(struct Tetrimino *tetrimino, struct Tetrimino *siguiente, uint8_t cuadricula[ALTO_CUADRICULA][ANCHO_CUADRICULA], bool *bandera, unsigned long *puntajeGlobal, bool *juegoTerminado, struct TetriminoParaElegir piezas[TOTAL_TETRIMINOS_DISPONIBLES])
{
    if (!tetriminoColisionaConCuadriculaAlAvanzar(tetrimino, cuadricula, 0, 1))
    {
        tetrimino->y++;
        *bandera = false;
    }
    else
    {

        // Ya te había avisado que te movieras. Esto significa que no te moviste y por lo tanto toca spawnear
        // una nueva pieza
        if (bandera)
        {
            /*
            Otra vez código nuevo porque migramos a uint16_t
            Primero toca copiar la pieza actual a la cuadrícula maestra

            */
            for (uint8_t indiceBit = 0; indiceBit < BITS_EN_UINT16; indiceBit++)
            {

                bool hayUnCuadroDeTetriminoEnLaCoordenadaActual = (tetrimino->cuadricula >> (MAXIMO_INDICE_BIT_EN_UINT16 - indiceBit)) & 1;
                if (!hayUnCuadroDeTetriminoEnLaCoordenadaActual)
                {
                    continue;
                }
                // Llegados aquí sabemos que el "continue" no se ejecutó y que SÍ hay un tetrimino

                // Coordenadas sobre la cuadrícula después de aplicar los modificadores
                uint8_t xRelativoDentroDeCuadricula = indiceBit % BITS_POR_FILA_PARA_TETRIMINO;
                uint8_t YRelativoDentroDeCuadricula = indiceBit / BITS_POR_FILA_PARA_TETRIMINO;
                int xEnCuadriculaDespuesDeModificar = tetrimino->x + xRelativoDentroDeCuadricula;
                int yEnCuadriculaDespuesDeModificar = tetrimino->y + YRelativoDentroDeCuadricula;
                int xEnByteDeCuadricula = xEnCuadriculaDespuesDeModificar / BITS_EN_UN_BYTE;
                int indiceBitDeByteEnCuadricula = xEnCuadriculaDespuesDeModificar % BITS_EN_UN_BYTE;
                cuadricula[yEnCuadriculaDespuesDeModificar][xEnByteDeCuadricula] = cuadricula[yEnCuadriculaDespuesDeModificar][xEnByteDeCuadricula] | (1 << (MAXIMO_INDICE_BIT_EN_BYTE - indiceBitDeByteEnCuadricula));
            }

            // Limpiar filas y bajar filas superiores hasta que ya no haya filas llenas
            /*
            Aquí puede que haya un "tetris", entonces calculamos un puntaje o cosas de esas, así que
            calculamos el puntaje
            */
            uint8_t lineasEliminadasConsecutivamente = 0;
            while (1)
            {
                int8_t posibleIndiceFilaLlena = indiceFilaLlena(cuadricula);
                // No hay filas llenas. Nada que limpiar
                if (posibleIndiceFilaLlena == -1)
                {
                    printf("Líneas consecutivas: %d\n", lineasEliminadasConsecutivamente);
                    *puntajeGlobal += lineasEliminadasConsecutivamente;
                    lineasEliminadasConsecutivamente = 0;
                    break;
                }
                else
                {
                    limpiarFilaYBajarFilasSuperiores(posibleIndiceFilaLlena, cuadricula);
                    lineasEliminadasConsecutivamente++;
                }
            }
            elegirSiguientePieza(tetrimino, siguiente, piezas);
            if (tetriminoColisionaConCuadriculaAlAvanzar(tetrimino, cuadricula, 0, 0))
            {
                *juegoTerminado = true;
            }
        }
        else
        {
            // No puedes bajar pero te doy un tiempo para que te puedas mover
            *bandera = true;
        }
    }
}

int main()
{
    srand(time(NULL));
    al_init();
    al_init_primitives_addon();
    al_install_keyboard();
    if (!al_init_image_addon())
    {
        fprintf(stderr, "Error inicializando librería imagen");
    }
    ALLEGRO_BITMAP *imagen_pieza_caida = NULL;
    ALLEGRO_BITMAP *imagen_pieza_movimiento = NULL;

    imagen_pieza_caida = al_load_bitmap("bitmap.png");

    if (!imagen_pieza_caida)
    {
        fprintf(stderr, "Error cargando imagen");
    }

    imagen_pieza_movimiento = al_load_bitmap("2.png");

    if (!imagen_pieza_movimiento)
    {
        fprintf(stderr, "Error cargando imagen");
    }

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 30.0);
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();

    /*Timers para bajar pieza automáticamente*/
    ALLEGRO_TIMER *timer_bajar_pieza = al_create_timer(1);
    al_register_event_source(queue, al_get_timer_event_source(timer_bajar_pieza));
    al_start_timer(timer_bajar_pieza);
    /*Terminan timers para bajar pieza automáticamente*/

    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
    // Aquí se configura el tamaño de la pantalla
    ALLEGRO_DISPLAY *disp = al_create_display((MEDIDA_CUADRO * ANCHO_CUADRICULA * BITS_EN_UN_BYTE) + OFFSET_X, (ALTO_CUADRICULA * MEDIDA_CUADRO) + GROSOR_BORDE * 2);
    ALLEGRO_FONT *font = al_create_builtin_font();

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool redraw = true;
    ALLEGRO_EVENT event;

    al_start_timer(timer);
    ALLEGRO_COLOR blanco = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR negro = al_map_rgb_f(0, 0, 0);
    ALLEGRO_COLOR rojo = al_map_rgb_f(255, 0, 0);
    ALLEGRO_COLOR azul = al_map_rgb(0, 0, 255);
    ALLEGRO_COLOR color_borde = al_map_rgb(136, 195, 191);
    ALLEGRO_COLOR verde = al_map_rgb_f(0, 255, 0);

    uint8_t otraCuadricula[ALTO_CUADRICULA][ANCHO_CUADRICULA] = {
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
    };
    struct Tetrimino piezaActual;
    struct Tetrimino piezaSiguiente;
    struct TetriminoParaElegir piezas[TOTAL_TETRIMINOS_DISPONIBLES];
    inicializarPiezas(piezas);
    aleatorizarPiezas(piezas);
    elegirPiezaAleatoria(&piezaActual, piezas);
    elegirPiezaAleatoria(&piezaSiguiente, piezas);
    al_init_font_addon();
    al_init_ttf_addon();
    ALLEGRO_FONT *fuente = al_load_font("arial.ttf", 20, 0);
    bool banderaTocoSuelo = false;
    bool juegoTerminado = false;
    unsigned long puntajeGlobal = 0;

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
                if (!juegoTerminado)
                {
                    bajarTetrimino(&piezaActual, &piezaSiguiente, otraCuadricula, &banderaTocoSuelo, &puntajeGlobal, &juegoTerminado, piezas);
                }
            }
        }
        else if (event.type == ALLEGRO_EVENT_KEY_CHAR)
        {
            int teclaPresionada = event.keyboard.keycode;
            if (!juegoTerminado)
            {

                if (teclaPresionada == ALLEGRO_KEY_UP || teclaPresionada == ALLEGRO_KEY_K)
                {
                    piezaActual.y = indiceYParaFantasma(&piezaActual, otraCuadricula);
                    banderaTocoSuelo = true;
                    bajarTetrimino(&piezaActual, &piezaSiguiente, otraCuadricula, &banderaTocoSuelo, &puntajeGlobal, &juegoTerminado, piezas);
                }
                else if (teclaPresionada == ALLEGRO_KEY_J || teclaPresionada == ALLEGRO_KEY_DOWN)
                {
                    bajarTetrimino(&piezaActual, &piezaSiguiente, otraCuadricula, &banderaTocoSuelo, &puntajeGlobal, &juegoTerminado, piezas);
                }
                else if (teclaPresionada == ALLEGRO_KEY_H || teclaPresionada == ALLEGRO_KEY_LEFT)
                {
                    if (!tetriminoColisionaConCuadriculaAlAvanzar(&piezaActual, otraCuadricula, -1, 0))
                    {
                        piezaActual.x--;
                    }
                }
                else if (teclaPresionada == ALLEGRO_KEY_L || teclaPresionada == ALLEGRO_KEY_RIGHT)
                {
                    if (!tetriminoColisionaConCuadriculaAlAvanzar(&piezaActual, otraCuadricula, 1, 0))
                    {
                        piezaActual.x++;
                    }
                }
                else if (teclaPresionada == ALLEGRO_KEY_SPACE)
                {
                    if (!tetriminoColisionaConCuadriculaAlRotar(&piezaActual, otraCuadricula))
                    {
                        piezaActual.cuadricula = rotar90CW(piezaActual.cuadricula);
                    }
                }
            }
            else
            {
                puntajeGlobal = 0;
                memset(otraCuadricula, 0, ANCHO_CUADRICULA * ALTO_CUADRICULA);
                juegoTerminado = false;
                elegirSiguientePieza(&piezaActual, &piezaSiguiente, piezas);
            }
        }
        else if ((event.type == ALLEGRO_EVENT_DISPLAY_CLOSE))
        {
            break;
        }

        if (redraw && al_is_event_queue_empty(queue))
        {
            if (juegoTerminado)
            {
                al_clear_to_color(al_map_rgb(255, 255, 0));
                al_draw_textf(
                    fuente,
                    rojo,
                    0,
                    0,
                    0,
                    "Juego terminado. Puntaje: %lu. Presiona una tecla para reiniciar",
                    puntajeGlobal);
                redraw = true;
                al_flip_display();
                continue;
            }
            // Evitar que haya líneas azules o de colores raros
            al_clear_to_color(al_map_rgb(0, 0, 0));
            /*
            =========================================
            Comienza dibujo de la cuadrícula donde las piezas ya han caído
            =========================================
            */

            float xCoordenada = 0, yCoordenada = 0;
            for (int y = 0; y < ALTO_CUADRICULA; y++)
            {
                for (int x = 0; x < ANCHO_CUADRICULA; x++)
                {
                    for (int i = 0; i < BITS_EN_UN_BYTE; i++)
                    {
                        int encendido = (otraCuadricula[y][x] >> (MAXIMO_INDICE_BIT_EN_BYTE - i)) & 1;
                        // Si hay un 1 en este bit entonces dibujamos la imagen
                        if (encendido)
                        {

                            /**/
                            float sw = al_get_bitmap_width(imagen_pieza_caida);
                            float sh = al_get_bitmap_height(imagen_pieza_caida);

                            float dx = xCoordenada + GROSOR_BORDE;
                            float dy = yCoordenada + GROSOR_BORDE;
                            float dw = MEDIDA_CUADRO;
                            float dh = MEDIDA_CUADRO;

                            al_draw_scaled_bitmap(imagen_pieza_caida,
                                                  0, 0, sw, sh,
                                                  dx, dy, dw, dh, 0);
                            /** */
                        }
                        else
                        {
                            // Si no, un rectángulo negro que coincida con el fondo
                            al_draw_filled_rectangle(xCoordenada + GROSOR_BORDE, yCoordenada + GROSOR_BORDE, xCoordenada + MEDIDA_CUADRO + GROSOR_BORDE, yCoordenada + MEDIDA_CUADRO + GROSOR_BORDE, encendido == 0 ? negro : rojo);
                        }

                        xCoordenada += MEDIDA_CUADRO;
                    }
                }
                xCoordenada = 0;
                yCoordenada += MEDIDA_CUADRO;
            }
            /*
                    =========================================
                    Termina dibujo de la cuadrícula donde las piezas ya han caído
                    =========================================
            */
            /*
                    =========================================
                    Empieza dibujo de la pieza (tetrimino) en movimiento
                    =========================================
            */
            for (uint8_t indiceBit = 0; indiceBit < BITS_EN_UINT16; indiceBit++)
            {

                bool hayUnCuadroDeTetriminoEnLaCoordenadaActual = (piezaActual.cuadricula >> (MAXIMO_INDICE_BIT_EN_UINT16 - indiceBit)) & 1;
                if (hayUnCuadroDeTetriminoEnLaCoordenadaActual)
                {
                    // Llegados aquí sabemos que el "continue" no se ejecutó y que SÍ hay un tetrimino

                    // Coordenadas sobre la cuadrícula después de aplicar los modificadores
                    uint8_t xRelativoDentroDeCuadricula = indiceBit % BITS_POR_FILA_PARA_TETRIMINO;
                    uint8_t YRelativoDentroDeCuadricula = indiceBit / BITS_POR_FILA_PARA_TETRIMINO;
                    int sumaX = piezaActual.x + xRelativoDentroDeCuadricula;
                    int sumaY = piezaActual.y + YRelativoDentroDeCuadricula;
                    int8_t indicePiezaFantasma = indiceYParaFantasma(&piezaActual, otraCuadricula);
                    // Este es el fantasma
                    al_draw_rectangle((sumaX * MEDIDA_CUADRO) + GROSOR_BORDE, ((YRelativoDentroDeCuadricula + indicePiezaFantasma) * MEDIDA_CUADRO) + GROSOR_BORDE, (sumaX * MEDIDA_CUADRO) + MEDIDA_CUADRO + GROSOR_BORDE, (((YRelativoDentroDeCuadricula + indicePiezaFantasma) * MEDIDA_CUADRO) + MEDIDA_CUADRO) + GROSOR_BORDE, azul, 2);
                    // Y esta es la pieza en movimiento
                    float sx = 0;
                    float sy = 0;
                    float sw = al_get_bitmap_width(imagen_pieza_movimiento);
                    float sh = al_get_bitmap_height(imagen_pieza_movimiento);

                    float dx = (sumaX * MEDIDA_CUADRO) + GROSOR_BORDE;
                    float dy = (sumaY * MEDIDA_CUADRO) + GROSOR_BORDE;
                    float dw = MEDIDA_CUADRO;
                    float dh = MEDIDA_CUADRO;

                    al_draw_scaled_bitmap(imagen_pieza_movimiento,
                                          sx, sy, sw, sh,
                                          dx, dy, dw, dh, 0);
                }
            }

            /*
                    =========================================
                    Termina dibujo de la pieza (tetrimino) en movimiento
                    =========================================
            */

            /*
                    =========================================
                    Empieza dibujo de la pieza (tetrimino) siguiente
                    =========================================
            */
            al_draw_textf(
                fuente,
                blanco,
                (ANCHO_CUADRICULA * BITS_EN_UN_BYTE * MEDIDA_CUADRO) + GROSOR_BORDE * 2,
                GROSOR_BORDE,
                ALLEGRO_ALIGN_LEFT,
                "Siguiente");
            for (uint8_t indiceBit = 0; indiceBit < BITS_EN_UINT16; indiceBit++)
            {
                bool hayUnCuadroDeTetriminoEnLaCoordenadaActual = (piezaSiguiente.cuadricula >> (MAXIMO_INDICE_BIT_EN_UINT16 - indiceBit)) & 1;
                if (hayUnCuadroDeTetriminoEnLaCoordenadaActual)
                {
                    // Llegados aquí sabemos que el "continue" no se ejecutó y que SÍ hay un tetrimino

                    // Coordenadas sobre la cuadrícula después de aplicar los modificadores
                    uint8_t xRelativoDentroDeCuadricula = indiceBit % BITS_POR_FILA_PARA_TETRIMINO;
                    uint8_t YRelativoDentroDeCuadricula = indiceBit / BITS_POR_FILA_PARA_TETRIMINO;
                    int sumaX = 50 + (ANCHO_CUADRICULA * BITS_EN_UN_BYTE * MEDIDA_CUADRO) + GROSOR_BORDE * 2 + xRelativoDentroDeCuadricula * MEDIDA_CUADRO;
                    int sumaY = 50 + GROSOR_BORDE + YRelativoDentroDeCuadricula * MEDIDA_CUADRO;
                    float sx = 0;
                    float sy = 0;
                    float sw = al_get_bitmap_width(imagen_pieza_movimiento);
                    float sh = al_get_bitmap_height(imagen_pieza_movimiento);

                    float dx = (sumaX * MEDIDA_CUADRO) + GROSOR_BORDE;
                    float dy = (sumaY * MEDIDA_CUADRO) + GROSOR_BORDE;
                    float dw = MEDIDA_CUADRO;
                    float dh = MEDIDA_CUADRO;

                    al_draw_scaled_bitmap(imagen_pieza_movimiento,
                                          sx, sy, sw, sh,
                                          sumaX, sumaY, dw, dh, 0);
                }
            }

            // Borde separador de puntaje y área de juego
            al_draw_rectangle(
                GROSOR_BORDE / 2,
                GROSOR_BORDE / 2,
                (ANCHO_CUADRICULA * BITS_EN_UN_BYTE * MEDIDA_CUADRO) + GROSOR_BORDE * 1.5,
                (ALTO_CUADRICULA * MEDIDA_CUADRO) + GROSOR_BORDE * 1.5,
                color_borde, GROSOR_BORDE);
            // Y ahora el puntaje
            al_draw_textf(
                fuente,
                blanco,
                (ANCHO_CUADRICULA * BITS_EN_UN_BYTE * MEDIDA_CUADRO) + GROSOR_BORDE * 2,
                ALTO_CUADRICULA * MEDIDA_CUADRO / 2,
                ALLEGRO_ALIGN_LEFT,
                "Puntaje: %lu",
                puntajeGlobal);

            al_flip_display();

            redraw = false;
        }
    }

    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_bitmap(imagen_pieza_caida);
    al_destroy_bitmap(imagen_pieza_movimiento);

    return 0;
}