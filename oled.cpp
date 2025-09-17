#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <stdbool.h>
#define OFFSET_Y 4
#define MAXIMO_INDICE_BIT_EN_UINT16 15
#define BITS_EN_UINT16 16
#define ANCHO_OLED 128
#define BITS_POR_FILA_PARA_TETRIMINO 4
#define ALTO_OLED 64
#define MEDIDA_CUADRO 4
#define ANCHO_CUADRICULA (ANCHO_OLED / 8 / MEDIDA_CUADRO)
#define ALTO_CUADRICULA (ALTO_OLED / MEDIDA_CUADRO) - OFFSET_Y
#define BITS_EN_UN_BYTE 8
#define MAXIMO_INDICE_BIT_EN_BYTE 7
#define CUADRICULA_TETRIMINO 4
#define TOTAL_TETRIMINOS_DISPONIBLES 5
Adafruit_SSD1306 display(ANCHO_OLED, ALTO_OLED, &Wire, -1);
struct Tetrimino
{
  uint16_t cuadricula;
  int x, y;
};
struct TetriminoParaElegir
{
  // medio byte por línea
  uint16_t cuadricula;
};
struct TetriminoParaElegir todasLasPiezasParaElegir[TOTAL_TETRIMINOS_DISPONIBLES];
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
void elegirPiezaAleatoria(struct Tetrimino *destino, struct TetriminoParaElegir *piezas)
{
  uint8_t indiceAleatorio = random(0, TOTAL_TETRIMINOS_DISPONIBLES);
  destino->cuadricula = piezas[indiceAleatorio].cuadricula;
  destino->x = 0;
  destino->y = 0;
}
void inicializarPiezas(struct TetriminoParaElegir *piezas)
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
}
/**
 * El alto en 40 ya no sirve, lo he dejado en 32 pero no sé si es coincidencia. Deberíamos
 * probar con 33 o similares
 *
 * sí, parece que 33 es el límite
 */

#if (MEDIDA_CUADRO == 2)
uint8_t otraCuadricula[ALTO_CUADRICULA][ANCHO_CUADRICULA] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
};
#elif (MEDIDA_CUADRO == 4)
uint8_t otraCuadricula[ALTO_CUADRICULA][ANCHO_CUADRICULA] = {
    {
        0,
        0,
        0,
        0,
    },
    {
        0,
        0,
        0,
        0,
    },
    {
        0,
        0,
        0,
        0,
    },
    {
        0,
        0,
        0,
        0,
    },
    {
        0,
        0,
        0,
        0,
    },
    {
        0,
        0,
        0,
        0,
    },
    {
        0,
        0,
        0,
        0,
    },
    {
        0,
        0,
        0,
        0,
    },
    {
        0,
        0,
        0,
        0,
    },
    {
        0,
        0,
        0,
        0,
    },
    {
        0,
        0,
        0,
        0,
    },
    {
        0,
        0,
        0,
        0,
    },
};
#else
uint8_t otraCuadricula[ALTO_CUADRICULA][ANCHO_CUADRICULA] = {
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},

};
#endif
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

void bajarTetrimino(struct Tetrimino *tetrimino, uint8_t cuadricula[ALTO_CUADRICULA][ANCHO_CUADRICULA], bool *bandera, unsigned long *puntajeGlobal)
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
      elegirPiezaAleatoria(tetrimino, todasLasPiezasParaElegir);
    }
    else
    {
      // No puedes bajar pero te doy un tiempo para que te puedas mover
      *bandera = true;
    }
  }
}

const int pinX = 0,
          pinY = 1;
struct Tetrimino tetrimino;
bool banderaTocoSuelo = false;
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
unsigned long ultimosMilisegundos = 0;
// TODO: tal vez no deba ser constante porque debemos reducirlo entre mayor dificultad
const long intervaloAvanzarPiezaEnMs = 500;
unsigned long puntaje = 0;

bool haPresionadoElBotonPreviamente = false;

void setup()
{
  puntaje = 0;
  randomSeed(analogRead(3));
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  inicializarPiezas(todasLasPiezasParaElegir);
  elegirPiezaAleatoria(&tetrimino, todasLasPiezasParaElegir);
}

