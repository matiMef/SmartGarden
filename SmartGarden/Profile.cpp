#include "Profile.h"

extern int profile;

void tftPrint();

void setProfile(int newProfile) {
  profile = newProfile;
  // writing data 
  EEPROM.write(0, profile); 
}

void changeProfile() {
  int newProfile;
  if ( digitalRead(buttonPin) == 0){
    if ( profile == 0 || profile == 1){
      newProfile = profile + 1;
      setProfile(newProfile);
      tftPrint();
    } 
    else{
      newProfile = 0;
      setProfile(newProfile);
      tftPrint();
    }
  };
}
