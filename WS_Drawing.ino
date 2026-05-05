
//=================== new variabel ========================//
char * const pasar[]  = {"WAGE", "KLIWON", "LEGI", "PAHING", "PON"}; 
char * const Hari[]  = {"MINGGU","SENIN","SELASA","RABU","KAMIS","JUM'AT","SABTU"};
//const char * const bulanMasehi[] PROGMEM = {"JANUARI", "FEBRUARI", "MARET", "APRIL", "MEI", "JUNI", "JULI", "AGUSTUS", "SEPTEMBER", "OKTOBER", "NOVEMBER", "DESEMBER" };
char* jadwal[] = {"IMSAK","SUBUH", "TERBT", "DUHUR", "ASHAR", "MAGRB", "ISYA'"};
char* jadwalAzzan[] = {"SUBUH","DZUHUR", "ASHAR", "MAGRIB", "ISYA'"};
char * namaBulanHijriah[] = {
    "MUHARRAM", "SHAFAR", "RABIUL AWAL",
    "RABIUL AKHIR", "JUMADIL AWAL", 
    "JUMADIL AKHIR", "RAJAB",
    "SYA'BAN", "RAMADHAN", "SYAWAL",
    "DZULQA'DAH", "DZULHIJAH"
};

struct SholatAnim {
  uint8_t  phase;      // IN / HOLD / OUT
  uint8_t  sNum;       // index sholat
  uint8_t  x;          // posisi animasi
  uint32_t timer;
};

struct MasehiAnim {
  uint8_t  phase;      // IN / HOLD / OUT
  uint8_t  sNum;       // index sholat
  uint8_t  x;          // posisi animasi
  uint32_t timer;
};

SholatAnim shAnim;
MasehiAnim msAnim;

#define SHOLAT_COUNT 7
#define PHASE_IN     0
#define PHASE_HOLD   1
#define PHASE_OUT    2

void initAnimSholat() {
  shAnim.phase = PHASE_IN;
  shAnim.sNum  = 0;
  shAnim.x     = 0;
  shAnim.timer = millis();
  Disp.clear();
}

void updateAnimSholat() {

  if(adzan) return;

  uint32_t now = millis();
  constexpr uint8_t center = 8;   // 32x16 panel
  
  if(reset_x) {
    shAnim.phase = PHASE_IN;
    shAnim.sNum  = 0;
    shAnim.x     = 0;
    now = 0;
    shAnim.timer = 0;
    reset_x = 0;
  }
  RtcDateTime noww = Rtc.GetDateTime();

  switch (shAnim.phase) {

    // ====== MASUK ======
    case PHASE_IN:
      if (now - shAnim.timer > 50) {
        shAnim.timer = now;
        if (shAnim.x < center) shAnim.x++;
        else shAnim.phase = PHASE_HOLD;
      }
      break;

    // ====== TAHAN ======
    case PHASE_HOLD:
      if (now - shAnim.timer > 2000) {
        shAnim.phase = PHASE_OUT;
      }
      break;

    // ====== KELUAR ======
    case PHASE_OUT:
      if (now - shAnim.timer > 50) {
        shAnim.timer = now;
        if (shAnim.x > 0) shAnim.x--;
        else {
          shAnim.sNum++;
          if (shAnim.sNum >= SHOLAT_COUNT) {
            line = ANIM_MASEHI;
            Serial.println("TIME:" + String(noww.Hour()) + "," + String(noww.Minute()) + "," + String(noww.Second()) + "," + String(noww.DayOfWeek()));
            shAnim.sNum=0;
            return;
          }
          shAnim.phase = PHASE_IN;
        }
      }
      break;
  }

  drawSholatFrame(shAnim.sNum, shAnim.x-center);
}
uint16_t sholatSec[5];   // waktu sholat (detik)
//===================== convert jam & menit ke detik =============================//
inline uint32_t toSecond(uint8_t h, uint8_t m, uint8_t s = 0)
{
  return (uint32_t)h * 3600UL + (uint32_t)m * 60UL + s;
}

void drawSholatFrame(uint8_t sNum, int8_t x) {

  if (sNum >= SHOLAT_COUNT) return;

  float sholatT[]={JWS.floatImsak,JWS.floatSubuh,JWS.floatTerbit,JWS.floatDzuhur,JWS.floatAshar,JWS.floatMaghrib,JWS.floatIsya};

  // ===== SHOLAT =====
  float st = sholatT[sNum];
  uint8_t hh = (uint8_t)st;
  uint8_t mm = (uint8_t)((st - hh) * 60);

    if(sNum == 1){sholatSec[1] = toSecond(hh, mm); }
    else if(sNum == 3){sholatSec[2] = toSecond(hh, mm); }
    else if(sNum == 4){sholatSec[3] = toSecond(hh, mm); }
    else if(sNum == 5){sholatSec[4] = toSecond(hh, mm); }
    else if(sNum == 6){sholatSec[5] = toSecond(hh, mm); }

   //Serial.println("hh:"+String(hh)+" "+"mm:"+String(mm));
  char timeBuf[6];
  snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d", hh, mm);

  fType(1);
  Disp.drawText(33, x, jadwal[sNum]);
  Disp.drawText(65, x, timeBuf);

}



void initAnimMasehi() {
  msAnim.phase = PHASE_IN;
  msAnim.x     = 0;
  msAnim.timer = millis();
  Disp.clear();
}

