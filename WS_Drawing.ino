
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


uint16_t sholatSec[5];   // waktu sholat (detik)
//===================== convert jam & menit ke detik =============================//
inline uint32_t toSecond(uint8_t h, uint8_t m, uint8_t s = 0)
{
  return (uint32_t)h * 3600UL + (uint32_t)m * 60UL + s;
}


//==================== animasi jam dan running text =================//

void dwMrq(const char* msg, uint8_t Speed, uint8_t dDT,uint8_t fontt) //running teks ada jam nya
  { 
    static uint16_t   x; 
    static uint16_t fullScroll = 0;
    //if(adzan) return;
    if (reset_x !=0) { x=0; reset_x = 0; fullScroll = 0;}      

    uint32_t          Tmr = millis();
    static uint32_t lss=0;
    RtcDateTime now = Rtc.GetDateTime();
    
    fType(fontt);
    uint16_t w = Disp.textWidth(msg);
    if (w == 0) {
       nextShowState();
       return;
    }
    fullScroll = w + DWidth;
      
    if((Tmr-lss)> Speed)
      { lss=Tmr;
        if (x < fullScroll) {++x;}
        else {
          nextShowState();
          x = 0; 
          fullScroll = 0;
          return;}
     if(dDT==1)
        {
        //fType(1);  //Marquee    jam yang tampil di bawah
        Disp.drawText(DWidth - x, 0, msg); //runing teks diatas

        if (x<=6)                     {fType(1); dwCtr(32, 16-x, TGLMASEHI());}
        else if (x>=(fullScroll-6))   {fType(1); dwCtr(32, 16-(fullScroll-x), TGLMASEHI());}
        else                          {fType(1); dwCtr(32,9,TGLMASEHI());}//posisi jamnya yang bawah

        if (x<=16)                     {Center(0,16-x); }
        else if (x>=(fullScroll-16))   {Center(0,16-(fullScroll-x));}
        else                           { Center(0,0); }//posisi jamnya yang bawah
   
        }
     else if(dDT==2) //jam yang diatas
        {    
        Disp.drawText(DWidth - x, 9 , msg);//runinng teks dibawah

        if (x<=6)                     { fType(1); dwCtr(32, x-6, TGLMASEHI());}
        else if (x>=(fullScroll-6))   { fType(1); dwCtr(32, (fullScroll-x)-6, TGLMASEHI());}
        else                          { fType(1); dwCtr(32,0,TGLMASEHI());}//posisi jamnya yang bawah

        if (x<=16)                    { Center(0,x-16); }
        else if (x>=(fullScroll-16))   { Center(0,(fullScroll-x)-16); }
        else                          { Center(0, 0); }//posisi jamnya yang bawah
        
        }
      else if(dDT==3) //jam yang diatas
      { 
        fType(fontt);  //Marquee    jam yang tampil di bawah
        Disp.drawText(DWidth - x, 0, msg); //runing teks diatas
        if (x<=32)                     {Center(x-32,0); }
        else if (x>=(fullScroll-32))   {Center((fullScroll-x)-32,0);}
        else                           {Center(0, 0); }//posisi jamnya yang bawah
      }
        DoSwap = true;
      }       
  }   

