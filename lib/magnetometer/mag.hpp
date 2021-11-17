#include <Wire.h>

// Classe simples para tratar a bússola
class Bussola 
{
  public:
    Bussola(void);
    bool inicia(void);
    void setDeclination (int graus , int mins, char dir);
    int leDirecao(void);
    void iniciaCal();
    void encerraCal();

  private:
    static const int ender_QMC = 0x0D; // endereço I2C do QMC5883
    static const int regCR1_QMC = 9;   // registrador de configuração
    static const int regSR_QMC = 11;   // registador set/reset
    static const int regXL_QMC = 0;    // primeiro registrador de dados
    static const int regST_QMC = 6;    // registrador de status
    // fatores de correção determinados na calibração
    int16_t xMin, yMin, xMax, yMax;
    float escX = 1.0;
    float escY = 1.0;
    int16_t offX = 0;
    int16_t offY = 0;
    // Diferença entre o Polo Magnético e o Geográfico
    float declination = 0.0;
    // Rotina para disparar leitura dos dados
    void pedeDados(int regStatus, int regDados);
};