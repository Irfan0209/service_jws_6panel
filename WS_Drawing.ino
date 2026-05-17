
//=================== new variabel ========================//
char * const pasar[]  = {"WAGE", "KLIWON", "LEGI", "PAHING", "PON"}; 
char * const Hari[]  = {"MINGGU","SENIN","SELASA","RABU","KAMIS","JUM'AT","SABTU"};
char * const bulanMasehi[]  = {"JANUARI", "FEBRUARI", "MARET", "APRIL", "MEI", "JUNI", "JULI", "AGUSTUS", "SEPTEMBER", "OKTOBER", "NOVEMBER", "DESEMBER" };
char* jadwal[] = {"IMSAK","SUBUH", "TERBIT","DHUHA", "DZUHUR", "ASHAR", "MAGHRIB", "ISYA'"};
char* jadwalAzzan[] = {"SUBUH","DZUHUR", "ASHAR", "MAGHRIB", "ISYA'"};
char * namaBulanHijriah[] = {
    "MUHARRAM", "SHAFAR", "RABIUL AWAL",
    "RABIUL AKHIR", "JUMADIL AWAL", 
    "JUMADIL AKHIR", "RAJAB",
    "SYA'BAN", "RAMADHAN", "SYAWAL",
    "DZULQA'DAH", "DZULHIJAH"
};

//==================== animasi jam dan running text =================//

void dwMrq(const char* msg, uint8_t Speed, uint8_t dDT, uint8_t fontt) { 
  static uint16_t x; 
  static uint16_t fullScroll = 0;
  
  // if(adzan) return;

  if (reset_x != 0) { 
    x = 0; 
    reset_x = 0; 
    fullScroll = 0;
    return;
  }      

  uint32_t Tmr = millis();
  static uint32_t lss = 0;
  
  // HAPUS: RtcDateTime now = Rtc.GetDateTime(); -> Tidak dipakai sama sekali

  // Eksekusi HANYA jika waktunya update frame (sesuai Speed)
  if ((Tmr - lss) > Speed) { 
    lss = Tmr;

    // 1. OPTIMASI KALKULASI: Hitung lebar teks SEKALI SAJA saat animasi mau mulai
      fType(fontt);
      uint16_t w = Disp.textWidth(msg);
      if (w == 0) {
        nextShowState();
        return;
      }
      fullScroll = w + DWidth;

    // 2. Logika Increment X
    if (x < fullScroll) {
      ++x;
    } else {
      nextShowState();
      x = 0; 
      fullScroll = 0;
      return; // Keluar agar tidak menggambar frame yang tidak perlu
    }

    // 3. Render UI berdasarkan mode dDT
    if (dDT == 1) { // Jam di bawah, Running Text di atas
      fType(fontt);
      Disp.drawText(DWidth - x, 0, msg);

      fType(1); // Set font SEKALI saja untuk semua kondisi TGLMASEHI di bawah ini
      if (x <= 6)                      { dwCtr(32, 16 - x, TGLMASEHI()); }
      else if (x >= (fullScroll - 6))  { dwCtr(32, 16 - (fullScroll - x), TGLMASEHI()); }
      else                             { dwCtr(32, 9, TGLMASEHI()); }

      if (x <= 16)                     { Center(0, 16 - x); }
      else if (x >= (fullScroll - 16)) { Center(0, 16 - (fullScroll - x)); }
      else                             { Center(0, 0); }
    }
    else if (dDT == 2) { // Jam di atas, Running Text di bawah
      fType(fontt);
      Disp.drawText(DWidth - x, 9, msg);

      fType(1); // Set font SEKALI saja untuk semua kondisi TGLMASEHI di bawah ini
      if (x <= 6)                      { dwCtr(32, x - 6, TGLMASEHI()); }
      else if (x >= (fullScroll - 6))  { dwCtr(32, (fullScroll - x) - 6, TGLMASEHI()); }
      else                             { dwCtr(32, 0, TGLMASEHI()); }

      if (x <= 16)                     { Center(0, x - 16); }
      else if (x >= (fullScroll - 16)) { Center(0, (fullScroll - x) - 16); }
      else                             { Center(0, 0); }
    }
    else if (dDT == 3) { // Mode 3: Running text di atas, animasi jam di tengah horizontal
      fType(fontt);
      Disp.drawText(DWidth - x, 0, msg);
      
      if (x <= 32)                     { Center(x - 32, 0); }
      else if (x >= (fullScroll - 32)) { Center((fullScroll - x) - 32, 0); }
      else                             { Center(0, 0); }
    }
    
    DoSwap = true;
  }       
}

