#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/binary_info.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"


#define LED_PIN 12          // Define o pino do LED
#define BUZZER_PIN 21       //define buzzer.

#define WIFI_SSID "Online Rose"  // Substitua pelo nome da sua rede Wi-Fi
#define WIFI_PASS "52074185" // Substitua pela senha da sua rede Wi-Fi


const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

// Buffer para respostas HTTP
#define HTTP_RESPONSE "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" \
"<!DOCTYPE html><html><head>" \
"<title>Teclado Musical</title>" \
"<style>" \
"body { font-family: Arial, sans-serif; margin: 0; padding: 0; box-sizing: border-box; }" \
".tecla { display: inline-block; width: 60px; height: 180px; margin: 2px; text-align: center; line-height: 180px; font-weight: bold; border: 2px solid black; border-radius: 5px; cursor: pointer; transition: 0.2s; }" \
".branca { background-color: white; }" \
".preta { background-color: black; color: white; width: 40px; height: 120px; position: relative; margin-left: -20px; margin-right: -20px; z-index: 1; }" \
".container { display: flex; justify-content: center; align-items: flex-end; padding: 10px; flex-wrap: wrap; }" \
"@media (max-width: 768px) { .tecla { width: 40px; height: 120px; line-height: 120px; font-size: 12px; } .preta { width: 30px; height: 90px; margin-left: -15px; margin-right: -15px; } }" \
"</style></head><body>" \
"<h1 style=\"text-align:center;\">Teclado Musical</h1>" \
"<div class=\"container\">" \
"<p><a href=\"/nota/0\" class=\"tecla branca\">Do</a></p>" \
"<p><a href=\"/nota/1\" class=\"tecla preta\">Do#</a></p>" \
"<p><a href=\"/nota/2\" class=\"tecla branca\">Re</a></p>" \
"<p><a href=\"/nota/3\" class=\"tecla preta\">Re#</a></p>" \
"<p><a href=\"/nota/4\" class=\"tecla branca\">Mi</a></p>" \
"<p><a href=\"/nota/5\" class=\"tecla branca\">Fa</a></p>" \
"<p><a href=\"/nota/6\" class=\"tecla preta\">Fa#</a></p>" \
"<p><a href=\"/nota/7\" class=\"tecla branca\">Sol</a></p>" \
"<p><a href=\"/nota/8\" class=\"tecla preta\">Sol#</a></p>" \
"<p><a href=\"/nota/9\" class=\"tecla branca\">La</a></p>" \
"<p><a href=\"/nota/10\" class=\"tecla preta\">La#</a></p>" \
"<p><a href=\"/nota/11\" class=\"tecla branca\">Si</a></p>" \
"</div></body></html>\r\n"

#define TOTAL_NOTAS 13

typedef struct 
{
    char nome[10];
    float frequencia;
} Nota;


Nota notas[13]= {
    {"do", 261.63},  // Dó
    {"do#", 277.18}, // Dó sustenido
    {"re", 293.66},  // Ré
    {"re#", 311.13}, // Ré sustenido
    {"mi", 329.63},  // Mi
    {"fa", 349.23},  // Fá
    {"fa#", 369.99}, // Fá sustenido
    {"sol", 392.00},  // Sol
    {"sol#", 415.30}, // Sol sustenido
    {"la", 440.00},  // Lá (padrão 440Hz)
    {"la#", 466.16}, // Lá sustenido
    {"si", 493.88},  // Si
    {"do", 523.25},  // Dó (oitava superior)
};




struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
      };
    

void display_text(char *lines[], int line_count);

void clear_display(uint8_t *ssd);

void showNote(char note[4]);

char *capturarNota(const char *request) {
    const char *pos = strstr(request, "/nota/");
    if (pos) {
        // Avança para a parte depois de "/nota/"
        pos += 6;  
        
        // Captura a nota (incluindo sustenido)
        static char nota[10];
        sscanf(pos, "%9s", nota);  
        return nota;
    }
    return NULL;
}

float getFrequencia(const char *nome) {
    for (int i = 0; i < TOTAL_NOTAS; i++) {
        if (strcmp(notas[i].nome, nome) == 0) {
            return notas[i].frequencia;
        }
    }
    return -1.0; // Retorna -1 se não encontrar
}

void set_buzzer_frequency(uint pin, float frequency) {
    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint32_t clock = clock_get_hz(clk_sys); // Clock do sistema (normalmente 125 MHz)
    float divider = (float)clock / (frequency * 4096);
    
    pwm_set_clkdiv(slice_num, divider);  // Ajusta o divisor do clock
    pwm_set_wrap(slice_num, 4095);       // Define a contagem máxima do PWM
    pwm_set_gpio_level(pin, 2048);       // Define um duty cycle de 50%
}

