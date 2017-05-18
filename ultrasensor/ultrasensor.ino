
#define PinTrig 7 //pin para el la lectura de la señal
#define PinEcho 8 //pin para generar la señal

void setup() {
  Serial.begin (9600);
  pinMode(PinEcho, OUTPUT); 
  pinMode(PinTrig, INPUT);
}

void loop() {
   float duracion;
   float distancia;
   digitalWrite(PinTrig, LOW);
   delayMicroseconds(2); //espera 2 microsegundos para seguir ejecutando el sketch
   digitalWrite(PinEcho, HIGH);

   delayMicroseconds(8); //espera 8 microsegundos para emular el los ciclos del sonic burst
   digitalWrite(PinEcho, LOW);
   duracion = pulseIn(PinTrig, HIGH); //PulseIn toma el tiempo que tarda entre pasar de LOW a HIGH o al contrario en un pin
   distancia = (duracion/2) / 58; //formula para generar al distancia en centimetros

 //condicionales para establecer limites
  if (distancia >= 300 || distancia <= 0){
    Serial.println("Fuera de Rango");
  }
  else {
    Serial.print("La distacia es de ");
    Serial.print(distancia);
    Serial.println(" cm");
  }
  delay(500);//espera de medio segundo entre calculo muestra y muestra
}