void nextShowState()
{ 
  switch(show){
    case ANIM_DATE:    show = ANIM_TEXT1; break;
    case ANIM_TEXT1:   show = ANIM_TEXT2; break;
    case ANIM_TEXT2:   show = ANIM_TEXT3; break;
    case ANIM_TEXT3:   show = ANIM_TEXT4; break;
    case ANIM_TEXT4:   show = ANIM_TEXT5; break;
    case ANIM_TEXT5:   (JUMAT==1)?show = ANIM_JUMAT1 : show = ANIM_SHOLAT; break;
    case ANIM_JUMAT1:  (JUMAT==1)?show = ANIM_JUMAT2 : show = ANIM_SHOLAT; break;
    case ANIM_JUMAT2:   show = ANIM_SHOLAT; break;
    case ANIM_NAME:     show = ANIM_CLOCK; break;
   // case ANIM_ADZAN:    show = ANIM_IQOMAH1; break;
  }
}

void Center(int8_t x,int8_t y){
  if(adzan) return;

  RtcDateTime now = Rtc.GetDateTime();
    
  Disp.drawFilledRect(0, 0, 31, 16, 0);
  
  if(now.Second() % 2 ){
      Disp.drawCircle(x + 15,y + 4,1,1);
      Disp.drawCircle(x + 15,y + 11,1,1);
    }else{
      Disp.drawCircle(x + 15,y + 4,1,0);
      Disp.drawCircle(x + 15,y + 11,1,0);
    }

  fType(3);
  
  Disp.drawChar(x + 0, y, '0' + now.Hour() / 10);
  Disp.drawChar(x + 7, y, '0' + now.Hour() % 10); 
  
  Disp.drawChar(x + 18, y, '0' + now.Minute() / 10);
  Disp.drawChar(x + 25, y, '0' + now.Minute() % 10);
}

void jamCenter() {
  if (adzan) return;
  
  static byte y;
  static bool s; // 0=in, 1=out
  static uint32_t lsRn;
  
  // Tambahan untuk optimasi CPU
  static RtcDateTime now;
  static uint32_t lastRtcRead; 
  
  uint32_t Tmr = millis();

  // 1. OPTIMASI: Baca RTC HANYA setiap 1 detik (1000 ms) atau saat animasi baru mulai (y==0)
  // Ini akan menghemat resource CPU secara drastis!
  if ((Tmr - lastRtcRead) >= 1000 || y == 0) { 
    now = Rtc.GetDateTime();
    lastRtcRead = Tmr;
  }

  // 2. Cache kalkulasi posisi Y
  int8_t drawY = 17 - y;

  if ((Tmr - lsRn) > 75) {
    if (s == 0 && y < 17) { lsRn = Tmr; y++; }
    else if (s == 1 && y > 0) { lsRn = Tmr; y--; }
  }

  fType(5);

  // 3. Variabel lokal
  uint8_t h = now.Hour();
  uint8_t m = now.Minute();
  bool blink = now.Second() % 2;
  
  // Gambar Jam
  Disp.drawChar(75, drawY, '0' + h / 10);
  Disp.drawChar(84, drawY, '0' + h % 10); 
  
  Disp.drawChar(99, drawY, '0' + m / 10);
  Disp.drawChar(108, drawY, '0' + m % 10);
  
  // Titik dua (colon) yang berkedip
  Disp.drawCircle(95, drawY + 4, 1, blink);
  Disp.drawCircle(95, drawY + 11, 1, blink);
      
  // Animasi Ikon
  icon1(165, y - 17);
  icon2(5, y - 17);
  
  // 4. Logika Transisi (Tahan 5 detik sebelum keluar)
  if ((Tmr - lsRn) > 8000 && y == 17) { s = 1; }
  
  if (y == 0 && s == 1) {
    Disp.clear();
    Serial.printf_P(PSTR("TIME:%d,%d,%d,%d\n"), h, m, now.Second(), now.DayOfWeek());
    s = 0;
    show = ANIM_DATE;
  }
  
  DoSwap = true;
}

