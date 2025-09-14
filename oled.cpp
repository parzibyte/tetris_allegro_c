
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <stdbool.h>
#define ANCHO_OLED 128
#define ALTO_OLED 64
#define CANTIDAD_BYTES_TETRIMINO 2
#define MEDIDA_CUADRO 4
#define ANCHO_CUADRICULA (ANCHO_OLED / 8 / MEDIDA_CUADRO)
#define ALTO_CUADRICULA (ALTO_OLED / MEDIDA_CUADRO)
#define BITS_EN_UN_BYTE 8
#define MAXIMO_INDICE_BIT_EN_BYTE 7
#define CUADRICULA_TETRIMINO 4

Adafruit_SSD1306 display(ANCHO_OLED, ALTO_OLED, &Wire, -1);
struct Tetrimino
{
  // medio byte por línea
  uint8_t cuadricula[CANTIDAD_BYTES_TETRIMINO];
  int x, y;
};
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

void bajarTetrimino(struct Tetrimino *tetrimino, uint8_t cuadricula[ALTO_CUADRICULA][ANCHO_CUADRICULA], bool *bandera)
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
      // Aquí después copiamos los datos y todo eso. Por ahora solo volvemos a subir la pieza XD
      // Ok entonces recorremos de igual manera el tetrimino
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
            int xEnCuadriculaDespuesDeModificar = tetrimino->x + (indiceBitTetrimino % 4);
            int yEnCuadriculaDespuesDeModificar = tetrimino->y + (indiceChar * CANTIDAD_BYTES_TETRIMINO) + (indiceBitTetrimino / 4);
            int xEnByteDeCuadricula = xEnCuadriculaDespuesDeModificar / BITS_EN_UN_BYTE;
            int indiceBitDeByteEnCuadricula = xEnCuadriculaDespuesDeModificar % BITS_EN_UN_BYTE;
            cuadricula[yEnCuadriculaDespuesDeModificar][xEnByteDeCuadricula] = cuadricula[yEnCuadriculaDespuesDeModificar][xEnByteDeCuadricula] | (1 << (MAXIMO_INDICE_BIT_EN_BYTE - indiceBitDeByteEnCuadricula));
          }
        }
      }
      tetrimino->y = 0;
      tetrimino->x = 0;
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
struct Tetrimino linea;
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

void setup()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  pinMode(2, INPUT_PULLUP);
  linea.cuadricula[0] = 228;
  linea.cuadricula[1] = 0;
  linea.x = 0;
  linea.y = 0;
}

void loop()
{

  display.clearDisplay();
  int valorX = analogRead(pinX),
      valorY = analogRead(pinY);

  bool presionado = digitalRead(2);
  unsigned long milisegundosActuales = millis();
  if (milisegundosActuales - ultimosMilisegundos >= intervaloAvanzarPiezaEnMs)
  {
    bajarTetrimino(&linea, otraCuadricula, &banderaTocoSuelo);
    ultimosMilisegundos = milisegundosActuales;
  }
  // Derecha
  if (valorX > 600)
  {

    if (!tetriminoColisionaConCuadriculaAlAvanzar(&linea, otraCuadricula, 1, 0))
    {
      linea.x++;
    }
  }
  // Izquierda
  if (valorX < 400)
  {

    if (!tetriminoColisionaConCuadriculaAlAvanzar(&linea, otraCuadricula, -1, 0))
    {
      linea.x--;
    }
  }
  // Abajo
  if (valorY < 400 || !presionado)
  {
    bajarTetrimino(&linea, otraCuadricula, &banderaTocoSuelo);
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
          display.fillRect(verdaderoX * MEDIDA_CUADRO, y * MEDIDA_CUADRO,
                           MEDIDA_CUADRO, MEDIDA_CUADRO, SSD1306_WHITE);
        }
        else
        {

          display.fillRect(verdaderoX * MEDIDA_CUADRO, y * MEDIDA_CUADRO,
                           MEDIDA_CUADRO, MEDIDA_CUADRO, SSD1306_BLACK);
        }
      }
    }
  }
  // Y ahora el tetrimino
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

        display.fillRect(sumaX * MEDIDA_CUADRO, sumaY * MEDIDA_CUADRO,
                         MEDIDA_CUADRO, MEDIDA_CUADRO, SSD1306_WHITE);
      }
    }
  }
  display.display();
}
