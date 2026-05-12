bool cekJadwalJumat(uint8_t jamNow, uint8_t menitNow,uint8_t dayy) {
  //RtcDateTime now = Rtc.GetDateTime();
  // Asumsi hari ke-5 atau ke-6 adalah Jumat (Tergantung library RTC kamu, biasanya 5 = Jumat)
  if (dayy != 5) return false; 
  
  uint16_t waktuSekarang = (jamNow * 60) + menitNow;
  uint16_t waktuMulai = (config.jumatMulaiJam * 60) + config.jumatMulaiMenit;
  uint16_t waktuSelesai = (config.jumatSelesaiJam * 60) + config.jumatSelesaiMenit;

  // Jika waktu sekarang berada di dalam rentang jadwal
  if (waktuSekarang >= waktuMulai && waktuSekarang < waktuSelesai) {
    return true;
  }
  else 
  return false;
}


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