//=======================
void drawJadwalSholat() {
  if(adzan) return;
  
  RtcDateTime now = Rtc.GetDateTime();
  static int y = 0, y1 = 0;
  static uint8_t s = 0, s1 = 0;
  static bool run = false;
  static uint8_t pairIdx = 0; // 0:Imsak-Subuh, 1:Terbit-Dhuha, dst

  static uint8_t list = 0;
  static uint32_t lsRn_y1 = 0;
  static uint32_t lsRn_y = 0;
  static uint32_t tHold = 0;

  uint32_t Tmr = millis();

  // Setiap pairIdx mengambil data: n (jadwal pertama) dan n+1 (jadwal kedua)
  uint8_t idx1 = pairIdx * 2;
  uint8_t idx2 = (pairIdx * 2) + 1;

  float time1 = getJWSValue(idx1);
  float time2 = getJWSValue(idx2);

  int8_t drawY = y1 - 17;
  // Transisi vertikal y1 (jam muncul/hilang)
  if ((Tmr - lsRn_y1) > 55) {
    lsRn_y1 = Tmr;

    if (s1 == 0 && y1 < 17) { y1++; }
    else if (s1 == 1 && y1 > 0) { y1--; }
  }

  // Saat y1 selesai muncul, mulai animasi jadwal
  if (y1 == 17 && s1 == 0) {
    run = true; 
    if(now.Second() % 2 ){
      Disp.drawCircle(15,drawY + 4,1,1);
      Disp.drawCircle(15,drawY + 11,1,1);
    }else{
      Disp.drawCircle(15,drawY + 4,1,0);
      Disp.drawCircle(15,drawY + 11,1,0);
    }
  }

  // Animasi gerakan teks (y)
  if (run && (Tmr - lsRn_y) > 55) {
    lsRn_y = Tmr;

    if (s == 0 && y < 9) {
      y++;
    } else if (s == 1 && y > 0) {
      y--;
    }
  }

  // Delay sebelum animasi keluar (reverse)
  if (y == 9 && s == 0 && tHold == 0) {
    tHold = millis();
  }
  if (tHold > 0 && (millis() - tHold > 4000)) {
    s = 1;     // mulai keluar
    tHold = 0; // reset timer
  }

  // Setelah animasi selesai
  if (y == 0 && s == 1) {
    s = 0;
    pairIdx++;
        if (pairIdx > 3) { // Jika sudah sampai Isya, kembali ke animasi utama
          pairIdx = 0;
          run = false;
          s1 = 1; // trigger keluar vertikal
        }
  }

// Tampilkan jam digital
  fType(3);
  Disp.drawChar(0, drawY, '0' + now.Hour() / 10);
  Disp.drawChar(7, drawY, '0' + now.Hour() % 10);

  Disp.drawChar(18, drawY, '0' + now.Minute() / 10);
  Disp.drawChar(25, drawY, '0' + now.Minute() % 10);

  fType(1);
  dwCtr(32,drawY,TGLMASEHI());

  // Baris 1: Jadwal Pertama (misal: Imsak)
  char buf1[30];
  snprintf(buf1, sizeof(buf1), "%s %02d:%02d  %s %02d:%02d", 
           jadwal[idx1], (uint8_t)time1, (uint8_t)((time1 - (uint8_t)time1) * 60),jadwal[idx2], (uint8_t)time2, (uint8_t)((time2 - (uint8_t)time2) * 60));
  
  fType(1); 
  dwCtr(32,18 - y, buf1); // Tampilkan di baris atas7711

  DoSwap = true;
  
  if (y1 == 0 && s1 == 1) {
    s1 = 0;
    show = ANIM_NAME; // Atau state awal kamu
  }
  
}

