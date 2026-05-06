
void cekJadwalPanel(uint8_t jamNow, uint8_t menitNow) {
  if(!config.stateAlarm) return;
  
  // Hitung waktu sekarang & jadwal dalam menit
  uint16_t nowTime  = jamNow * 60 + menitNow;
  uint16_t onTime   = config.jamOn * 60 + config.menitOn;
  uint16_t offTime  = config.jamOff * 60 + config.menitOff;

  bool shouldOn;

  // ====== HANDLE JADWAL NORMAL (ON < OFF) ======
  if (onTime < offTime) {
    shouldOn = (nowTime >= onTime && nowTime < offTime);
  }
  // ====== HANDLE JADWAL LEWAT TENGAH MALAM ======
  else {
    shouldOn = (nowTime >= onTime || nowTime < offTime);
  }

  // ====== EKSEKUSI ======
  if (shouldOn && !panelState) {
    panelON();
  } 
  else if (!shouldOn && panelState) {
    panelOFF();
  }
}

void panelON() {
  Disp.setBrightness(config.brightness);
  panelState = true;
  Serial.println("Panel ON");
}

void panelOFF() {
  Disp.setBrightness(0);
  panelState = false;
  Serial.println("Panel OFF");
}