void updateAnimUpDown(const char* msg) {

  if(adzan) return;

  uint32_t now = millis();
  const uint8_t center = 8;   // 32x16 panel
  
  if(reset_x) {
    msAnim.phase = PHASE_IN;
    msAnim.sNum  = 0;
    msAnim.x     = 0;
    reset_x = 0;
    now = 0;
  }

  switch (msAnim.phase) {

    // ====== MASUK ======
    case PHASE_IN:
      if (now - msAnim.timer > 50) {
        msAnim.timer = now;
        if (msAnim.x < center) msAnim.x++;
        else msAnim.phase = PHASE_HOLD;
      }
      break;

    // ====== TAHAN ======
    case PHASE_HOLD:
      if (now - msAnim.timer > 2000) {
        msAnim.phase = PHASE_OUT;
      }
      break;

    // ====== KELUAR ======
    case PHASE_OUT:
      if (now - msAnim.timer > 50) {
        msAnim.timer = now;
        if (msAnim.x > 0) msAnim.x--;
        else {
          if(line == ANIM_MASEHI){ line = ANIM_DAY_NASIONAL; }
          else if(line == ANIM_DAY_NASIONAL){ line = ANIM_DAY_PASARAN; }
          else if(line == ANIM_DAY_PASARAN){ line = ANIM_SHOLAT; }
          msAnim.phase = PHASE_IN;
          return;
        }
      }
      break;
  }

  fType(1);
  dwCtr(32,msAnim.x-center,msg);
 
}


//==================== animasi jam dan running text =================//
void Center(int8_t y){
  if(adzan) return;

  RtcDateTime now = Rtc.GetDateTime();
    
  Disp.drawFilledRect(0, 0, 31, 16, 0);
  Disp.drawLine(31,0,31,16,0);
  
  if(now.Second() % 2 ){
      Disp.drawCircle(15,y + 4,1,1);
      Disp.drawCircle(15,y + 11,1,1);
    }else{
      Disp.drawCircle(15,y + 4,1,0);
      Disp.drawCircle(15,y + 11,1,0);
    }

  fType(3);
  
  Disp.drawChar(0, y, '0' + now.Hour() / 10);
  Disp.drawChar(7, y, '0' + now.Hour() % 10); 
  
  Disp.drawChar(18, y, '0' + now.Minute() / 10);
  Disp.drawChar(25, y, '0' + now.Minute() % 10);
  //DoSwap = true;
}

void jamCenter() {
  if (adzan) return;
  
  logo1(165);
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
  //uint8_t d = now.Second();
  bool blink = now.Second() % 2;
  
  blink?snprintf(jam,sizeof(jam),"%02d:%02d",h,m) : snprintf(jam,sizeof(jam),"%02d %02d",h,m);
  
  dwCtr(0,drawY,jam);
  
  // 4. Logika Transisi
  if ((Tmr - lsRn) > 5000 && y == 17) { s = 1; }
  
  if (y == 0 && s == 1) {
    Disp.clear();
    // Gunakan F() macro sudah benar untuk hemat RAM
    Serial.printf_P(PSTR("TIME:%d,%d,%d,%d\n"), h, m, now.Second(), now.DayOfWeek());
    s = 0;
    show = ANIM_DATE;
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
//    if (w == 0) {
//       nextShowState();
//       return;
//    }
    fullScroll = w + DWidth;
      
    if((Tmr-lss)> 45)
      { lss=Tmr;
        if (x < fullScroll) {++x;}
        else {
          
          x = 0; 
          fullScroll = 0;
          return;}
     Disp.drawText(DWidth - x, 9, showTanggal()); //runing teks diatas
        //fType(1);
        if (x<=6)                     { Center(x-6); fType(1); dwCtr(0, x-6, TGLMASEHI());}
        else if (x>=(fullScroll-6))   { Center((fullScroll-x)-6); fType(1); dwCtr(0, (fullScroll-x)-6, TGLMASEHI());}
        else                          { Center(0); fType(1); dwCtr(0,0,TGLMASEHI());}//posisi jamnya yang bawah
        //fType(1);  //Marquee    jam yang tampil di bawah
        
      
        DoSwap = true;
      }
}

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
    nextShowState();
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
    nextShowState();
    return;
  }

  Disp.drawText(
    DWidth - x,
    (fontt == 5) ? 0 : 8,
    msg
  );
  DoSwap = true;
}

void nextShowState()
{ 
  
  switch(show){
    case ANIM_BIGFONT: show = ANIM_DATE;  line = ANIM_SHOLAT; break;
    case ANIM_DATE:    show = ANIM_TEXT1; break;
    case ANIM_TEXT1:   show = ANIM_TEXT2; break;
    case ANIM_TEXT2:   show = ANIM_TEXT3; break;
    case ANIM_TEXT3:   show = ANIM_TEXT4; break;
    case ANIM_TEXT4:   show = ANIM_TEXT5; break;
    case ANIM_TEXT5:   show = ANIM_COUNTER; break;
    case ANIM_COUNTER:   show = ANIM_BIGFONT; line = ANIM_ZONK; reset_x = 1; break;
  }
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
          dwCtr(0, 0, "ADZAN");
          dwCtr(0, 8, sholat);
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
    char locBuff[10];  
    uint32_t now = millis();  // Simpan millis() di awal
    
    uint16_t cn_l = (iqomah[sholatNow] * 60);
    
    mnt = (cn_l - ct) / 60;
    scd = (cn_l - ct) % 60;
    snprintf(locBuff,sizeof(locBuff), "-%02d:%02d", mnt, scd);

   // if ((ct & 1) == 0) {  // Gunakan bitwise untuk optimasi modulo 2
    fType(1);
    dwCtr(0, 0, "MENUJU IQOMAH:");
    dwCtr(0, 8, locBuff);
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
    
    fType(0);
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
        show = ANIM_BIGFONT;
    }
}
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











  
