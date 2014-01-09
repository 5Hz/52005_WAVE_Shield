#include <AF_Wave.h>
#include <util.h>
#include <wave.h>
#include <avr/pgmspace.h>

#define IMAGINE "IMAGINE1.WAV"
#define SNOW "SNOWHEYO.WAV"
#define DARKNESS "UNDERCOV.WAV"
#define LIVEONCE "YOUONLYL.WAV"
#define HIGHWAY "HIGHWAYT.WAV"
#define ROCK "ROCKNROL.WAV"
#define VIDA "VIVALAVI.WAV"
#define BRIDGE "UNDERTHE.WAV"
#define RINGTONE "RINGTONE.WAV"

boolean OpenFlag = false;
int actualFlag = 0;

char *listilla[8] = {
"IMAGINE1.WAV",
"SNOWHEYO.WAV",
"UNDERCOV.WAV",
"YOUONLYL.WAV",
"HIGHWAYT.WAV",
"ROCKNROL.WAV",
"VIVALAVI.WAV",
"UNDERTHE.WAV"
};

AF_Wave card;
File f;
Wavefile wave;      // only one!

#define redled 9

int wasplaying = 0;

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Wave test!");
  Serial.println("Elige una cancion :) \n");
  Serial.println("a. IMAGINE - John Lennon");
  Serial.println("b. SNOW - Red Hot Chili Peppers");
  Serial.println("c. UNDER COVER OF DARKNESS - The Strokes");
  Serial.println("d. YOU ONLY LIVE ONCE - The Strokes");
  Serial.println("e. HIGHWAY TO HELL - ACDC");
  Serial.println("f. ROCK' N ROLL TRAIN - ACDC");
  Serial.println("g. VIVA LA VIDA - Coldplay");
  Serial.println("h. UNDER THE BRIDGE - Red Hot Chili Peppers");
  Serial.println(">. Reproducir la siguiente cancion");
  Serial.println("<. Reproducir la cancion anterior");
  
  
  pinMode(2, OUTPUT); 
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(redled, OUTPUT);
  
  if (!card.init_card()) {
    Serial.println("Card init. failed!"); return;
  }
  if (!card.open_partition()) {
    Serial.println("No partition!"); return;
  }
  if (!card.open_filesys()) {
    Serial.println("Couldn't open filesys"); return;
  }
  if (!card.open_rootdir()) {
    Serial.println("Couldn't open dir"); return;
  }

  ls();
//  playfile(ONEKHZ);

}

void loop() { 
     char x, *toplay;
     if (Serial.available()) {
     x = Serial.read();
 Serial.println(x);

     if (x == '>') {
       if(actualFlag == 9)
       {
         actualFlag = 1;
         toplay = listilla[actualFlag - 1];
       }
       else
       {
         actualFlag++;
         toplay = listilla[actualFlag - 1];
       }
     }
     else if(x == '<')
     {
       if(actualFlag <= 1)
       {
         actualFlag = 9;
         toplay = listilla[actualFlag - 1];
       }
       else
       {
         actualFlag--;
         toplay = listilla[actualFlag - 1];
       }
     }
     else
     {
       toplay = listilla[x - 'a'];
       actualFlag = x - 'a' + 1;
     }
//     else {
//       return;
//     }
     
     if (wave.isplaying) {// already playing something, so stop it!
         wave.stop(); // stop it
         card.close_file(f);
         OpenFlag  = false;
         //actualFlag = 0;
     }
     playfile(toplay);
   }
   
   if (OpenFlag && !wave.isplaying) {
     card.close_file(f);
     OpenFlag  = false;
     actualFlag = 0;
   }

   if (wave.isplaying && !wasplaying) {
     digitalWrite(redled, HIGH);
   } else if (!wave.isplaying && wasplaying) {
     digitalWrite(redled, LOW);
   }
   wasplaying = wave.isplaying;
   delay(100);
}


void ls() {
  char name[13];
  card.reset_dir();
  Serial.println("Files found:");
    while (1) {
    if (!card.get_next_name_in_dir(name)) {
       card.reset_dir();
       return;
    }
    Serial.println(name);
  }
}

void playfile(char *name) {
   f = card.open_file(name);
   if (f) {
     OpenFlag  = true;
   }
   else {
      Serial.println("Couldn't open file"); return;
   }
   if (!wave.create(f)) {
      Serial.println("Not a valid WAV"); return;
   }
   // ok time to play!
   wave.play();
}

void playcomplete(char *name) {
  // call our helper to find and play this name
  playfile(name);
  while (wave.isplaying) {
  // do nothing while its playing
  }
  // now its done playing
}
