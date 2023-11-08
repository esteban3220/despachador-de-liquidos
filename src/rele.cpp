#include "rele.hpp"
#include "hardware/gpio.h"
#include "lcd.hpp"
#include <chrono>
#include <iostream>
#include <vector>
#include <thread>
#include "pico/stdlib.h"
#include "pico/time.h"


w_rele::w_rele(/* args */) : START(10), MONEDERO(15), RELE(12), PRECIO(4), PARO_EME(14)
{

    stdio_init_all();
    i2c_init(i2c_default, 100 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    LCD::lcd_init();

    gpio_init(RELE);
    gpio_init(START);
    gpio_init(MONEDERO);
    gpio_init(PARO_EME);

    gpio_set_dir(RELE, GPIO_OUT);
    gpio_set_dir(START, GPIO_IN);
    gpio_set_dir(MONEDERO, GPIO_IN);
    gpio_set_dir(PARO_EME, GPIO_IN);

    gpio_pull_up(MONEDERO);
    gpio_pull_up(START);
    gpio_pull_up(PARO_EME);

    gpio_put(RELE, 0);
}

w_rele::~w_rele()
{
}

void w_rele::init() {


    LCD::lcd_set_cursor(0, 0);
    LCD::lcd_string("Ingrese Dinero");
    LCD::lcd_set_cursor(1, 0);
    std::string reini = "$" + std::to_string(PRECIO) + " LT. de Cloro";
    LCD::lcd_string(reini.c_str());

    while (true)
    {
        // 2 pulso = 1 peso
        // 3 pulsos = 2 pesos
        // 6 pulsos = 5 pesos
        // 11 pulsos = 10 pesos
        //MODO  FAST
        if (!gpio_get(MONEDERO)) {
            pulso++;
            auto start_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::milliseconds(1200);

            while (std::chrono::high_resolution_clock::now() - start_time < duration)
            {
                if (!gpio_get(MONEDERO)) {
                    pulso++;
                    sleep_ms(100);
                }
            }

            switch (pulso)
            {
            case 2:
            {
                dinero += 1;
                pulso = 0;
                std::string total = "Total: $ " + std::to_string(static_cast<int>(dinero));
                LCD::lcd_clear();
                LCD::lcd_set_cursor(0, 0);
                LCD::lcd_string(total.c_str());
                LCD::lcd_set_cursor(1, 0);
                std::string Litros = "Litros: " + std::to_string((float) static_cast<int>(dinero) / PRECIO);
                LCD::lcd_string(Litros.c_str());
                break;
            }
            case 3:
            {
                dinero += 2;
                pulso = 0;
                std::string total = "Total: $ " + std::to_string(static_cast<int>(dinero));
                LCD::lcd_clear();
                LCD::lcd_set_cursor(0, 0);
                LCD::lcd_string(total.c_str());
                LCD::lcd_set_cursor(1, 0);
                std::string Litros = "Litros: " + std::to_string((float) static_cast<int>(dinero) / PRECIO);
                LCD::lcd_string(Litros.c_str());
                break;
            }
            case 6:
            {
                dinero += 5;
                pulso = 0;
                std::string total = "Total: $ " + std::to_string(static_cast<int>(dinero));
                LCD::lcd_clear();
                LCD::lcd_set_cursor(0, 0);
                LCD::lcd_string(total.c_str());
                LCD::lcd_set_cursor(1, 0);
                std::string Litros = "Litros: " + std::to_string((float) static_cast<int>(dinero) / PRECIO);
                LCD::lcd_string(Litros.c_str());
                break;
            }
            case 11:
            {
                dinero += 10;
                pulso = 0;
                std::string total = "Total: $ " + std::to_string(static_cast<int>(dinero));
                LCD::lcd_clear();
                LCD::lcd_set_cursor(0, 0);
                LCD::lcd_string(total.c_str());
                LCD::lcd_set_cursor(1, 0);
                std::string Litros = "Litros: " + std::to_string((float) static_cast<int>(dinero) / PRECIO);
                LCD::lcd_string(Litros.c_str());
                break;
            }
            default:
            {
                std::system("clear");
                std::cout << "Pulsos " << static_cast<int>(pulso) << std::endl;
                std::cout << "Total " << static_cast<int>(dinero) << " $" << std::endl;
                std::string total = "Total: $ " + std::to_string(static_cast<int>(dinero));
                LCD::lcd_clear();
                LCD::lcd_set_cursor(0, 0);
                LCD::lcd_string(total.c_str());
                LCD::lcd_set_cursor(1, 0);
                std::string total2 = "Pulsos: " + std::to_string(static_cast<int>(pulso));
                LCD::lcd_string(total2.c_str());
                pulso = 0;
                break;
            }
            }
        }



        if (!gpio_get(START) && dinero >= 1) {

            std::string total = "Total: " + std::to_string((float) static_cast<int>(dinero) / PRECIO) + " L";
            LCD::lcd_clear();
            LCD::lcd_string(total.c_str());
            gpio_put(RELE, 1);

            /// 35.5 segundos por litro
            auto start_time = std::chrono::high_resolution_clock::now();
            size_t dur_mil = static_cast<size_t>((float) static_cast<int>(dinero) / PRECIO * 68'130);
            auto duration = std::chrono::milliseconds(dur_mil);

            while (std::chrono::high_resolution_clock::now() - start_time < duration)
            {
                sleep_ms(10);
                if (!gpio_get(PARO_EME)) {
                    LCD::lcd_clear();
                    LCD::lcd_set_cursor(0, 0);
                    LCD::lcd_string("Paro de");
                    LCD::lcd_set_cursor(1, 0);
                    LCD::lcd_string("Emergencia");
                    break;
                }
            }


            gpio_put(RELE, 0);
            LCD::lcd_clear();
            LCD::lcd_set_cursor(0, 0);
            LCD::lcd_string("Llenado finalizado");
            dinero = 0;
            sleep_ms(1000);
            std::system("clear");
            LCD::lcd_clear();
            LCD::lcd_set_cursor(0, 0);
            LCD::lcd_string("Ingrese Dinero");
            LCD::lcd_set_cursor(1, 0);
            std::string reini = "$" + std::to_string(PRECIO) + " LT. de Cloro";
            LCD::lcd_string(reini.c_str());
        }
    }
}