void loop()
{

   display.clearDisplay();
  int valorX = analogRead(pinX),
      valorY = analogRead(pinY);

  bool presionado = !digitalRead(5);
  unsigned long milisegundosActuales = millis();
  display.fillRect(0, 0, ANCHO_OLED, OFFSET_Y * MEDIDA_CUADRO,SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK);
 
  display.setCursor((ANCHO_OLED / 2) - (6 * String(puntaje).length() / 2),1);
  display.setTextSize(1);
  display.print(puntaje);
  if (milisegundosActuales - ultimosMilisegundos >= intervaloAvanzarPiezaEnMs)
  {
    bajarTetrimino(&tetrimino, otraCuadricula, &banderaTocoSuelo, &puntaje);
    ultimosMilisegundos = milisegundosActuales;
  }
  // Derecha
  if (!digitalRead(4))
  {

    if (!tetriminoColisionaConCuadriculaAlAvanzar(&tetrimino, otraCuadricula, 1, 0))
    {
      tetrimino.x++;
    }
  }
  // Izquierda
  if (!digitalRead(2))
  {

    if (!tetriminoColisionaConCuadriculaAlAvanzar(&tetrimino, otraCuadricula, -1, 0))
    {
      tetrimino.x--;
    }
  }
  // Abajo
  if (!digitalRead(3))
  {
    bajarTetrimino(&tetrimino, otraCuadricula, &banderaTocoSuelo, &puntaje);
  }
  if (presionado)
  {
    haPresionadoElBotonPreviamente = true;
  }
  else
  {
    if (haPresionadoElBotonPreviamente)
    {
      haPresionadoElBotonPreviamente = false;
      if (!tetriminoColisionaConCuadriculaAlRotar(&tetrimino, otraCuadricula))
      {
        tetrimino.cuadricula = rotar90CW(tetrimino.cuadricula);
      }
    }
  }

  // Dibujamos toda la cuadrícula...

  for (int y = 0; y < ALTO_CUADRICULA; y++)
  {
    for (int x = 0; x < ANCHO_CUADRICULA; x++)
    {
      for (int i = 0; i < BITS_EN_UN_BYTE; i++)
      {
        int encendido = (otraCuadricula[y][x] >> (MAXIMO_INDICE_BIT_EN_BYTE - i)) & 1;
        int verdaderoX = (x * BITS_EN_UN_BYTE) + i;
        if (encendido)
        {
          display.fillRect(verdaderoX * MEDIDA_CUADRO, (y + OFFSET_Y) * MEDIDA_CUADRO,
                           MEDIDA_CUADRO, MEDIDA_CUADRO, SSD1306_WHITE);
        }
        else
        {

          display.fillRect(verdaderoX * MEDIDA_CUADRO, (y + OFFSET_Y) * MEDIDA_CUADRO,
                           MEDIDA_CUADRO, MEDIDA_CUADRO, SSD1306_BLACK);
        }
      }
    }
  }
  // Y ahora el tetrimino

  // Comienza nuevo código...
  for (uint8_t indiceBit = 0; indiceBit < BITS_EN_UINT16; indiceBit++)
  {

    bool hayUnCuadroDeTetriminoEnLaCoordenadaActual = (tetrimino.cuadricula >> (MAXIMO_INDICE_BIT_EN_UINT16 - indiceBit)) & 1;
    if (hayUnCuadroDeTetriminoEnLaCoordenadaActual)
    {
      // Llegados aquí sabemos que el "continue" no se ejecutó y que SÍ hay un tetrimino

      // Coordenadas sobre la cuadrícula después de aplicar los modificadores
      uint8_t xRelativoDentroDeCuadricula = indiceBit % BITS_POR_FILA_PARA_TETRIMINO;
      uint8_t YRelativoDentroDeCuadricula = indiceBit / BITS_POR_FILA_PARA_TETRIMINO;
      int sumaX = tetrimino.x + xRelativoDentroDeCuadricula;
      int sumaY = tetrimino.y + YRelativoDentroDeCuadricula;
      display.fillRect(sumaX * MEDIDA_CUADRO, (sumaY + OFFSET_Y) * MEDIDA_CUADRO,
                       MEDIDA_CUADRO, MEDIDA_CUADRO, SSD1306_WHITE);
    }
  }

  display.drawRect(0, OFFSET_Y * MEDIDA_CUADRO, ANCHO_OLED, (ALTO_OLED - (OFFSET_Y * MEDIDA_CUADRO)), SSD1306_WHITE);

  display.display();
}