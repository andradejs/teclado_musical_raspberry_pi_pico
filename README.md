🎵 Teclado Musical com Raspberry Pi Pico W

Este projeto implementa um servidor web no Raspberry Pi Pico W, permitindo que os usuários interajam com um teclado virtual no navegador. O buzzer reproduz os sons das notas musicais, e o display OLED SSD1306 exibe a nota tocada.
📌 Funcionalidades

✅ Conexão Wi-Fi e servidor HTTP
✅ Interface Web com teclado interativo
✅ Reprodução de notas musicais via buzzer
✅ Exibição das notas no display OLED
🛠️ Dependências
🔹 Hardware

    Raspberry Pi Pico W
    Buzzer Piezoelétrico
    Display OLED SSD1306 (I2C)
    Jumpers e resistores para conexões

🔹 Bibliotecas

    Pico SDK
    LWIP (para rede e servidor HTTP)
    SSD1306 (para comunicação com o display)

🔌 Conexões
Componente	Pino Pico W
Buzzer	GP21
LED	GP12
OLED SDA	GP14
OLED SCL	GP15
🚀 Configuração e Execução
🔧 1️⃣ Configurar o Ambiente

Instale o Pico SDK seguindo a documentação oficial:
🔗 Pico SDK Guide
📂 2️⃣ Compilar o Projeto

    Clone este repositório:

git clone https://github.com/seu-usuario/teclado-musical-pico.git
cd teclado-musical-pico

Compile o código:

    mkdir build && cd build
    cmake ..
    make

    Carregue o firmware no Raspberry Pi Pico.

🌐 3️⃣ Acessar o Teclado pelo Navegador

    Ligue o Pico W e aguarde a conexão Wi-Fi.
    O endereço IP será exibido no display OLED.
    No navegador, acesse:

    http://<endereco-ip>

    Clique nas teclas para tocar as notas! 🎶

📜 Estrutura do Projeto

📁 teclado-musical-pico
├── 📂 inc                  # Arquivos de cabeçalho
│   ├── ssd1306.h           # Controle do display OLED
│   ├── ssd1306_font.h      # Fontes para o display
│   ├── ssd1306_i2c.c       # Comunicação I2C com o SSD1306
│   ├── ssd1306_i2c.h       # Cabeçalho da interface I2C
├── CMakeLists.txt          # Configuração do CMake
├── pico_sdk_import.cmake   # Importação do Pico SDK
├── lwipopts.h              # Configuração do LWIP (rede)
├── texto_http.c            # Código do servidor web e interação

📜 Licença

Este projeto é open-source e pode ser utilizado livremente para fins educacionais e experimentação
