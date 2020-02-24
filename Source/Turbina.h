#define ALEJA

#ifdef ALEJA
#define TURBINA_STOP   50   //50 Aleja  75 RACH
#define TURBINA_MAX    90   //90 Aleja  50 RACH

#else

#define TURBINA_STOP   75   //50 Aleja  75 RACH
#define TURBINA_MAX    50   //90 Aleja  50 RACH
#endif


void Turbina_Init (void);
void Turbina_Control (int Velocidad);
