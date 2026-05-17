

bool cekPersiapanSemuaAdzan(RtcDateTime now) {
  uint16_t menitSekarang = (now.Hour() * 60) + now.Minute();

  // Lakukan pemindaian ke 5 waktu sholat tersebut
  for (uint8_t i = 0; i < 5; i++) {
    // =========================================================
    if (now.DayOfWeek() == 5 && i == 1) {
      continue; // Abaikan pengecekan untuk index ini, lanjut ke Ashar
    }

    uint16_t menitTarget = (sholatHour[i] * 60) + sholatMinute[i];
    
    // Pastikan kita hanya mengecek jadwal sholat yang belum terlewat (masih di depan)
    if (menitTarget > menitSekarang) {
      uint16_t selisihMenit = menitTarget - menitSekarang;
      
      // Jika selisihnya kurang dari atau sama dengan durasi persiapan (misal 15 menit)
      if (selisihMenit <= config.durasiPraiqomah) {
        return true; // Beri sinyal untuk mulai menampilkan teks berjalan!
      }
    }
  }
  
  return false; // Belum masuk waktu persiapan untuk sholat manapun
}

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