// Helper untuk ambil nilai float JWS berdasarkan index
float getJWSValue(uint8_t index) {
  switch (index) {
    case 0: return JWS.floatImsak;
    case 1: return JWS.floatSubuh;
    case 2: return JWS.floatTerbit;
    case 3: return JWS.floatDhuha;
    case 4: return JWS.floatDzuhur;
    case 5: return JWS.floatAshar;
    case 6: return JWS.floatMaghrib;
    case 7: return JWS.floatIsya;
    default: return 0;
  }
}
//=======================

void drawTestPanel() {
    static uint32_t lastTestUpdate = 0;
    static uint8_t testStep = 0;
    uint32_t currentMillis = millis();

    // Ganti pola test setiap 2000 ms (2 detik)
    if (currentMillis - lastTestUpdate > 2000) { 
        lastTestUpdate = currentMillis;
        
        //Disp.clear(); // Selalu bersihkan layar sebelum ganti pola
        
        if (testStep == 0) {
            // STEP 1: Nyalakan SEMUA LED (Untuk cek dead pixel)
            // Sesuaikan dimensi (192, 16) dengan konfigurasi susunan 6 panelmu
            Disp.drawFilledRect(0, 0, DWidth, DHeight); 
            testStep = 1;
        } 
        else if (testStep == 1) {
            // STEP 2: Matikan SEMUA LED (Untuk cek LED yang stuck/nyala terus)
            // Layar dibiarkan kosong karena sudah di-clear di atas
            testStep = 2;
        }
        else if (testStep == 2) {
            // STEP 3: Pola Garis Horizontal (Untuk cek kestabilan jalur data IC)
            for (int y = 0; y < DHeight; y += 2) {
                Disp.drawLine(0, y, DWidth, y);
            }
            testStep = 3;
        }
        else if (testStep == 3) {
            // STEP 4: Pola Garis Vertikal
            for (uint8_t x = 0; x < DWidth; x += 4) {
                Disp.drawLine(x, 0, x, DHeight);
            }
            testStep = 4;
        }
        else if (testStep == 4) {
            // STEP 5: Pola Blok IC Shift Register 74HC595 (Sumbu X)
            for (uint8_t x = 0; x < DWidth; x += 8) {
                if ((x / 8) % 2 == 0) {
                    // Parameter dirubah menjadi (x1, y1, x2, y2)
                    // x2 diisi dengan (x + 7) agar lebarnya selalu 8 piksel
                    // y2 diisi dengan (DHeight - 1) agar tidak melebihi batas bawah panel
                    Disp.drawFilledRect(x, 0, x + 7, DHeight - 1);
                }
            }
            testStep = 5; // Lanjut ke step 6
        }
        else if (testStep == 5) {
            // STEP 6: Pola Cek IC Decoder 74HC138 (Sumbu Y - 1/4 Scan)
            // Menggambar garis horizontal setiap kelipatan 4 baris
            // (Mewakili baris ke 0, 4, 8, dan 12)
            for (uint8_t y = 0; y < DHeight; y += 4) {
                // Ubah DWidth menjadi (DWidth - 1) agar garis mentok di piksel terakhir
                Disp.drawLine(0, y, DWidth - 1, y);
            }
            testStep = 0; // Sequence selesai, ulangi dari awal
        }
        
        
        DoSwap = true; // Eksekusi gambar ke buffer layar
    }
}

