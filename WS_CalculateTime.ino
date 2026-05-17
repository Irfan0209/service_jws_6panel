
/*/======================== new program =========================//
void islam() {
  static uint32_t lastRtcRead = 0;
  static RtcDateTime waktuSekarang;
  
  if (millis() - lastRtcRead > 1000) {
    waktuSekarang = Rtc.GetDateTime();
    lastRtcRead = millis();
  }

  if(cekPersiapanSemuaAdzan(waktuSekarang) && !adzan){ Serial.println("iqomah"); show = ANIM_PREIQOMAH1 ; }
  
  RtcDateTime now = Rtc.GetDateTime();
  
  cekJadwalPanel(now.Hour(), now.Minute());

  static bool last = false; // Lebih baik pakai false/true untuk tipe boolean agar konsisten

// Langsung lempar nilai kembalian fungsi ke variabel
JUMAT = cekJadwalJumat(now.Hour(), now.Minute(), now.DayOfWeek()); 

// Logika transisi state (Rising Edge & Falling Edge)
if (JUMAT && !last) {
  show = ANIM_JUMAT1;
  last = true;
} else if (!JUMAT && last) {
  show = ANIM_CLOCK;
  last = false;
}

  
  static int8_t lastHalfPlay = -1;

  // Bunyi jam tepat
  if (now.Minute() == 0 && now.Second() == 0 && now.Hour() != lastHalfPlay && config.stateBuzzerClock) {
    lastHalfPlay = now.Hour();
    stateBuzzWar = 1;
  }

   static int8_t lastDayCalc = -1;
  if (now.Minute() == 0 && now.Second() == 0 && now.Hour() != lastDayCalc){
    lastDayCalc = now.Hour();
    butuhHitungJadwal = true; 
  }

  // Restart otomatis (opsional, dari kodemu)
  if (now.Minute() == 0 && now.Second() == 0 && now.Hour() == 0){
    Serial.println("restart");
    // ESP.restart(); // (Aktifkan jika memang alat diset untuk restart harian)
  }

  // --- EKSEKUSI RUMUS HANYA JIKA FLAG AKTIF ---
  if(butuhHitungJadwal){
    // Menggunakan perulangan for untuk eksekusi 2 kali
    for(uint8_t i = 0; i < 2; i++) {
      JWS.Update(config.zonawaktu, config.latitude, config.longitude, config.altitude, now.Year(), now.Month(), now.Day());
      JWS.setIkhtiSu = dataIhty[0];
      JWS.setIkhtiDzu = dataIhty[1];
      JWS.setIkhtiAs = dataIhty[2];
      JWS.setIkhtiMa = dataIhty[3];
      JWS.setIkhtiIs = dataIhty[4];
      JWS.setIkhtiIm = dataIhty[5];
      Hijir.Update(now.Year(), now.Month(), now.Day(), config.Correction);
    }
    butuhHitungJadwal = false; // Matikan flag setelah kedua eksekusi selesai
  }
}*/

void islam() {

   RtcDateTime now = now = Rtc.GetDateTime(); // Gunakan 'now' sebagai variabel statis penyimpan waktu
  
  // Eksekusi panel berdasarkan waktu yang di-cache
  cekJadwalPanel(now.Hour(), now.Minute());

  // 2. LOGIKA STATE MACHINE (JUMAT & PERSIAPAN SHOLAT)
  static bool lastJumat = false;
  static bool lastPreIqomah = false;
  
  JUMAT = cekJadwalJumat(now.Hour(), now.Minute(), now.DayOfWeek()); 
  PRE_IQOMAH = (cekPersiapanSemuaAdzan(now) && !adzan);

  // Transisi Persiapan Sholat (Tarhim)
  if (PRE_IQOMAH && !lastPreIqomah) {
    show = ANIM_PREIQOMAH1;
    lastPreIqomah = true;
  } 
  else if (!PRE_IQOMAH && lastPreIqomah) {
    lastPreIqomah = false;
  }

  // Transisi Jumat
  if (JUMAT && !lastJumat) {
    show = ANIM_JUMAT1;
    lastJumat = true;
  } 
  else if (!JUMAT && lastJumat) {
    // Saat Jumat selesai, kembalikan ke jam normal (tapi pastikan tidak sedang Tarhim)
    if (!PRE_IQOMAH) show = ANIM_CLOCK;
    lastJumat = false;
  }

  // 3. LOGIKA BUNYI JAM TEPAT
  static int8_t lastHalfPlay = -1;
  if (now.Minute() == 0 && now.Second() == 0 && now.Hour() != lastHalfPlay && config.stateBuzzerClock) {
    lastHalfPlay = now.Hour();
    stateBuzzWar = 1;
  }

  // 4. LOGIKA PEMBARUAN JADWAL HARIAN
  static int8_t lastDayCalc = -1;
  if (now.Minute() == 0 && now.Second() == 0 && now.Hour() != lastDayCalc){
    lastDayCalc = now.Hour();
    butuhHitungJadwal = true; 
  }

  // Restart otomatis
  if (now.Minute() == 0 && now.Second() == 0 && now.Hour() == 0){
    Serial.println("restart");
    // ESP.restart(); 
  }

  // 5. EKSEKUSI RUMUS KALKULASI JWS
  if(butuhHitungJadwal){
    // Pertanyaan: Kenapa i < 2 (dieksekusi dua kali)? 
    // Jika library tidak wajib butuh 2 pass, i < 1 sudah cukup.
    for(uint8_t i = 0; i < 2; i++) {
      JWS.Update(config.zonawaktu, config.latitude, config.longitude, config.altitude, now.Year(), now.Month(), now.Day());
      JWS.setIkhtiSu = dataIhty[0];
      JWS.setIkhtiDzu = dataIhty[1];
      JWS.setIkhtiAs = dataIhty[2];
      JWS.setIkhtiMa = dataIhty[3];
      JWS.setIkhtiIs = dataIhty[4];
      JWS.setIkhtiIm = dataIhty[5];
      Hijir.Update(now.Year(), now.Month(), now.Day(), config.Correction);
    }
    butuhHitungJadwal = false; 
  }
}

// digunakan untuk menghitung hari pasaran
uint32_t jumlahhari() { 
  RtcDateTime now = Rtc.GetDateTime();
  uint8_t d = now.Day();
  uint8_t m = now.Month();
  uint16_t y = now.Year();

  static const uint16_t hb[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
  uint32_t ht = (y - 1970) * 365 - 1;
  uint16_t hs = hb[m - 1] + d;

  if (y % 4 == 0 && m > 2) hs++; // Tambahkan 1 hari jika tahun kabisat dan lewat Februari

  uint32_t kab = (y - 1969) / 4;  // Hitung langsung jumlah tahun kabisat

  return (ht + hs + kab);
}