void pwm_init_buzzer(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);

    pwm_config config = pwm_get_default_config();
    pwm_init(slice_num, &config, true);
}

void play_note(uint pin, float frequency, uint duration_ms) {
    set_buzzer_frequency(pin, frequency);
    sleep_ms(duration_ms);
    pwm_set_gpio_level(pin, 0);  // Para o som
}

// Função de callback para processar requisições HTTP
static err_t http_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p == NULL) {
        // Cliente fechou a conexão
        tcp_close(tpcb);
        return ERR_OK;
    }

    // Processa a requisição HTTP
    char *request = (char *)p->payload;

    char *nota = capturarNota(request);
    showNote(notas[atoi(nota)].nome);
    printf(notas[atoi(nota)].nome);
    play_note(BUZZER_PIN,notas[atoi(nota)].frequencia,500);

    // if (strstr(request, "GET /nota/do")) {
    //     play_note(BUZZER_PIN, 261.63, 500);
    //     showNote("Do");
        

    // } else if (strstr(request, "GET /nota/re")) {
    //     play_note(BUZZER_PIN, 294, 500);
    // }

    // Envia a resposta HTTP
    tcp_write(tpcb, HTTP_RESPONSE, strlen(HTTP_RESPONSE), TCP_WRITE_FLAG_COPY);

    // Libera o buffer recebido
    pbuf_free(p);

    return ERR_OK;
}

// Callback de conexão: associa o http_callback à conexão
static err_t connection_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, http_callback);  // Associa o callback HTTP
    return ERR_OK;
}

// Função de setup do servidor TCP
static void start_http_server(void) {
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) {
        printf("Erro ao criar PCB\n");
        return;
    }

    // Liga o servidor na porta 80
    if (tcp_bind(pcb, IP_ADDR_ANY, 80) != ERR_OK) {
        printf("Erro ao ligar o servidor na porta 80\n");
        return;
    }

    pcb = tcp_listen(pcb);  // Coloca o PCB em modo de escuta
    tcp_accept(pcb, connection_callback);  // Associa o callback de conexão

    printf("Servidor HTTP rodando na porta 80...\n");
}

void init_display() {
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init();
    calculate_render_area_buffer_length(&frame_area);
}

void clear_display(uint8_t *ssd) {
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);
}

void display_text(char *lines[], int line_count) {
    uint8_t ssd[ssd1306_buffer_length];
    clear_display(ssd);

    int y = 0;
    for (int i = 0; i < line_count; i++) {
        ssd1306_draw_string(ssd, 5, y, lines[i]);
        y += 8;
    }
    render_on_display(ssd, &frame_area);
}

void showNote(char nota[4]){
    char *text[] = {
        "            ",
        "            ",
        "", nota, "",
        "            ",
        "            "
    };
    
    display_text(text, 5);
}

int main() {
    stdio_init_all();  // Inicializa a saída padrão
    sleep_ms(10000);
    printf("Iniciando servidor HTTP\n");

    uint8_t *ip_adress_rasp = malloc(4); 
    // Inicializa o Wi-Fi
    if (cyw43_arch_init()) {
        printf("Erro ao inicializar o Wi-Fi\n");
        return 1;
    }

    cyw43_arch_enable_sta_mode();
    printf("Conectando ao Wi-Fi...\n");

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        printf("Falha ao conectar ao Wi-Fi\n");
        return 1;
    }else {
        printf("Connected.\n");
        // Read the ip address in a human readable way
        uint8_t *ip_address = (uint8_t*)&(cyw43_state.netif[0].ip_addr.addr);
        memcpy(ip_adress_rasp, ip_address, 4);
        printf("Endereço IP %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    }

    printf("Wi-Fi conectado!\n");
    printf("Para ligar ou desligar o LED acesse o Endereço IP seguido de /led/on ou /led/off\n");

    pwm_init_buzzer(BUZZER_PIN);


    // Configura o LED como saída
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Inicia o servidor HTTP
    start_http_server();

    init_display();
    char ip_str[16];
    sprintf(ip_str, "%d.%d.%d.%d", ip_adress_rasp[0], ip_adress_rasp[1], ip_adress_rasp[2], ip_adress_rasp[3]);

    char *text[] = {
        "            ",
        "            ",
        "    digite   ",
        ip_str,
        "     no     ",
        "  navegador  "
    };
    
    display_text(text, 6);
    
    // Loop principal
    while (true) {
        cyw43_arch_poll();  // Necessário para manter o Wi-Fi ativo
        sleep_ms(100);
    }

    cyw43_arch_deinit();  // Desliga o Wi-Fi (não será chamado, pois o loop é infinito)
    return 0;
}