void icon1 (uint8_t x, uint8_t y){
  if (adzan) return;
  static const uint8_t icon1[] PROGMEM = {
  21,16,
  0x11, 0x50, 0x00, 0x21, 0xf0, 0x20, 0x40, 0x02, 0x30, 0xa0, 0x23, 0x38, 0x40, 0x33, 0xb0, 0x00, 
  0x33, 0x30, 0x06, 0x33, 0x30, 0x0f, 0x33, 0x30, 0x0b, 0x33, 0x30, 0x8b, 0x33, 0x30, 0x9b, 0x33, 
  0x30, 0xf3, 0x33, 0x30, 0x63, 0x73, 0x30, 0x03, 0xf3, 0x30, 0x01, 0xbe, 0x20, 0x00, 0x1c, 0x20
  };
  Disp.drawBitmap(x,y,icon1);
}

void icon2 (uint8_t x, uint8_t y){
  if (adzan) return;
  static const uint8_t icon2[] PROGMEM = {
  22,16,
  0x00, 0x54, 0x00, 0x70, 0x7c, 0x38, 0x50, 0x00, 0x7c, 0x30, 0x00, 0x4c, 0x02, 0x00, 0x4c, 0x06, 
  0x00, 0x78, 0x07, 0x00, 0xe0, 0x03, 0x07, 0x80, 0x01, 0x9f, 0xc0, 0x01, 0x80, 0xf0, 0x41, 0x80, 
  0x18, 0x41, 0x81, 0xfc, 0x43, 0xe7, 0x8c, 0x7e, 0x76, 0x00, 0x3c, 0x1f, 0xc0, 0x00, 0x0f, 0xc0 
  };
  Disp.drawBitmap(x,y,icon2);
}


/*======================= animasi memasuki waktu sholat ====================================*/
void drawAzzan()
{
    //static const char *jadwal[] = {"SUBUH", "DZUHUR", "ASHAR", "MAGRIB","ISYA'"};
    const char *sholat = jadwalAzzan[sholatNow]; 
    static uint8_t ct = 0;
    static uint32_t lsRn = 0;
    uint32_t Tmr = millis();
    const uint8_t limit = config.durasiadzan;

    if (Tmr - lsRn > 500 && ct <= limit)
    {
        lsRn = Tmr;
        if (!(ct & 1))  // Lebih cepat dibandingkan ct % 2 == 0
        {
          fType(1);
//          dwCtr(0, 0, "ADZAN");
//          dwCtr(0, 8, sholat);
          Buzzer(1);
           
        }
        else
        {
          Buzzer(0);
        }
        ct++;
        //DoSwap = true;
    }
    
    if ((Tmr - lsRn) > 1500 && (ct > limit))
    {
        show = ANIM_IQOMAH1;
        ct = 0;
        Buzzer(0);
        // reset_x = 1;
    }
}

void runn(const char* msg, uint8_t speed, uint8_t fontt)
{
  static uint32_t x = 0;
  static uint32_t fullScroll = 0;
  static uint32_t lastMs = 0;

  if(reset_x){
    x = 0;
    fullScroll = 0;
    lastMs = 0;
    reset_x = 0;
    return;
  }
  
  // ====== Pesan baru → reset animasi ======

  fType(fontt);
  // ====== Pesan kosong → langsung lompat ======
  uint16_t w = Disp.textWidth(msg);
  if (w == 0) {
    nextShowStateRun();
    return;
  }

  // ====== Hitung panjang scroll hanya sekali ======
    fullScroll = w + DWidth;

  uint32_t now = millis();
  if (now - lastMs < speed) return;
  lastMs = now;

  // ====== Animasi scroll ======
  if (x < fullScroll) {
    x++;
  } else {
    x = 0;
    fullScroll = 0;
    nextShowStateRun();
    return;
  }

  Disp.drawText(
    DWidth - x,0,msg
  );
  DoSwap = true;
}

void nextShowStateRun()
{ 
  switch(show){
    case ANIM_IQOMAH1:    show = ANIM_IQOMAH2; break;
    case ANIM_IQOMAH2:   show = ANIM_IQOMAH1; break;
    
  }
}

