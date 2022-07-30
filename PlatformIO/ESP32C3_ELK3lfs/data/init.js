//ESP32C2-12F-Kit LED toggling... 
let led = 3;                   // LED pin
gpio.mode(led, 2);         // Set LED pin to output mode
while (1) {
  gpio.write(led, 1);
  delay(500);
  gpio.write(led, 0);
  delay(500);
  led++;
  if (led>5) led=3;
  gpio.mode(led, 2);  
}