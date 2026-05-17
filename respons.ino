

/*/ Fungsi untuk mengecek apakah teks persiapan harus tampil
bool cekTampilTeksPersiapan(int rtcH, int rtcM, int adzanH, int adzanM, int offsetMenit) {
  // 1. Konversi waktu sekarang (RTC) ke total menit
  int totalMenitRTC = (rtcH * 60) + rtcM;
  
  // 2. Konversi waktu Adzan ke total menit
  int totalMenitAdzan = (adzanH * 60) + adzanM;
  
  // 3. Hitung target waktu mulai tampil (Adzan - 15 menit)
  int waktuMulaiTampil = totalMenitAdzan - offsetMenit;
  
  // Safety check: Jika pengurangan melewati tengah malam ke hari sebelumnya (jarang terjadi pada jadwal sholat, tapi aman untuk ditambahkan)
  if (waktuMulaiTampil < 0) {
    waktuMulaiTampil += 1440; // 1440 adalah total menit dalam 24 jam (24 * 60)
  }

  // 4. Logika penentu: 
  // Tampil JIKA waktu sekarang >= waktu mulai DAN waktu sekarang < waktu Adzan
  if (totalMenitRTC >= waktuMulaiTampil && totalMenitRTC < totalMenitAdzan) {
    return true; // Tampilkan teks
  } else {
    return false; // Sembunyikan teks
  }
}

// Asumsi array jadwal sholat di kodemu bernama jadwalJam[] dan jadwalMenit[]
// Sesuaikan nama array ini dengan yang ada di program aslimu.

bool cekPersiapanSemuaAdzan(RtcDateTime now) {
  uint16_t menitSekarang = (now.Hour() * 60) + now.Minute();
  constexpr uint8_t durasiPersiapan = 15;

  // Lakukan pemindaian ke 5 waktu sholat tersebut
  for (uint8_t i = 0; i < 5; i++) {
    //uint8_t idx = indeksWajib[i];
    uint16_t menitTarget = (sholatHour[i] * 60) + sholatMinute[i];
    
    // Pastikan kita hanya mengecek jadwal sholat yang belum terlewat (masih di depan)
    if (menitTarget > menitSekarang) {
      uint16_t selisihMenit = menitTarget - menitSekarang;
      
      // Jika selisihnya kurang dari atau sama dengan durasi persiapan (misal 15 menit)
      if (selisihMenit <= durasiPersiapan) {
        return true; // Beri sinyal untuk mulai menampilkan teks berjalan!
      }
    }
  }
  
  return false; // Belum masuk waktu persiapan untuk sholat manapun
}*/

bool cekPersiapanSemuaAdzan(RtcDateTime now) {
  uint16_t menitSekarang = (now.Hour() * 60) + now.Minute();
  constexpr uint8_t durasiPersiapan = 1;

  // Lakukan pemindaian ke 5 waktu sholat tersebut
  for (uint8_t i = 0; i < 5; i++) {
    
    // =========================================================
    // MODIFIKASI: Pengecualian Dzuhur di hari Jumat
    // Asumsi: now.DayOfWeek() == 5 adalah Jumat (library Makuna)
    // Asumsi: i == 1 adalah index untuk sholat Dzuhur
    // =========================================================
    if (now.DayOfWeek() == 5 && i == 1) {
      continue; // Abaikan pengecekan untuk index ini, lanjut ke Ashar
    }

    uint16_t menitTarget = (sholatHour[i] * 60) + sholatMinute[i];
    
    // Pastikan kita hanya mengecek jadwal sholat yang belum terlewat (masih di depan)
    if (menitTarget > menitSekarang) {
      uint16_t selisihMenit = menitTarget - menitSekarang;
      
      // Jika selisihnya kurang dari atau sama dengan durasi persiapan (misal 15 menit)
      if (selisihMenit <= durasiPersiapan) {
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