void drawSmartText(const char* msg, uint8_t speed,uint8_t fontt) {
  static uint16_t x = 0;
  static uint16_t textW = 0;
  static uint16_t fullScroll = 0;
  static bool isScrolling = false;
  //static uint32_t diamTimer = 0; // Timer khusus untuk teks diam
  
  // Reset trigger dari luar (saat ganti menu/tampilan)
  if (reset_x != 0) { 
    x = 0; 
    textW = 0;
    reset_x = 0; 
    return;
  }

  uint32_t Tmr = millis();
  static uint32_t lss = 0;

  // Render frame setiap 45ms (Kecepatan standar agar smooth)
  if ((Tmr - lss) > speed) {
    lss = Tmr;

    fType(fontt);

    // ==============================================================
    // 1. OPTIMASI: Hitung lebar teks SEKALI SAJA di awal
    // ==============================================================
    if (textW == 0) {
      textW = Disp.textWidth(msg);
      Serial.println(textW);
      
      // Cek apakah lebar teks melebihi lebar layar 6 Panel (DWidth)
      if (textW > DWidth) {
        isScrolling = true;
        fullScroll = textW + DWidth;
      } else {
        isScrolling = false;
        //diamTimer = Tmr; // Mulai argon timer untuk durasi teks diam
      }
    }

    // ==============================================================
    // 2. LOGIKA TAMPILAN (Diam vs Berjalan)
    // ==============================================================
    if (isScrolling) {
      // --- MODE BERJALAN (Lebih dari 6 Panel) ---
      if (x < fullScroll) {
        ++x;
      } else {
        x = 0; 
        textW = 0;
        fullScroll = 0;
        
        // Panggil fungsi ganti tampilan di sini jika diperlukan
        // nextShowState(); 
        return;
      }
      
      // Render teks berjalan, posisi Y = 4 (tengah vertikal)
      Disp.drawText(DWidth - x, 4, msg);
      
    } else {
      // --- MODE DIAM (Kurang dari atau sama dengan 6 Panel) ---
      dwCtr(0, 4, msg);
      
      // PENTING: Karena teks diam tidak punya titik akhir scroll,
      // kita harus memberi batasan waktu tampil (misal: 5000 ms / 5 detik)
//      if (Tmr - diamTimer > 5000) {
//        textW = 0;
//        
//        // Panggil fungsi ganti tampilan di sini jika diperlukan
//        // nextShowState(); 
//        return;
//      }
    }

    DoSwap = true;
  }
}

void drawIqomah()  // Countdown Iqomah (9 menit)
{  
    static uint32_t lsRn = 0;
    static uint16_t ct = 0;  // Mulai dari 0 untuk menghindari error
    static uint8_t mnt, scd;
    char locBuff[25];  
    uint32_t now = millis();  // Simpan millis() di awal
    
    uint16_t cn_l = (iqomah[sholatNow] * 60);
    
    mnt = (cn_l - ct) / 60;
    scd = (cn_l - ct) % 60;
    snprintf(locBuff,sizeof(locBuff), "%s -%02d:%02d","MENUJU IQOMAH:", mnt, scd);

   // if ((ct & 1) == 0) {  // Gunakan bitwise untuk optimasi modulo 2
    fType(1);
    dwCtr(0, 4, locBuff);
    DoSwap = true;
    
    if (now - lsRn > 1000) 
    {   
        lsRn = now;
        ct++;

        if (ct > (cn_l - 5)) {
            Buzzer(ct & 1);  // Gunakan bitwise untuk menggantikan modulo 2
        }
    }

    if (ct >= cn_l)  // Pakai >= untuk memastikan countdown selesai dengan benar
    {
        ct = 0;
        Buzzer(0);
        reset_x = 1;
        show = ANIM_BLINK;
    }    
}