void nextShowState()
{ 
  switch(show){
    //case ANIM_DATE:    show = ANIM_TEXT1; break;
    case ANIM_TEXT1:   show = ANIM_TEXT2; break;
    case ANIM_TEXT2:   show = ANIM_TEXT3; break;
    case ANIM_TEXT3:   show = ANIM_TEXT4; break;
    case ANIM_TEXT4:   show = ANIM_TEXT5; break;
    case ANIM_TEXT5:   show = ANIM_SHOLAT; break;
    case ANIM_NAME:   show = ANIM_CLOCK; break;
    //case ANIM_COUNTER: show = ANIM_BIGFONT; break;
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
  //DoSwap = true;
}

void jamCenter() {
  if (adzan) return;
  
  iconAllah(165);
  logo2(0);
  
  char jam[10];
  static byte y;
  static bool s; // 0=in, 1=out
  static uint32_t lsRn;
  //static uint8_t lastSec; // Simpan detik terakhir
  uint32_t Tmr = millis();

  // 1. Ambil waktu hanya jika diperlukan (misal: saat detik berubah atau animasi jalan)
  RtcDateTime now = Rtc.GetDateTime();
  
  // 2. Cache kalkulasi posisi Y
  int8_t drawY = 17 - y;

  if ((Tmr - lsRn) > 75) {
    if (s == 0 && y < 17) { lsRn = Tmr; y++; }
    else if (s == 1 && y > 0) { lsRn = Tmr; y--; }
  }

  fType(5);

  // 3. Optimasi Gambar: Gunakan variabel lokal untuk digit agar tidak hitung berulang
  uint8_t h = now.Hour();
  uint8_t m = now.Minute();
  
  bool blink = now.Second() % 2;
  
//  snprintf(jam,sizeof(jam),"%02d %02d",h,m);
//
//  dwCtr(0,drawY,jam);
  Disp.drawChar(75, drawY, '0' + h / 10);
  Disp.drawChar(84, drawY, '0' + h % 10); 
  
  Disp.drawChar(99, drawY, '0' + m / 10);
  Disp.drawChar(108, drawY, '0' + m % 10);
  
  Disp.drawCircle(95,drawY + 4,1,blink);
  Disp.drawCircle(95,drawY+ 11,1,blink);
      

  // 4. Logika Transisi
  if ((Tmr - lsRn) > 5000 && y == 17) { s = 1; }
  
  if (y == 0 && s == 1) {
    Disp.clear();
    // Gunakan F() macro sudah benar untuk hemat RAM
    Serial.printf_P(PSTR("TIME:%d,%d,%d,%d\n"), h, m, now.Second(), now.DayOfWeek());
    s = 0;
    show = ANIM_CLOCK;//ANIM_DATE;
  }
  
  DoSwap = true;
}

void animasi2(){
   static uint16_t   x; 
    static uint16_t fullScroll = 0;
    if(adzan) return;
    if (reset_x !=0) { x=0; reset_x = 0; fullScroll = 0;}      

    uint32_t          Tmr = millis();
    static uint32_t lss=0;
    RtcDateTime now = Rtc.GetDateTime();
    
    fType(1);
    uint16_t w = Disp.textWidth(showTanggal());
    fullScroll = w + DWidth;
      
    if((Tmr-lss)> 45)
      { lss=Tmr;
        if (x < fullScroll) {++x;}
        else {
          show = ANIM_TEXT1;
          x = 0; 
          fullScroll = 0;
          return;}
     Disp.drawText(DWidth - x, 9, showTanggal()); //runing teks diatas
        
        if (x<=6)                     { fType(1); dwCtr(32, x-6, TGLMASEHI());}
        else if (x>=(fullScroll-6))   { fType(1); dwCtr(32, (fullScroll-x)-6, TGLMASEHI());}
        else                          { fType(1); dwCtr(32,0,TGLMASEHI());}//posisi jamnya yang bawah

        if (x<=16)                    { Center(0,x-16); }
        else if (x>=(fullScroll-16))   { Center(0,(fullScroll-x)-16); }
        else                          { Center(0,0); }//posisi jamnya yang bawah
      
        DoSwap = true;
      }
}

void animasi3(){
  static uint16_t   x; 
    static uint16_t fullScroll = 0;
    if(adzan) return;
    if (reset_x !=0) { x=0; reset_x = 0; fullScroll = 0;}      

    uint32_t          Tmr = millis();
    static uint32_t lss=0;
    RtcDateTime now = Rtc.GetDateTime();
    
    fType(1);
    uint16_t w = Disp.textWidth(config.name);

    fullScroll = w + DWidth;
      
    if((Tmr-lss)> config.speedName)
      { lss=Tmr;
        if (x < fullScroll) {++x;}
        else {
          show = ANIM_CLOCK;
          x = 0; 
          fullScroll = 0;
          return;}
         Disp.drawText(DWidth - x, 0, config.name); //runing teks diatas
        
        if (x<=6)                     {fType(1); dwCtr(32, 16-x, TGLMASEHI());}
        else if (x>=(fullScroll-6))   {fType(1); dwCtr(32, 16-(fullScroll-x), TGLMASEHI());}
        else                          {fType(1); dwCtr(32,9,TGLMASEHI());}//posisi jamnya yang bawah

        if (x<=16)                     {Center(0, 16-x); }
        else if (x>=(fullScroll-16))   {Center(0,16-(fullScroll-x));}
        else                           { Center(0,0); }//posisi jamnya yang bawah
      
      
        DoSwap = true;
      }
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

void runn(const char* msg, uint8_t speed, uint8_t fontt)
{
  if(adzan) return;
  
  
  static uint32_t x = 0;
  static uint32_t fullScroll = 0;
  static uint32_t lastMs = 0;

  if(reset_x){
    x = 0;
    fullScroll = 0;
    lastMs = 0;
    reset_x = 0;
  }
  
  // ====== Pesan baru → reset animasi ======

  fType(fontt);
  // ====== Pesan kosong → langsung lompat ======
  uint16_t w = Disp.textWidth(msg);
  if (w == 0) {
    //nextShowState();
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
    //nextShowState();
    return;
  }

  Disp.drawText(
    DWidth - x,
    (fontt == 5) ? 0 : 8,
    msg
  );
  DoSwap = true;
}

void logo1 (uint8_t x){
  if (adzan) return;
  static const uint8_t logo1[] PROGMEM = {
    16,16,
  0x00, 0x00, 0x0c, 0xc0, 0x0d, 0xc0, 0x19, 0xc1, 0x00, 0x03, 0x04, 0x0b, 0x4c, 0xdb, 0x9c, 0xdb, 0xbc, 0xdb, 0xfc, 0xdb, 0x6c, 0xdb, 0x0c, 0xdb, 0x0c, 0xdb, 0x0f, 0xfb, 0x07, 0x32, 0x00, 0x00

    //0x06, 0x60, 0x06, 0xe3, 0x0c, 0xe3, 0x00, 0x01, 0x02, 0x05, 0x06, 0x6d, 0x4e, 0x6d, 0x5e, 0x6d, 0x7e, 0x6d, 0x36, 0x6d, 0x06, 0x6d, 0x06, 0x6d, 0x06, 0x6d, 0x07, 0xfd, 0x03, 0x98, 0x00, 0x00
  };
  Disp.drawBitmap(x,0,logo1);
}

void logo2 (uint8_t x){
  if (adzan) return;
  static const uint8_t logo2[] PROGMEM = {
    16,16,
    0x00, 0x00, 0x13, 0x00, 0x1b, 0x00, 0x18, 0x38, 0x08, 0x2c, 0x0c, 0x78, 0x0d, 0xf0, 0x07, 0x00, 0x07, 0xff, 0x0c, 0x7c, 0x1d, 0xe0, 0x77, 0x80, 0xe3, 0x80, 0x83, 0x80, 0x01, 0x80, 0x00, 0x00
};
  Disp.drawBitmap(x,0,logo2);
}

void iconAllah(uint8_t x) {
  if (adzan) return;

  static const uint8_t bitmap_Allah_P10_32x16[] PROGMEM = {
    32, 16, // HEADER: Lebar 32 pixel, Tinggi 16 pixel

    // DATA PIXEL KALIGRAFI
    // Baris 0-3 
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x18, 0x18, 0x00,
    0x01, 0x3E, 0x3E, 0x80,
    0x03, 0x3E, 0x3E, 0xC0,

    // Baris 4-7 
    0x03, 0x3F, 0x3F, 0xC0,
    0x07, 0x7F, 0x7F, 0xE0,
    0x0F, 0xFF, 0xFF, 0xF0,
    0x0F, 0xFD, 0xFD, 0xF0,

    // Baris 8-11 
    0x1F, 0xF9, 0xF9, 0xF8,
    0x1F, 0xF1, 0xF1, 0xF8,
    0x1F, 0xE1, 0xE1, 0xF8,
    0x1F, 0xF3, 0xF3, 0xF8,

    // Baris 12-15 
    0x0F, 0xFF, 0xFF, 0xF0,
    0x07, 0xFE, 0xFE, 0xE0,
    0x03, 0xFC, 0xFC, 0xC0,
    0x00, 0x00, 0x00, 0x00
  };

  Disp.drawBitmap(x, 0, bitmap_Allah_P10_32x16); 
  
  DoSwap = true;
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
        DoSwap = true;
    }
    
    if ((Tmr - lsRn) > 1500 && (ct > limit))
    {
        show = ANIM_IQOMAH;
        ct = 0;
        Buzzer(0);
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
        show = ANIM_BLINK;
    }    
}

void blinkBlock()
{
    static uint32_t lsRn;
    static uint32_t lastMove = 0; 
    static uint16_t ct = 0;
    
    // Titik awal, kita mulai dari pojok kiri atas
    static int posX = 0;
    static int posY = 0;
    static int dirX = 1;  
    static int dirY = 1;  

    /* =============================================================
       BATAS PANTULAN UNTUK Disp.drawText (Rata Kiri Atas)
       Asumsi Lebar teks "00:00:00" = ~48 px, Tinggi teks = ~8 px
       Susunan 6 panel memanjang (192 x 16)
    ============================================================= */
    const int minX = 0;          // Mentok layar kiri
    const int maxX = 192 - 48;   // Mentok layar kanan (Lebar total - Lebar teks)
    const int minY = 0;          // Mentok layar atas
    const int maxY = 16 - 7;     // Mentok layar bawah (Tinggi total - Tinggi teks)

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











  
