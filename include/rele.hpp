#pragma once
#include <cstdint>
#include <string>

class w_rele
{
private:
    const uint8_t START, MONEDERO , RELE , PRECIO, PARO_EME;
    uint8_t dinero = 0, pulso = 0;
public:
    w_rele(/* args */);
    ~w_rele();
    void init();
};