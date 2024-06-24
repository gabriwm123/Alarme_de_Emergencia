//-----------------------------------------------------------------------------------------------------------------------------------------
// IOT com ESP32 e WhatsApp
//-----------------------------------------------------------------------------------------------------------------------------------------
// Área de Inclusão de arquivos.


#include <WiFi.h>        // Para acesso ao wifi
#include <HTTPClient.h>  // Para criar um cliente HTTP
#include <UrlEncode.h>   // Para enviar a mensagem via post deve ser no formato URL

//-----------------------------------------------------------------------------------------------------------------------------------------
// Protótipos de função

void WhatsAppMessages(String message);  // Função que envia uma mensagem para o whatsapp

//-----------------------------------------------------------------------------------------------------------------------------------------
// Dados da sua WiFi.

const char* SSID = "xxxxxx";      // Nome da rede wifi. Identificador da rede
const char* PASSWORD = "xxxxxx";  // Senha da rede wifi. Sua senha do Wifi

//-----------------------------------------------------------------------------------------------------------------------------------------
// Dados do celular e chave do whatsapp

String phoneNumber2[2] = { "+5545999999999", "+5545999999998" };  // Número de celular com formato internacional
String apiKey[2] = { "xxxxxxx", "xxxxxxx" };                      //a API irá fornecer essa chave. Basta entrar no site e cadastrar os celulares
bool valor = false;
int constante = 0;  // A chave que o bot do whatsapp enviou
int erros = 0;
float tempo = 0;
int quantidade_numeros = 2;
const int BUZZER = 23;
const int LED = 13;
const int botao = 33;

//-----------------------------------------------------------------------------------------------------------------------------------------

void setup() {
  delay(1000);
  pinMode(LED, OUTPUT);     // Configura pino do led1 como saida GPIO_NUM_13
  pinMode(BUZZER, OUTPUT);  // Configura pino do buzzer como saida GPIO_NUM_23

  Serial.begin(115200);  // Inicia a serial com 115200 bps

  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();


  if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT0)  //Se a razão de acordar for um evento externo (pressionar o botão)
  {
    WiFi.begin(SSID, PASSWORD);    // Inicia o wifi com o nome da rede e a senha
    Serial.println("Conectando");  // Envia pela serial que esta conectando

    while ((WiFi.status() != WL_CONNECTED) && (tempo < 40))  // Aguarda a conexão
    {
      delay(500);         // Espera 500ms
      Serial.print(".");  // Vai escrevendo ...
      tempo += 0.5;
    }
  // Caso demorar demais, o código continua e o buzzer é acionado, já que não teve conexão wifi

    Serial.println("");                 // Pula uma linha
    Serial.print("Conectado no IP: ");  // Mostra que esta conectado no IP
    Serial.println(WiFi.localIP());     // Mostra o IP no qual foi conectado

    WhatsAppMessages("O BOTÃO DE ALARME DA MARIA FOI ACIONADO!");  // Envia mensagem para o bot do whatsapp
  
    if (erros > 0) {  // Em caso de erro, acionar buzzer 5 vezes
      for (int i = 0; i < 5; i++) {
        digitalWrite(BUZZER, HIGH);
        delay(500);

        digitalWrite(BUZZER, LOW);
        delay(500);
      }
    }

    delay(1000);
    Serial.println("Going to sleep now");
    delay(1000);
  } else {
    Serial.println("O ESP nao foi acordado pelo botão");
  }

  esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 1);  //Habilita pino 33 (botão) para acordar o ESP32
  esp_deep_sleep_start();  //Aciona modo "sono profundo"
  Serial.println("Isso nunca será imprimido");
}

//-----------------------------------------------------------------------------------------------------------------------------------------

void loop() {
  delay(250);
}

//-----------------------------------------------------------------------------------------------------------------------------------------
// Definição das funções

void WhatsAppMessages(String message) {
  for (int i = 0; i < quantidade_numeros; i++) {
    // Dados a serem enviados pelo método Post suportado pelo protocolo HTTP
    String url = "https://api.callmebot.com/whatsapp.php?phone=" + phoneNumber2[i] + "&apikey=" + apiKey[i] + "&text=" + urlEncode(message);

    HTTPClient http;  // Cria um objeto HTTP
    http.begin(url);  // Inicia o objeto HTTP passando a string acima

    http.addHeader("Content-Type", "application/x-www-form-urlencoded");  // Adiciona um cabeçalho

    uint16_t httpResponseCode = http.POST(url);  // Envia o método POST HTTP de requisição e lê o resultado

    if (httpResponseCode == 200)  // Deu tudo certo?
    {
      Serial.print("Mensagem enviada com sucesso!");  // Envia mensagem na serial que deu certo
     
      for(int i=0; i<5; i++){   //Aciona o led 5 vezes
        digitalWrite(LED, HIGH);
        delay(1000);
        digitalWrite(LED, LOW);
        delay(500);
      }
      delay(500);  // Espera 0,5s

    } else          // Caso contrário, se não foi possivel enviar a mensagem
    {
      Serial.println("Erro ao tentar enviar a mensagem!");  // Envia para a serial a mensagem de erro
      Serial.print("Código HTTP: ");                        // Vai enviar para a serial o código de erro que retornou
      Serial.println(httpResponseCode);                     // Envia o código de erro
      erros += 1;
    }
    http.end();  // Finaliza o objeto http
  }
}