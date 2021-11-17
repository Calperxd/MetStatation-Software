#include "mag.hpp"
#include "Arduino.h"

    // Construtor
    Magnetometer::Magnetometer(void) 
    {
    }

    // Inicia comunicação com a bússola
    bool Magnetometer::inicia() 
    {
        // Confere o endereço
        Wire.beginTransmission(ender_QMC);
        if (Wire.endTransmission() != 0) 
        {
            return false;
        }
        // Inicia o chip para modo contínuo
        Wire.beginTransmission(ender_QMC);
        Wire.write(regSR_QMC);
        Wire.write(0x01);
        Wire.endTransmission();
        Wire.beginTransmission(ender_QMC);
        Wire.write(regCR1_QMC);
        Wire.write(0x0D);
        Wire.endTransmission();
        return true;
    }

    // Define a declinação (correção entre o Norte magnético e o Norte geofráfico)
    // ver http://www.magnetic-declination.com/
    void Magnetometer::setDeclination (int graus , int mins, char dir) 
    {
        declination = (graus + mins / 60.0) * PI / 180.0;
        if (dir == 'W') 
        {
            declination = - declination;
        }

        Serial.println (declination);
    }

    // Le a direção da bússola em graus (0 a 360) em relação à marcação do eixo X
    // Assume que a bússola esta na horizontal
    int Magnetometer::leDirecao(void) 
    {
        int16_t x, y, z;
        // Le a intesidade do campo magnético
        pedeDados(regST_QMC, regXL_QMC);
        x = Wire.read();          //LSB  x
        x |= Wire.read() << 8;    //MSB  x
        y = Wire.read();          //LSB y
        y |= Wire.read() << 8;    //MSB y
        z = Wire.read();          //LSB  z
        z |= Wire.read() << 8;    //MSB z
        // Registra mínimo e máximo para a calibração
        if (x < xMin) 
        {
            xMin = x;
        }
        if (xMax < x) 
        {
            xMax = x;
        }
        if (y < yMin) 
        {
            yMin = y;
        }
        if (yMax < y) 
        {
            yMax = y;
        }

        // corrige e calcula o angulo em radianos
        float xC = (x - offX) * escX;
        float yC = (y - offY) * escY;
        float angulo = atan2 (yC, xC) + declination;

        // Garante que está entre 0 e 2*PI
        if (angulo < 0) 
        {
            angulo += 2.0 * PI;
        } 
        else if (angulo >= 2 * PI) 
        {
            angulo -= 2.0 * PI;
        }
        // Converte para graus
        return round ((angulo * 180.0) / PI);
    }

    void Magnetometer::pedeDados(int regStatus, int regDados) 
    {
    // Espera ter um dado a ler
    do 
    {
        Wire.beginTransmission(ender_QMC);
        Wire.write(regStatus);
        Wire.endTransmission();
        Wire.requestFrom(ender_QMC, 1);
    } while ((Wire.read() & 1) == 0);
        Wire.beginTransmission(ender_QMC);
        Wire.write(regDados);
        Wire.endTransmission();
        Wire.requestFrom(ender_QMC, 6);
    }

    // Inicia processo de calibração
    void Magnetometer::iniciaCal() 
    {
        xMax = yMax = -32768;
        xMin = yMin = 32767;
    }

    // Encerra a calibração
    void Magnetometer::encerraCal() 
    {
        Serial.print ("X: ");
        Serial.print (xMin);
        Serial.print (" - ");
        Serial.println (xMax);
        Serial.print ("Y: ");
        Serial.print (yMin);
        Serial.print (" - ");
        Serial.println (yMax);
        offX = (xMax + xMin) / 2;
        offY = (yMax + yMin) / 2;
        int16_t varX = xMax - xMin;
        int16_t varY = yMax - yMin;
        if (varY > varX) 
        {
            escY = 1.0;
            escX = (float) varY / varX;
        } 
        else 
        {
            escX = 1.0;
            escY = (float) varX / varY;
        }
    }