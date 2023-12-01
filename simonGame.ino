int leds[] = {22, 18, 16, 2};
int bts[]  = {23, 19, 17, 4};

String labels[] = {"Red", "Green", "Yellow", "Blue"};

QueueHandle_t sequenceQueue;
SemaphoreHandle_t mutex;

void led(void *pvParameters)
{
  while (1)
  {
    for (int i = 0; i < 4; i++)
    {
      digitalWrite(leds[i], LOW);
      delay(150);
    }

    for (int i = 0; i < 4; i++)
    {
      if (digitalRead(bts[i]) == LOW)
      {
        digitalWrite(leds[i], HIGH);
        delay(100);
        Serial.println(labels[i]);

        xQueueSend(sequenceQueue, &i, portMAX_DELAY);
      }
    }

    delay(10);
  }
}

void simonGame(void *pvParameters)
{
  int round = 1;
  int score = 0;
  int expectedButton;
  int userButton;

  while (round <= 10)
  {
    int sequence[round];
    for (int i = 0; i < round; i++)
    {
      sequence[i] = random(4);
    }

    for (int i = 0; i < round; i++)
    {
      digitalWrite(leds[sequence[i]], HIGH);
      delay(1000); 
      digitalWrite(leds[sequence[i]], LOW);
      delay(200);
    }

    for (int i = 0; i < round; i++)
    {
      xQueueReceive(sequenceQueue, &userButton, portMAX_DELAY);

      if (userButton != sequence[i])
      {
        Serial.printf("\nGame Over! Score: %d/10", score);
        Serial.println("\n============================\n");

        vTaskDelete(NULL);
      }
    }

    score++;
    
    vTaskDelay(pdMS_TO_TICKS(2000));
    round++;
  }

  vTaskDelete(NULL);
}

void setup()
{
  Serial.begin(115200);
  Serial.println("\n======== Simon Game ========\n");

  sequenceQueue = xQueueCreate(10, sizeof(int));
  mutex = xSemaphoreCreateMutex();

  for (int i = 0; i < 4; i++)
  {
    pinMode(leds[i], OUTPUT);
    pinMode(bts[i], INPUT_PULLUP);
  }

  xTaskCreatePinnedToCore(led, "Led", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(simonGame, "SimonGame", 4096, NULL, 2, NULL, 1);
}

void loop() {}
