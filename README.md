# 🎵 Teclado Musical com Raspberry Pi Pico W  

Este projeto implementa um **servidor web** no **Raspberry Pi Pico W**, permitindo que os usuários interajam com um **teclado virtual** no navegador. O buzzer reproduz os sons das notas musicais, e o display OLED SSD1306 exibe a nota tocada.  

## 📌 Funcionalidades  

- ✅ Conexão Wi-Fi e servidor HTTP  
- ✅ Interface Web com teclado interativo  
- ✅ Reprodução de notas musicais via buzzer  
- ✅ Exibição das notas no display OLED  

## 🛠️ Dependências  

### 🔹 Hardware  

- **Raspberry Pi Pico W**  
- **Buzzer Piezoelétrico**  
- **Display OLED SSD1306 (I2C)**  
- **Jumpers e resistores para conexões**  

### 🔹 Bibliotecas  

- **Pico SDK**  
- **LWIP** (para rede e servidor HTTP)  
- **SSD1306** (para comunicação com o display)  

## 🔌 Conexões  

| Componente  | Pino Pico W |
|-------------|------------|
| Buzzer      | GP21       |
| OLED SDA    | GP14       |
| OLED SCL    | GP15       |

## 🚀 Configuração e Execução  

### 🔧 1️⃣ Configurar o Ambiente  

Instale o **Pico SDK** seguindo a documentação oficial:  
🔗 [Pico SDK Guide](https://github.com/raspberrypi/pico-sdk)  

### 📂 2️⃣ Compilar o Projeto  

Clone este repositório:  

```sh
git clone https://github.com/andradejs/teclado_musical_raspberry_pi_pico
cd teclado-musical-pico