void blinkBlock()
{
    static uint32_t lsRn;
    static uint32_t lastMove = 0; 
    static uint16_t ct = 0;
    
    // Titik awal, kita mulai dari pojok kiri atas
    static int16_t posX = 0;
    static int16_t posY = 0;
    static int16_t dirX = 1;  
    static int16_t dirY = 1;  

    /* =============================================================
       BATAS PANTULAN UNTUK Disp.drawText (Rata Kiri Atas)
       Asumsi Lebar teks "00:00:00" = ~48 px, Tinggi teks = ~8 px
       Susunan 6 panel memanjang (192 x 16)
    ============================================================= */
    constexpr uint8_t minX = 0;          // Mentok layar kiri
    constexpr uint8_t maxX = 192 - 48;   // Mentok layar kanan (Lebar total - Lebar teks)
    constexpr uint8_t minY = 0;          // Mentok layar atas
    constexpr uint8_t maxY = 16 - 7;     // Mentok layar bawah (Tinggi total - Tinggi teks)

    // Catatan: Jika panelmu 2 baris (96 x 32), ubah nilai maxX menjadi 48 (96-48) 
    // dan maxY menjadi 24 (32-8).

    const uint16_t ct_l = displayBlink[sholatNow] * 60;  
    uint32_t noww = millis();  

    RtcDateTime now = Rtc.GetDateTime();

    uint8_t mnt = (ct_l - ct) / 60;
    uint8_t scd = (ct_l - ct) % 60;

    char timeBuff[9];
    snprintf(timeBuff, sizeof(timeBuff), "%02d:%02d:%02d", now.Hour(), now.Minute(), now.Second());

    // Update posisi teks setiap 50ms
    if (noww - lastMove > 150) 
    {
        lastMove = noww;
        posX += dirX;
        posY += dirY;

        // Putar balik arah jika menabrak batas
        if (posX <= minX || posX >= maxX) dirX = -dirX;
        if (posY <= minY || posY >= maxY) dirY = -dirY;
    }
    
    fType(1); // Pastikan ini mengatur ke font 5x7 
    
    // Cetak teks menggunakan Disp.drawText dengan posisi X dan Y yang bergerak
    Disp.drawText(posX, posY, timeBuff); 
    DoSwap = true;
      
    if (noww - lsRn > 1000)
    {
        lsRn = noww;
        ct++;
    }

    if (ct > ct_l)
    {
        sholatNow = -1;
        adzan = false;
        ct = 0;
        show = ANIM_CLOCK;
    }
}

/*void blinkBlock()
{
    static uint32_t lsRn;
    static uint16_t ct = 0;
    const uint16_t ct_l = displayBlink[sholatNow] * 60;  // Durasi countdown
    uint32_t noww = millis();  // Simpan millis()

    // Ambil waktu dari RTC
    RtcDateTime now = Rtc.GetDateTime();

    // Hitung countdown
    uint8_t mnt = (ct_l - ct) / 60;
    uint8_t scd = (ct_l - ct) % 60;

    // Tampilkan jam besar
    char timeBuff[9];
    snprintf(timeBuff,sizeof(timeBuff), "%02d:%02d:%02d", now.Hour(), now.Minute(), now.Second());
    
    fType(1);
    dwCtr(60, 8, timeBuff);
    DoSwap = true;
     
    // Update countdown setiap detik
    if (noww - lsRn > 1000)
    {
        lsRn = noww;
        ct++;
    }

    // Reset jika countdown selesai & kembali ke animasi utama
    if (ct > ct_l)
    {
        sholatNow = -1;
        adzan = false;
        ct = 0;
        show = ANIM_CLOCK;
    }
}*/
//===================================== end =================================//



void dwCtr(int8_t x, int8_t y, const char* Msg){
   uint16_t   tw = Disp.textWidth(Msg);
   int16_t   c = int16_t((DWidth-x-tw)/2);
   Disp.drawText(x+c,y,Msg);
}
  
void fType(uint8_t x)
  {
    if(x==0) Disp.setFont(Font0);
    else if(x==1) Disp.setFont(Font1); 
    else if(x==2) Disp.setFont(Font2);
    else if(x==3) Disp.setFont(Font3);
    else if(x==4) Disp.setFont(Font4);
    else if(x==5) Disp.setFont(Font5);
    else if(x==6) Disp.setFont(Font6); 
    else if(x==7) Disp.setFont(Font7); 
//    else if(x==8) Disp.setFont(Font8);
  }











  
