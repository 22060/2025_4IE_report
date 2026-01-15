#include <7080S.H>
#include "libmemes.h"

typedef unsigned short uint16_t;
typedef unsigned long  uint32_t;
typedef unsigned char  uint8_t;

typedef signed int _SINT;
typedef unsigned int _UINT;
typedef signed char _SBYTE;
// MIDI関連の定数
#define MIDI_NOTE_ON    0x90
#define MIDI_NOTE_OFF   0x80
#define MAX_MIDI_EVENTS 512  // 256から512に増加
#define TICKS_PER_QUARTER 480  // 標準的なMIDIのタイミング分解能


#define LCD_RS (PA.DR.BIT.B22)
#define LCD_E (PA.DR.BIT.B23)
#define LCD_RW (PD.DR.BIT.B23)
#define LCD_DATA (PD.DR.BYTE.HH)

#define DIG1 (PE.DR.BIT.B3)
#define DIG2 (PE.DR.BIT.B2)
#define DIG3 (PE.DR.BIT.B1)

// 音程と時間を格納する構造体
struct TONE {
    uint16_t frequency;  // 周波数 (Hz), 0は無音
    uint16_t duration;   // 持続時間 (ms)
};

// タイマー設定値を事前計算した構造体
struct TIMER_DATA {
    uint16_t tgra_sound;  // MTU21のTGRA値（音声用）
    uint16_t tgra_wait;   // MTU23のTGRA値（待機時間用）
    uint8_t is_sound;     // 0:無音, 1:音あり
};

// MIDIノート番号を周波数に変換するテーブル（A4=440Hz基準）
static const uint16_t midi_freq_table[128] = {
    // C-1からC9まで
    8, 9, 9, 10, 10, 11, 12, 12, 13, 14, 15, 15,
    16, 17, 18, 19, 21, 22, 23, 24, 26, 28, 29, 31,
    33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62,
    65, 69, 73, 78, 82, 87, 92, 98, 104, 110, 117, 123,
    131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247,
    262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,
    523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988,
    1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976,
    2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951,
    4186, 4435, 4699, 4978, 5274, 5588, 5920, 6272, 6645, 7040, 7459, 7902,
    8372, 8870, 9397, 9956, 10548, 11175, 11840, 12544
};

#define TFTDATA (*(volatile unsigned short *)0x08000000)
#define TFTCTRL (*(volatile unsigned short *)0x08000002)
#define _COL_WHITE (0xFFFF)
#define _COL_BLACK (0x0000)

// #define speed 8

// Frame buffer union for both 8-bit and 16-bit access
uint16_t framebuf[240 * 320];

static const uint8_t Font6x8[][6] = {
#include "font6x8.inc"   // フォント別ファイルを後述
};


#define SW6 (PD.DR.BIT.B18)
#define SW5 (PD.DR.BIT.B17)
#define SW4 (PD.DR.BIT.B16)
#define LED6 (PE.DR.BIT.B11)
#define LED5 (PE.DR.BIT.B9)
#define LED_ON (0)
#define LED_OFF (1)


#define printf ((int (*)(const char *, ...))0x00007c7c)
#define scanf ((int (*)(const char *, ...))0x00007cb8)


#define SD_CD (PE.DR.BIT.B11)
#define SD_CS (PE.DR.BIT.B9)

#define SPK (PE.DR.BIT.B0)



// --------------------------------------------------
struct disk_t
{
    unsigned int First_sect_LBA;
    unsigned int BPB_BytesPerSec;
    unsigned int BPB_SecPerClus;
    unsigned int BPB_RsvdSecCnt;
    unsigned int BPB_NumFATs;
    unsigned int BPB_RootEntCnt;
    unsigned int BPB_FATSz16;
    unsigned int First_RDE_sect;
    unsigned int First_FAT_sect;
    unsigned int First_Data_sect;
} DISK;

struct file_t
{
    unsigned char Filename[12];
    unsigned int n;          // RDE の(ゼロから数えて) n 番目のファイル
    unsigned int attr;       // 属性
    unsigned int File_year;  // 年
    unsigned int File_month; // 月
    unsigned int File_date;  // 日
    unsigned int File_hour;  // 時
    unsigned int File_min;   // 分
    unsigned int File_sec;   // 秒
    unsigned int FstClusLO;
    unsigned int FileSize;
    uint16_t *Data; // ファイル本体
};

void TFT_draw_char(int x, int y, char c, uint16_t color);
void TFT_draw_string(int x, int y, char *str, uint16_t color);
void show_file_info(struct file_t *file);
void show_all_files();
int find_and_load_file(struct file_t *target_file, char *target_filename);
int find_and_load_midi_file(struct file_t *target_file, char *target_filename);
int load_files_by_name();
void TFT_send_draw(uint16_t *fr,int count);
uint16_t hue999_to_rgb565(uint16_t h);
void spawn_enemy_bullet(int enemy_x, int enemy_y);
void LCD_inst(_SBYTE);
void LCD_data(_SBYTE);
void LCD_cursor(_UINT, _UINT);
void LCD_putch(_SBYTE);
void LCD_putstr(_SBYTE *);
void LCD_init(void);
void show7seg(int val, int num, int visible);

// ボス関連の関数プロトタイプ
void init_boss();
void spawn_boss();
void update_boss();
void draw_boss();
void check_skill_boss_collision();
void boss_shoot_bullets();

// ウルト関連の関数プロトタイプ
void update_ult_gauge();
void activate_ult();
void update_ult();
void draw_ult_gauge();
void check_ult_input();
void init_beam();
void draw_beam();
void check_beam_collision();

// MIDI関連の関数プロトタイプ
int parse_midi_to_tone(struct file_t *midi_file, struct TONE *tone_array);
uint32_t read_variable_length(uint8_t *data, int *offset);
uint16_t midi_note_to_freq(uint8_t note);
int process_midi_track(uint8_t *track_data, int track_length, struct TONE *tone_array);
void get_File_as_bytes(struct file_t *file);
void play_tone_sequence(struct TONE *tone_array, int count);
void convert_tone_to_timer_data(struct TONE *tone_array, int count, struct TIMER_DATA *timer_data);

// フィールド選択関数
int select_field();

// フィールド専用のファイルロード関数
void load_field_assets(int field_id);

// フィールド情報に基づいてゲームパラメータを調整する関数
void apply_field_modifiers();

//DMAC
void DMA0(void *SAR, void *DAR, uint8_t DM, uint8_t SM, int count);
void DMA1(void *SAR, void *DAR, uint8_t DM, uint8_t SM, int count);
void DMA2(void *SAR, void *DAR, uint8_t DM, uint8_t SM, int count);
void DMA3(void *SAR, void *DAR, uint8_t DM, uint8_t SM, int count);

// --------------------------------------------------
// -- グローバル変数 --
unsigned char dt[512];   // セクタリード用ワーク
unsigned char fat[2048]; // FAT
unsigned char rde[512];  // RDE

struct file_t mari;           // ファイル
struct file_t buttle;           // ファイル
struct file_t boss_img;           // ボス画像ファイル
struct file_t title_img;           // タイトル画像ファイル
// UNオーエンは彼女なのか
struct file_t un_file;
struct TONE un[MAX_MIDI_EVENTS];
struct TIMER_DATA un_data[MAX_MIDI_EVENTS];  // 事前計算済みタイマーデータ
uint16_t midiData[2048];  // MIDIファイル用（4KBまで対応）
// タイトル用music
struct file_t title_music_file;
struct TONE title_music[MAX_MIDI_EVENTS];
struct TIMER_DATA title_music_data[MAX_MIDI_EVENTS];  // 事前計算済みタイマーデータ
uint16_t titlemusicData[2048];  // MIDIファイル用（4KBまで対応）
uint16_t FileData0[1024*4]; // ファイルデータ本体
// uint16_t FileData1[320*180 + 4]; // ファイルデータ本体
uint16_t FileData3[320*240 + 4]; // ファイルデータ本体
uint16_t FileData2[64*64 + 4]; // ボス画像データ本体（64x64想定）
uint8_t DMAC0f = 0;
uint8_t DMAC1f = 0;
uint8_t DMAC2f = 0;
uint8_t DMAC3f = 0;
uint8_t music_playing = 0;

// MIDI音楽データを格納する配列
struct TONE music_data[MAX_MIDI_EVENTS];

// 障害物システム
#define MAX_OBSTACLES 8
struct obstacle_t {
    int x;
    int y;
    int width;
    int height;
    uint16_t color;
    int active;
    int velocity;
    int hp;
    int max_hp;
} obstacles[MAX_OBSTACLES];

int obstacle_spawn_timer = 0;
int obstacle_spawn_interval = 30; // フレーム数
int collision_count = 0; // 当たり判定カウント用変数

// ボスシステム
struct boss_t {
    int x;
    int y;
    int width;
    int height;
    uint16_t color;
    int active;
    int hp;
    int max_hp;
    int direction; // 移動方向 (1:下, -1:上)
    int velocity;
    int shoot_timer;
    int shoot_pattern;
    int invincible_timer; // 無敵時間
} boss;

int boss_spawn_timer = 0;
int boss_active = 0;

// スキルシステム（プレイヤーの弾）
#define MAX_SKILLS 10
struct skill_t {
    int x;
    int y;
    int width;
    int height;
    uint16_t color;
    int active;
    int velocity;
} skills[MAX_SKILLS];

// 敵の弾幕システム
#define MAX_ENEMY_BULLETS 20
struct enemy_bullet_t {
    int x;
    int y;
    int width;
    int height;
    uint16_t color;
    int active;
    int velocity_x;
    int velocity_y;
} enemy_bullets[MAX_ENEMY_BULLETS];

// ゲームシステム
int score = 0;
int game_level = 1;
int picx = 0;
int picy = 0;

// ウルトシステム
int ult_gauge = 0;          // ウルトゲージ（0-100）
int ult_max_gauge = 100;    // ゲージ最大値
int ult_active = 0;         // ウルト発動中フラグ
int ult_timer = 0;          // ウルト継続時間（フレーム数）
int ult_max_timer = 180;    // 3秒 = 60fps * 3
int ult_damage_total = 0;   // ウルト中の総ダメージ
int ult_available = 0;      // ウルト使用可能フラグ

// ビームシステム（ウルト攻撃用）
struct beam_t {
    int active;
    int x, y;
    int width, height;
    uint16_t color;
    int damage_per_frame;
} ult_beam;

// フィールド関連の定数とデータ構造
#define MAX_FIELDS 3
#define FIELD_FOREST 0
#define FIELD_DESERT 1
#define FIELD_SPACE 2

// フィールド情報構造体
struct FieldInfo {
    char name[16];
    char bg_file[16];    // 背景ファイル名
    int enemy_speed;     // 敵の移動速度倍率（%）
    int enemy_spawn_rate; // 敵の出現率倍率（%）
    int boss_hp_multiplier; // ボスHP倍率（%）
};

// フィールド情報テーブル
struct FieldInfo field_data[MAX_FIELDS] = {
    {"Forest Field", "FOREST  .IMG", 80, 100, 100},   // 森フィールド：敵が少し遅い
    {"Desert Field", "DESERT  .IMG", 100, 120, 120},  // 砂漠フィールド：標準
    {"Space Field",  "SPACE   .IMG", 120, 150, 150}   // 宇宙フィールド：敵が速く多い
};

// 現在のフィールド
int current_field = FIELD_FOREST;

// --------------------------------------------------
// --------------------------------------------------
int make1s = 9;
int count = 0;
int lastcount = 0;
int frag = 0;
int timing_frag = 0;
int tone_count;
int tone_count2;
int timing = 0;
#pragma interrupt INT_CMT1_CMI0
void INT_CMT1_CMI0()
{
	CMT1.CMCSR.BIT.CMF = 0;		// 割り込み要求をクリア

	// SPK ^= 1;
    lastcount = count;
    frag  = 1;
    count = 0;
    LED5 ^= 1;
    // printf("timing:%d\n", timing);
}
#pragma interrupt INT_MTU2_CMI1
void INT_MTU2_CMI1()
{
	MTU21.TSR.BIT.TGFA = 0;		// 割り込み要求をクリア
    // printf(".");

	SPK ^= 1;
}
#pragma interrupt INT_MTU23_CMI2
void INT_MTU23_CMI2()
{
	MTU23.TSR.BIT.TGFA = 0;		// 割り込み要求をクリア
    static uint16_t freq = 0,dur = 0,music4count=0,is_sound =0;

    // MTU2.TSTR.BIT.CST1 ^= 1;       // MTU2 CH1スタート
    // printf(".");
    make1s++;
    if(make1s >= 10){
        make1s = 0;
        timing++;
        MTU2.TSTR.BIT.CST1 = 0;  // タイマー停止
        MTU2.TSTR.BIT.CST3 = 0;  // タイマー開始
        if(music_playing == 0){
            freq = title_music_data[timing].tgra_sound;
            dur = title_music_data[timing].tgra_wait;
            music4count = tone_count2;
            is_sound = title_music_data[timing].is_sound;
        }else{
            freq = un_data[timing].tgra_sound;
            dur = un_data[timing].tgra_wait;
            music4count = tone_count;
            is_sound = un_data[timing].is_sound;
        }
        // printf("playing freq:%d,wait:%d\n",freq,dur);
        if (timing < music4count && is_sound) {
            // 音を再生する - 事前計算済みの値を使用
            MTU21.TGRA = freq;
            MTU21.TCNT = 0; // カウンタクリア
            
            // 持続時間 - 事前計算済みの値を使用
            MTU23.TGRA = dur;
            MTU23.TCNT = 0; // カウンタクリア
            MTU2.TSTR.BIT.CST1 = 1;  // タイマー開始
            MTU2.TSTR.BIT.CST3 = 1;  // タイマー開始
        } else if (timing < music4count) {
            // 無音期間 - 事前計算済みの値を使用
            MTU23.TGRA = dur;
            MTU23.TCNT = 0; // カウンタクリア
            MTU2.TSTR.BIT.CST3 = 1;  // タイマー開始
        }else{
            timing = 0;
            MTU23.TGRA = 1;
            MTU2.TSTR.BIT.CST3 = 1;  // タイマー開始
        }
    }
}
#pragma interrupt INT_MTU2_CMI3
void INT_MTU2_CMI3()
{
	MTU24.TSR.BIT.TGFA = 0;		// 割り込み要求をクリア
    static int num =0;
    static int val = 0;
    if(music_playing == 0){
            val = title_music[timing].frequency;
        }else{
            val = un[timing].frequency;
        }
    num++;
    if(num >= 3){
        num = 0;
    }
    switch(num){
        case 0:
            val = val % 10;
            break;
        case 1:
            val = (val / 10) % 10;
            break;
        case 2:
            val = (val / 100) % 10;
            break;
    }
    // printf("val:%d,keta:%d\n",val,keta);
    PA.DR.BYTE.HL &= 0xF0;
    if (num == 0)
    {
        DIG1 = 1;
        DIG2 = 0;
        DIG3 = 0;
    }
    else if (num == 1)
    {
        DIG1 = 0;
        DIG2 = 1;
        DIG3 = 0;
    }
    else
    {
        DIG1 = 0;
        DIG2 = 0;
        DIG3 = 1;
    }
    PA.DR.BYTE.HL |= val;
    // show7seg(val, keta, 0);
}
// int freq = 0,dur = 0;
void init_CMT0()
{
    STB.CR4.BIT._CMT = 0;

    CMT0.CMCNT = 0;
    CMT0.CMCSR.BIT.CKS = 1; // CKS設定(0:1/8, 1:1/32, 2:1/128, 3:1/512)
}
void init_MTU2(){
    
    STB.CR4.BIT._MTU2 = 0;
    STB.CR4.BIT._AD0 = 0;
    // MTU2 ch0
    MTU20.TCR.BIT.TPSC = 3;  // 1/64選択
    MTU20.TCR.BIT.CCLR = 1;  // TGRAのコンペアマッチでクリア
    MTU20.TGRA = 31250 - 1;  // 100ms
    MTU20.TIER.BIT.TTGE = 1; // A/D変換開始要求を許可
    INTC.IPRD.BIT._MTU20G = 9;	// 割り込み優先レベル = 11
    // AD0
    AD0.ADCSR.BIT.ADM = 3;   // cycleモード
    AD0.ADCSR.BIT.CH = 1;    // AN0,An1
    AD0.ADCSR.BIT.TRGE = 1;  // MTU2からのトリガ有効
    AD0.ADTSR.BIT.TRG0S = 1; // TGRAコンペアマッチでトリガ

    //MTU2 ch1
    MTU21.TCR.BIT.TPSC = 3;  // 1/64選択
    MTU21.TCR.BIT.CCLR = 1;  // TGRAのコンペアマッチでクリア
    MTU21.TGRA = 3125/20 - 1;  // 100ms
    MTU21.TIER.BIT.TGIEA = 1; // TGFAビットによる割り込み要求を許可
    INTC.IPRD.BIT._MTU21G = 11;	// 割り込み優先レベル = 10
    //MTU2 ch3
    MTU23.TCR.BIT.TPSC = 3;  // 1/64選択
    MTU23.TCR.BIT.CCLR = 1;  // TGRAのコンペアマッチでクリア
    MTU23.TGRA = 7812 - 1;  // 100ms
    MTU23.TIER.BIT.TGIEA = 1; // TGFAビットによる割り込み要求を許可
    INTC.IPRE.BIT._MTU23G = 10;	// 割り込み優先レベル = 9
    //MTU2 ch4
    MTU24.TCR.BIT.TPSC = 3;  // 1/64選択
    MTU24.TCR.BIT.CCLR = 1;  // TGRAのコンペアマッチでクリア
    MTU24.TGRA = 900 - 1;  // 100ms
    MTU24.TIER.BIT.TGIEA = 1; // TGFAビットによる割り込み要求を許可
    INTC.IPRF.BIT._MTU24G = 9;	// 割り込み優先レベル = 9
}
void init_CMT1(){
    STB.CR4.BIT._CMT = 0;		// CMTモジュールスタンバイの解除
    CMT1.CMCSR.BIT.CKS = 3;		// 1/512
    CMT1.CMCOR = 39062 - 1;		// 500ms
    CMT1.CMCSR.BIT.CMIE = 1;	// コンペアマッチ割り込み許可
    INTC.IPRJ.BIT._CMT1 = 8;	// 割り込み優先レベル = 8

    set_imask(7);			// マスクビット = 7
}
// void init_MTU2()
// {
//     STB.CR4.BIT._MTU2 = 0;
//     // MTU2 ch3
//     MTU23.TCR.BIT.TPSC = 2; // 1/16選択
//     MTU23.TCR.BIT.CCLR = 1; // TGRAのコンペアマッチでクリア
//     MTU23.TGRA = 1250 - 1;     // 1k hz
// }

// --------------------------------------------------
void wait_us(unsigned int us)
{
    unsigned int val;

    val = us * 10 / 16;
    if (val >= 0xffff)
        val = 0xffff;

    CMT0.CMCOR = val;
    CMT0.CMCSR.BIT.CMF &= 0;
    CMT.CMSTR.BIT.STR0 = 1;

    while (CMT0.CMCSR.BIT.CMF == 0)
        ;
    CMT0.CMCSR.BIT.CMF = 0;
    CMT.CMSTR.BIT.STR0 = 0;
}

// --------------------------------------------------
void init_SCI2()
{
    STB.CR3.BIT._SCI2 = 0;
    SCI2.SCSCR.BYTE = 0x00;
    SCI2.SCSMR.BIT.CA = 1;   // クロック同期式
    SCI2.SCBRR = 12;         // 384kbps
    SCI2.SCSDCR.BIT.DIR = 1; // MSB first
    wait_us(1);
    PFC.PECRL3.BIT.PE10MD = 2; // PE10 .. TxD
    PFC.PECRL3.BIT.PE8MD = 2;  // PE8 .. SCK
    PFC.PECRL2.BIT.PE7MD = 2;  // PE7 .. RxD
    PFC.PEIORL.BIT.B12 = 0;    // PE12 .. WP入力
    PFC.PEIORL.BIT.B11 = 0;    // PE11 .. CD入力
    PFC.PEIORL.BIT.B9 = 1;     // PE9 .. CS出力
    PE.DR.BIT.B9 = 1;          // CS初期値
    SCI2.SCSCR.BYTE |= 0x30;   // TE, RE = 1
}

// --------------------------------------------------
unsigned char SPI_tx_rx(unsigned char ch)
{
    while (!SCI2.SCSSR.BIT.TDRE)
        ;
    SCI2.SCTDR = ch;
    SCI2.SCSSR.BIT.TDRE = 0;

    while (!SCI2.SCSSR.BIT.RDRF)
        ;
    ch = SCI2.SCRDR;
    SCI2.SCSSR.BIT.RDRF = 0;
    return (ch);
}

// --------------------------------------------------
int card_exist()
{
    return (PE.DR.BIT.B11 ? 0 : 1);
}

// --------------------------------------------------
unsigned char calc_CRC7(unsigned char *data, int len)
{
    int i, j;
    char crc, dt;

    crc = 0;
    for (i = 0; i < len; i++)
    {
        dt = *data++;
        for (j = 0; j < 8; j++)
        {
            crc <<= 1;
            if ((crc & 0x80) ^ (dt & 0x80))
                crc ^= 0x09;
            dt <<= 1;
        }
    }
    return (crc & 0x7f);
}

// --------------------------------------------------
unsigned char SD_send_cmd(unsigned char cmd, int arg)
{
    int i;
    unsigned char cmd_token[6], ret;

    while (1)
    {
        if (SPI_tx_rx(0xff) == 0xff)
            break;
    }

    cmd_token[0] = cmd;
    cmd_token[1] = (arg >> 24) & 0xff;
    cmd_token[2] = (arg >> 16) & 0xff;
    cmd_token[3] = (arg >> 8) & 0xff;
    cmd_token[4] = arg & 0xff;
    cmd_token[5] = (calc_CRC7(cmd_token, 5) << 1) | 0x01;

    for (i = 0; i < 6; i++)
        SPI_tx_rx(cmd_token[i]);

    do
    {
        ret = SPI_tx_rx(0xff);
    } while (ret & 0x80);

    return (ret);
}

// --------------------------------------------------
int Enter_SPI_mode()
{
    int i;
    unsigned char ret;

    SD_CS = 1; // CS negate

    for (i = 0; i < 10; i++)
        SPI_tx_rx(0xff);

    SD_CS = 0; // CS assert

    ret = SD_send_cmd(0x40, 0);
    if (ret != 0x01)
        return (-1);

    do
    {
        ret = SD_send_cmd(0x41, 0);
    } while (ret != 0x00);

    return (1);
}

// --------------------------------------------------

void read_sector(unsigned int sect, unsigned char *dt)
{
    unsigned char ch;
    int i;

    ch = SD_send_cmd(0x51, sect * 512);
    while (ch != 0xfe)
        ch = SPI_tx_rx(0xff);

    for (i = 0; i < 512; i++)
        *dt++ = SPI_tx_rx(0xff);
    SPI_tx_rx(0xff); // CRC分
    SPI_tx_rx(0xff); // CRC分
}

// --------------------------------------------------
void print_sector(unsigned int sect, unsigned char *dt)
{
    int i, j;
    unsigned int adr;

    adr = sect * 512;

    printf("           ");
    for (i = 0; i < 16; i++)
    {
        printf("+%x ", i);
        if (i == 7)
            printf(" ");
    }
    printf("\n");

    for (i = 0; i < 32; i++)
    {
        printf("%08x : ", adr);
        for (j = 0; j < 16; j++)
        {
            printf("%02x ", dt[i * 16 + j]);
            if (j == 7)
                printf(" ");
        }
        printf(" ");
        for (j = 0; j < 16; j++)
        {
            if (isprint(dt[i * 16 + j]))
                printf("%c", dt[i * 16 + j]);
            else
                printf(".");
            if (j == 7)
                printf(" ");
        }
        printf("\n");
        adr += 16;
    }
}

// --------------------------------------------------
// --------------------------------------------------
// MBR を解析する
//
void ex_MBR()
{
    // -- 演習１ --
    read_sector(0, dt);
    // printf("dt[454]=0x%02x dt[455]=0x%02x dt[456]=0x%02x dt[457]=0x%02x\n", dt[454], dt[455], dt[456], dt[457]);
    DISK.First_sect_LBA = dt[454] | (dt[455] << 8) | (dt[456] << 16) | (dt[457] << 24);
}

// --------------------------------------------------
// BPB を解析する
//
void ex_BPB()
{
    // -- 演習２ --
    read_sector(DISK.First_sect_LBA, dt);
    DISK.BPB_BytesPerSec = dt[11] | (dt[12] << 8);
    DISK.BPB_SecPerClus = dt[13];
    DISK.BPB_RsvdSecCnt = dt[14] | (dt[15] << 8);
    DISK.BPB_NumFATs = dt[16];
    DISK.BPB_RootEntCnt = dt[17] | (dt[18] << 8);
    DISK.BPB_FATSz16 = dt[22] | (dt[23] << 8);
}

// --------------------------------------------------
// RDE を取得
//
void get_RDE(int n)
{
    int i;
    // -- 演習３ --
    DISK.First_RDE_sect = DISK.First_sect_LBA + DISK.BPB_RsvdSecCnt + (DISK.BPB_NumFATs * DISK.BPB_FATSz16);
    read_sector(DISK.First_RDE_sect + n, rde);
    // print_sector(DISK.First_RDE_sect + n, rde);
}

// --------------------------------------------------
// RDE の n 番目のファイル情報を取得
//
int get_file_info(struct file_t *file)
{
    // -- 演習４ --
    int i;
    unsigned int offset;

    offset = file->n * 32;
    if (rde[offset] == 0xe5 && rde[offset + 1] == 0x4a)
    { // 削除ファイル
        return (-1);
    }
    if (rde[offset] == 0x00)
    { // 空きエントリ
        file->Filename[0] = 0x00;
        return (0);
    }

    // -- ファイル名 --
    for (i = 0; i < 8; i++)
        file->Filename[i] = rde[offset + i];

    file->Filename[8] = '.';

    // -- 拡張子 --
    for (i = 8; i < 11; i++)
        file->Filename[i + 1] = rde[offset + i];

    // -- ここから編集 --
    file->attr = rde[offset + 11];
    file->File_hour = (rde[offset + 15] >> 3) & 0x1f;
    file->File_min = ((rde[offset + 15] & 0x07) << 3) + (rde[offset + 14] >> 5);
    file->File_sec = (rde[offset + 14] & 0x1f) * 2;
    file->File_year = ((rde[offset + 17] >> 1) & 0x7f) + 1980;
    file->File_month = ((rde[offset + 16] >> 5) & 0x07) + (((rde[offset + 17]) & 0x01) * 8);
    file->File_date = rde[offset + 16] & 0x1f;
    file->FstClusLO = rde[offset + 26] | (rde[offset + 27] << 8);
    file->FileSize = rde[offset + 28] | (rde[offset + 29] << 8) | (rde[offset + 30] << 16) | (rde[offset + 31] << 24);
    return (1);
}

// --------------------------------------------------
// FAT を取得
// サイズが大きいファイルに対応するため、FAT は 2 セクタ分取得しておく
//
void get_FAT()
{
    int i;
    // -- 演習５ --
    DISK.First_FAT_sect = DISK.First_sect_LBA + DISK.BPB_RsvdSecCnt;
    read_sector(DISK.First_FAT_sect, fat);
    read_sector(DISK.First_FAT_sect + 1, &fat[512]);
    read_sector(DISK.First_FAT_sect + 2, &fat[1024]);
    read_sector(DISK.First_FAT_sect + 3, &fat[1536]);
}

// --------------------------------------------------
// クラスタチェーンを表示
//
void ex_FAT(struct file_t *file)
{
    // -- 演習６ --
    unsigned int cluster;

    cluster = file->FstClusLO;
    printf("%d ", cluster);
    while (cluster != 0xffff)
    {
        cluster = fat[cluster * 2] | (fat[cluster * 2 + 1] << 8);
        printf(" -> %d ", cluster);
    }
    printf("\n");
}

// --------------------------------------------------
// ファイルを取得
//
void get_File(struct file_t *file)
{
    // -- 演習７ --
    int i, j;
    unsigned int sz;
    unsigned int cluster;
    unsigned int sz_16bit = 0; // 16bitデータのインデックス

    DISK.First_Data_sect = DISK.First_sect_LBA + DISK.BPB_RsvdSecCnt + (DISK.BPB_NumFATs * DISK.BPB_FATSz16) + ((DISK.BPB_RootEntCnt * 32 + (DISK.BPB_BytesPerSec - 1)) / DISK.BPB_BytesPerSec);
    cluster = file->FstClusLO;
    // printf("First_Data_sect=0x%x\n", (DISK.First_Data_sect + (cluster - 2) * DISK.BPB_SecPerClus));
    sz = 0;
    while (cluster != 0xffff)
    {
        for (i = 0; i < DISK.BPB_SecPerClus; i++)
        {
            read_sector(DISK.First_Data_sect + (cluster - 2) * DISK.BPB_SecPerClus + i, dt);
            // if(cluster == 252){
                
            // printf("Reading sector: %u\n", DISK.First_Data_sect + (cluster - 2) * DISK.BPB_SecPerClus + i);
            // print_sector(DISK.First_Data_sect + (cluster - 2) * DISK.BPB_SecPerClus + i, dt);
            // }
            for (j = 0; j < 512; j += 2) // 2バイトずつ処理
            {
                if (sz + 1 < file->FileSize) {
                    // 2つのuint8_tを1つのuint16_tに結合（Little Endian）
                    file->Data[sz_16bit] = (dt[j+1] << 8) | dt[j];
                    sz_16bit++;
                    sz += 2;
                } else if (sz < file->FileSize) {
                    // 奇数バイトの場合、下位バイトのみ設定
                    file->Data[sz_16bit] = dt[j];
                    sz_16bit++;
                    sz++;
                }
                
                if (sz >= file->FileSize)
                {
                    printf("File read complete: %u bytes\n", sz);
                    break;
                }
            }
            if (sz >= file->FileSize){
                printf("File read complete: %u bytes\n", sz);
                break;
            }
        }
        if (sz >= file->FileSize)
            break;
        cluster = fat[cluster * 2] | (fat[cluster * 2 + 1] << 8);
        printf("Next cluster=%d\n", cluster);
    }
}

// --------------------------------------------------
// ファイルをバイト単位で取得（MIDI等のバイナリファイル用）
//
void get_File_as_bytes(struct file_t *file)
{
    int i, j;
    unsigned int sz;
    unsigned int cluster;

    DISK.First_Data_sect = DISK.First_sect_LBA + DISK.BPB_RsvdSecCnt + (DISK.BPB_NumFATs * DISK.BPB_FATSz16) + ((DISK.BPB_RootEntCnt * 32 + (DISK.BPB_BytesPerSec - 1)) / DISK.BPB_BytesPerSec);
    cluster = file->FstClusLO;
    sz = 0;
    
    uint8_t *byte_data = (uint8_t*)file->Data;
    
    while (cluster != 0xffff)
    {
        for (i = 0; i < DISK.BPB_SecPerClus; i++)
        {
            read_sector(DISK.First_Data_sect + (cluster - 2) * DISK.BPB_SecPerClus + i, dt);
            for (j = 0; j < 512; j++)
            {
                if (sz < file->FileSize) {
                    byte_data[sz] = dt[j];
                    sz++;
                } else {
                    break;
                }
            }
            if (sz >= file->FileSize)
                break;
        }
        if (sz >= file->FileSize)
            break;
        cluster = fat[cluster * 2] | (fat[cluster * 2 + 1] << 8);
    }
}

// --------------------------------------------------
// ファイル内容を表示
//
void print_File(struct file_t *file)
{
    int i;

    for (i = 0; i < file->FileSize; i++)
        printf("%c", file->Data[i]);
    printf("\n");
}

// void TFT_clear(void)
// {
//     static uint16_t white = 0xFFFF;
//     if(DMAC1f){
//         while(DMAC1.CHCR.BIT.TE == 0)
//             ;
//         DMAC1.CHCR.BIT.DE = 0;
//     }else{
//         DMAC1f = 1;
//     }
//     TFTCTRL = 0x4001;
//     // DMAC1動作停止
//     DMAC1.CHCR.BIT.DE = 0;

//     // DMAC1設定
//     DMAC1.SAR = (void*)&white;   // ソースアドレス
//     DMAC1.DAR = (void *)0x08000000;           // デスティネーションアドレス
//     DMAC1.DMATCR = 320*240 - 1; // 転送カウント(16bit単位)

//     // printf("DMAC1 Transfer Start\n");

//     //CHCRの設定
//     DMAC1.CHCR.BIT.DM = 0;    //デスティネーションアドレス固定
//     DMAC1.CHCR.BIT.SM = 0;    //ソースアドレスインクリメント
//     DMAC1.CHCR.BIT.RS = 4;    //リソースセレクト4
//     DMAC1.CHCR.BIT.TS = 1;    //16bit転送
//     DMAC1.CHCR.BIT.TB = 0;    //サイクルモード
//     DMAC1.CHCR.BIT.IE = 0;    //割り込み禁止
//     DMAC1.CHCR.BIT.TE = 0;    //転送終了フラグクリア
//     DMAC1.CHCR.BIT.DE = 1;    //DMA有効化
// }
void TFT_On(void)
{
    TFTCTRL = 0x4000;
}
void init_CS2(void)
{
    BSC.CS2BCR.LONG = 0x12490400;
    BSC.CS2WCR = 0x000302C0;
    PFC.PACRL4.BIT.PA15MD = 1;
    PFC.PACRL2.BIT.PA6MD = 2;
}
void TFT_draw_pic(struct file_t *file,uint16_t x,uint8_t y)
{
    // .IMGファイルの場合、3バイト目が0なので簡単に取得可能
    uint8_t *raw_data = (uint8_t*)file->Data;
    int w = raw_data[0]  + (raw_data[1] << 8);  // width (1バイト目)
    int h = raw_data[3];  // height (2バイト目)
    // raw_data[2] = 0 (3バイト目は0固定)
    // for(int i = 0;i < 16;i++){
    //     printf("raw_data[%d]=0x%02x\n", i, raw_data[i]);
    // }
    
    uint16_t *data_ptr = file->Data + 1;  // 画像データの開始位置（4バイト目から）
    
    // 画像が画面外に完全にある場合は何もしない
    if (x >= 320 || y >= 240) return;
    
    // 描画範囲を画面内に制限
    int draw_h = h;
    int draw_w = w;
    
    if (y + h > 240) draw_h = 240 - y;
    if (x + w > 320) draw_w = 320 - x;
    // printf("Drawing image at (%d, %d) with size %dx%d\n", x, y, draw_w, draw_h);
    
    for (int i = 0; i < draw_h; i++)
    {
        for (int j = 0; j < draw_w; j++)
        {
            // フレームバッファの正しい位置を計算
            int pos = (y + i) * 320 + (x + j);
            if(*data_ptr != 0xFFFF) {
                // 透明色（白）なら描画しない
                framebuf[pos] = *data_ptr;
            }
            data_ptr++;  // 次のピクセルデータへ
        }
        
        // 画面外にはみ出した部分のデータをスキップ
        if (draw_w < w) {
            data_ptr += (w - draw_w);
        }
    }
}

// 超高速化バージョン（32bit単位での書き込み）
void TFT_fill_fast(uint16_t *buffer,uint16_t *color, int count)
{
    if(DMAC3f){
        while(DMAC3.CHCR.BIT.TE == 0)
            ;
        DMAC3.CHCR.BIT.DE = 0;
    }else{
        DMAC3f = 1;
    }
    // DMAC3設定
    DMAC3.SAR = (void*)color;   // ソースアドレス
    DMAC3.DAR = (void *)framebuf;           // デスティネーションアドレス
    DMAC3.DMATCR = count - 1; // 転送カウント(16bit単位)

    // printf("DMAC3\n");

    //CHCRの設定
    DMAC3.CHCR.BIT.DM = 1;    //デスティネーションアドレスインクリメント
    DMAC3.CHCR.BIT.SM = 0;    //ソースアドレス固定
    DMAC3.CHCR.BIT.RS = 4;    //リソースセレクト4
    DMAC3.CHCR.BIT.TS = 1;    //16bit転送
    DMAC3.CHCR.BIT.TB = 0;    //サイクルモード
    DMAC3.CHCR.BIT.IE = 0;    //割り込み禁止
    DMAC3.CHCR.BIT.TE = 0;    //転送終了フラグクリア
    DMAC3.CHCR.BIT.DE = 1;    //DMA有効化
    
    // DMA転送の完了を待つ
}




void TFT_draw_char(int x, int y, char ch, uint16_t color)
{
    int i, j;
    uint8_t line;
    unsigned char font_index;

    // ASCII 32(スペース)から127まで対応
    // フォント配列は32から始まるので、32を引く
    if (ch < 32 || ch > 127) {
        font_index = 0; // スペース文字を使用
    } else {
        font_index = ch - 32; // ASCII 32からの相対位置
    }

    // フォントデータは6列×8行で格納されている
    for (j = 0; j < 6; j++)  // 6列をループ
    {
        line = Font6x8[font_index][j];  // j列目のデータ取得
        for (i = 0; i < 8; i++)  // 8行をループ
        {
            if (line & (1 << i))  // i行目のビットをチェック
            {
                framebuf[(y + i) * 320 + (x + j)] = color;
            }
            else
            {
                framebuf[(y + i) * 320 + (x + j)] = _COL_WHITE;
            }
        }
    }
}
void TFT_draw_string(int x, int y, char *str, uint16_t color)
{
    while (*str)
    {
        TFT_draw_char(x, y, *str++, color);
        x += 6;
    }
}
// --------------------------------------------------
// セクタダンプ
//
// uint16_t h = 0;
// void dump_SECT()
// {
//     unsigned int s;
//     unsigned int n;
//     unsigned int cnt = 2; // RDE セクタ数
//     // uint16_t col = hue999_to_rgb565(h);
//     // h += 10;
//     // if(h >= 1000){
//     //     h = 0;
//     // }
//     // for(int n = 0;n < 320*60;n++){
//     //     framebuf[n] = (uint32_t)col;  // 下位16bitに色データ、上位16bitは0
//     // }
//     return;
//     int i = 0;
//     for (i = 0; i < 4096; i++)
//         File0.Data[i] = 0;

//     printf("\nfile shower(s,n) = ");
//     TFT_draw_string(0,0,"cnt:", _COL_BLACK);
//     TFT_draw_char(30,0,(char)(cnt + '0'), _COL_BLACK);
//     while(SW5 == 0)
//     {
//         if(SW4 == 1){
//             while(SW4 == 1)
//             {
//                 // 何もしない
//             }
//             cnt++;
//             if(cnt > 9){
//                 cnt = 0;
//             }
//             TFT_draw_char(30,0,(char)(cnt + '0'), _COL_BLACK);
//         }
//     }
//     s = 0;
//     n = 3 + cnt;
//     get_RDE(s);
//     File0.n = n; // RDE 3 番目のファイル情報
//     File0.Data = FileData0;
//     if (get_file_info(&File0) == 1)
//     {
//         for (i = 0; i < 12; i++)
//             printf("%c", File0.Filename[i]);
//         printf(" -> ");

//         get_File(&File0);
//         if (File0.Filename[8] == '.' && File0.Filename[9] == 'I' && File0.Filename[10] == 'M' && File0.Filename[11] == 'G')
//         {
//             TFT_draw_pic(&File0);
//         }
//         else
//             print_File(&File0);
//     }
//     else if (get_file_info(&File0) == -1)
//     {
//         printf("Deleted file\n");
//     }
//     else
//     {
//         printf("No file\n");
//     }
//     while(SW6 == 0)
//     {
//         // 何もしない
//     }

//     File0.n = 3;
//     show_file_info(&File0);
// }
// --------------------------------------------------
// show all files information
void show_all_files()
{
    struct file_t temp_file;
    temp_file.n = 3;  // n=0,1,2は予約領域なのでn=3から開始
    int i = 0;
    int t = 1;
    int status;
    printf("------------------------------------------------\n");
    while (status = get_file_info(&temp_file))
    {
        printf("s=%X ", i);
        printf("n=%d ", temp_file.n);
        if (status == -1)
        {
            printf("削除ファイル");
            // sprintf使用を避ける
            // TFT_draw_string(0,t*10, "DEL FILE", _COL_BLACK);
            t++;
            temp_file.n++;
        }
        else
        {
            for (int j = 0; j < 12; j++)
                printf("%c", temp_file.Filename[j]);
            printf(" / ");
            // printf("属性 = 0x%x\n", temp_file.attr);
            printf("Time = %02d:%02d:%02d / ", temp_file.File_hour, temp_file.File_min, temp_file.File_sec);
            printf("Date = %4d/%02d/%02d", temp_file.File_year, temp_file.File_month, temp_file.File_date);
            
            // sprintf使用を避けて、簡単な文字列表示
            // TFT_draw_string(16, t*10, temp_file.Filename, _COL_BLACK);
            // TFT_draw_char(0,t*10,(char)t + '0',_COL_BLACK);
            // printf("FileSize = %d\n", temp_file.FileSize);
            // printf("FstClusLO = %d\n", temp_file.FstClusLO);
            temp_file.n++;
            t++;
        }

        if (temp_file.n == 9 && i == 1)
        {
            temp_file.n++;
        }
        if (temp_file.n > (DISK.BPB_RootEntCnt / 32) - 1)
        {
            i++;
            get_RDE(i);
            temp_file.n = 0;
        }
        printf("\n");
        // ex_FAT(&temp_file);
    }
    // TFT_draw_string(160,230,"show", _COL_BLACK);
}

// show specific file information by filename
void show_file_info(struct file_t *file)
{
    printf("File Information:\n");
    printf("Filename: ");
    for (int j = 0; j < 12; j++)
        printf("%c", file->Filename[j]);
    printf("\n");
    printf("Attribute: 0x%x\n", file->attr);
    printf("Time: %02d:%02d:%02d\n", file->File_hour, file->File_min, file->File_sec);
    printf("Date: %4d/%02d/%02d\n", file->File_year, file->File_month, file->File_date);
    printf("FileSize: %d bytes\n", file->FileSize);
    printf("First Cluster: %d\n", file->FstClusLO);
    printf("------------------------------------------------\n");
}

// ファイル名を比較する関数（大文字小文字を考慮）
int strcmp_filename(char *str1, char *str2) {
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            return 0; // 不一致
        }
        i++;
    }
    return (str1[i] == '\0' && str2[i] == '\0') ? 1 : 0; // 一致の場合は1
}

// 指定したファイル名のファイルを見つけて読み込む関数
int find_and_load_file(struct file_t *target_file, char *target_filename) {
    struct file_t temp_file;
    temp_file.n = 3;  // n=0,1,2は予約領域なのでn=3から開始
    int status;
    int sector_num = 0;
    
    printf("Searching for file: %s\n", target_filename);
    
    get_RDE(sector_num);
    
    while (1) {
        status = get_file_info(&temp_file);
        printf("Status: %d, File number: %d\n", status, temp_file.n);
        if (status == 0) {
            // 空きエントリに到達
            if (temp_file.n > (DISK.BPB_RootEntCnt / 32) - 1) {
                sector_num++;
                get_RDE(sector_num);
                temp_file.n = 0;
                continue;
            } else {
                break; // ファイルが見つからない
            }
        }
        
        if (status == 1) {
            // 有効なファイルエントリ
            
            // ファイル名が一致するかチェック
            if (strcmp_filename(temp_file.Filename, target_filename)) {
                // ファイルが見つかった場合、target_fileにコピー
                // ファイル情報をコピー（Dataポインタは保持）
                uint16_t *saved_data = target_file->Data;
                *target_file = temp_file;
                target_file->Data = saved_data; // Dataポインタを復元
                
                // ファイル内容を読み込み
                printf("File found: %s, Size: %d bytes\n", target_filename, target_file->FileSize);
                get_File(target_file);
                
                return 1; // 成功
            }
        }
        
        temp_file.n++;
        
        if (temp_file.n > (DISK.BPB_RootEntCnt / 32) - 1) {
            sector_num++;
            get_RDE(sector_num);
            temp_file.n = 0; // 新しいセクタでは0から開始
        }
    }
    
    printf("File not found: %s\n", target_filename);
    return 0; // ファイルが見つからない
}

// MIDIファイル用の特別な読み込み関数
int find_and_load_midi_file(struct file_t *target_file, char *target_filename) {
    struct file_t temp_file;
    temp_file.n = 3;  // n=0,1,2は予約領域なのでn=3から開始
    int status;
    int sector_num = 0;
    
    printf("Searching for MIDI file: %s\n", target_filename);
    
    get_RDE(sector_num);
    
    while (1) {
        status = get_file_info(&temp_file);
        
        if (status == 0) {
            // 空きエントリに到達
            if (temp_file.n > (DISK.BPB_RootEntCnt / 32) - 1) {
                sector_num++;
                get_RDE(sector_num);
                temp_file.n = 0;
                continue;
            } else {
                break; // ファイルが見つからない
            }
        }
        
        if (status == 1) {
            // 有効なファイルエントリ
            
            // ファイル名が一致するかチェック
            if (strcmp_filename(temp_file.Filename, target_filename)) {
                // ファイルが見つかった場合、target_fileにコピー
                // ファイル情報をコピー（Dataポインタは保持）
                uint16_t *saved_data = target_file->Data;
                *target_file = temp_file;
                target_file->Data = saved_data; // Dataポインタを復元
                
                // MIDIファイルはバイト単位で読み込み
                get_File_as_bytes(target_file);
                
                return 1; // 成功
            }
        }
        
        temp_file.n++;
        
        if (temp_file.n > (DISK.BPB_RootEntCnt / 32) - 1) {
            sector_num++;
            get_RDE(sector_num);
            temp_file.n = 0; // 新しいセクタでは0から開始
        }
    }
    
    printf("MIDI file not found: %s\n", target_filename);
    return 0; // ファイルが見つからない
}

// 複数のファイルを名前で自動読み込み
int load_files_by_name() {
    int success_count = 0;
    
    printf("Auto-loading files by name...\n");
    
    // MARIO.IMGを読み込み
    mari.Data = FileData0;
    if (find_and_load_file(&mari, "MARI    .IMG")) {
        printf("MARI.IMG loaded\n");
        TFT_draw_string(0, 8, "MARI.IMG loaded", _COL_BLACK);
        success_count++;
    } else {
        printf("MARI.IMG not found\n");
        TFT_draw_string(0, 8, "MARI.IMG not found", _COL_BLACK);
    }
    TFT_send_draw(framebuf, 320 * 240);

    
    // BOSS.IMGを読み込み
    boss_img.Data = FileData2;
    if (find_and_load_file(&boss_img, "BOSS    .IMG")) {
        printf("BOSS.IMG loaded\n");
        TFT_draw_string(0, 24, "BOSS.IMG loaded", _COL_BLACK);
        success_count++;
    } else {
        printf("BOSS.IMG not found\n");
        TFT_draw_string(0, 24, "BOSS.IMG not found", _COL_BLACK);
    }
    TFT_send_draw(framebuf, 320 * 240);
    
    // MIDIファイルを読み込み（複数の名前を試行）
    un_file.Data = midiData;
    if (find_and_load_midi_file(&un_file, "UN      .MID")) {
        tone_count = parse_midi_to_tone(&un_file, un);
        // TONE配列からTIMER_DATA配列へ変換
        convert_tone_to_timer_data(un, tone_count, un_data);
        printf("MIDI file loaded\n");
        TFT_draw_string(0, 32, "MIDI file loaded", _COL_BLACK);
        success_count++;
    } else {
        printf("No MIDI file found\n");
        TFT_draw_string(0, 32, "No MIDI file found", _COL_BLACK);
    }
    TFT_send_draw(framebuf, 320 * 240);

    
    // struct file_t title_music_file;
    // struct TONE title_music[MAX_MIDI_EVENTS];
    // struct TIMER_DATA title_music_data[MAX_MIDI_EVENTS];  // 事前計算済みタイマーデータ
    // uint16_t titlemusicData[2048];  // MIDIファイル用（4KBまで対応）
    /*
    
struct TIMER_DATA {
    uint16_t tgra_sound;  // MTU21のTGRA値（音声用）
    uint16_t tgra_wait;   // MTU23のTGRA値（待機時間用）
    uint8_t is_sound;     // 0:無音, 1:音あり
};
    */
    title_music_file.Data = titlemusicData;
    if( find_and_load_midi_file(&title_music_file, "TITLE   .MID")) {
        tone_count2 = parse_midi_to_tone(&title_music_file, title_music);
        convert_tone_to_timer_data(title_music, tone_count2, title_music_data);
        // TONE配列からTIMER_DATA配列へ変換
        printf("TITLE.MID loaded\n");
        TFT_draw_string(0, 40, "TITLE.MID loaded", _COL_BLACK);
        success_count++;
    } else {
        printf("TITLE.MID not found\n");
        TFT_draw_string(0, 40, "TITLE.MID not found", _COL_BLACK);
    }
    TFT_send_draw(framebuf, 320 * 240);

    title_img.Data = FileData3;
    if (find_and_load_file(&title_img, "TITLE   .IMG")) {
        printf("TITLE.IMG loaded\n");
        TFT_draw_string(0, 48, "TITLE.IMG loaded", _COL_BLACK);
        success_count++;
    } else {
        printf("TITLE.IMG not found\n");
        TFT_draw_string(0, 48, "TITLE.IMG not found", _COL_BLACK);
    }
    TFT_send_draw(framebuf, 320 * 240);
    
    printf("Auto-loading completed: %d/5 files loaded\n", success_count);

    return success_count;
}

uint16_t hue999_to_rgb565(uint16_t h)
{
    // 0〜999 → 0〜359
    uint16_t hue = (h * 360) / 1000;

    uint8_t r = 0, g = 0, b = 0;

    uint16_t region = hue / 60;   // 0〜5
    uint16_t f = hue % 60;        // 0〜59

    uint8_t p = 0;
    uint8_t q = 255 - (255 * f) / 60;
    uint8_t t = (255 * f) / 60;

    switch (region) {
        case 0: r = 255; g = t;   b = p;   break; // 赤→黄
        case 1: r = q;   g = 255; b = p;   break; // 黄→緑
        case 2: r = p;   g = 255; b = t;   break; // 緑→水
        case 3: r = p;   g = q;   b = 255; break; // 水→青
        case 4: r = t;   g = p;   b = 255; break; // 青→紫
        default:r = 255; g = p;   b = q;   break; // 紫→赤
    }

    // RGB888 → RGB565
    return ((r & 0xF8) << 8)
         | ((g & 0xFC) << 3)
         | (b >> 3);
}

// 矩形描画関数（fillrect風）
void TFT_fill_rect(int x, int y, int width, int height, uint16_t color)
{
    int i, j;
    
    // 画面範囲チェック
    if (x < 0 || y < 0 || x >= 320 || y >= 180) return;
    if (x + width > 320) width = 320 - x;
    if (y + height > 180) height = 180 - y;
    
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            framebuf[(y + i) * 320 + (x + j)] = color;
        }
    }
}

// 障害物（敵）初期化
void init_obstacles()
{
    int i;
    for (i = 0; i < MAX_OBSTACLES; i++)
    {
        obstacles[i].active = 0;
        obstacles[i].x = 320;
        obstacles[i].y = 0;
        obstacles[i].width = 25;
        obstacles[i].height = 25;
        obstacles[i].velocity = 2;
        obstacles[i].color = 0xF800; // 赤色
        obstacles[i].hp = 3;
        obstacles[i].max_hp = 3;
    }
}

// 新しい敵を生成
void spawn_obstacle()
{
    int i;
    for (i = 0; i < MAX_OBSTACLES; i++)
    {
        if (!obstacles[i].active)
        {
            obstacles[i].active = 1;
            obstacles[i].x = 320;
            obstacles[i].y = 10 + (count * 37) % (160); // Y位置を制限
            obstacles[i].width = 20 + ((count * 17) % 15); // 幅20-34
            obstacles[i].height = 20 + ((count * 23) % 15); // 高さ20-34
            
            // フィールドの速度倍率を適用
            int base_velocity = 1 + ((count * 13) % 3); // 基本速度1-3
            obstacles[i].velocity = (base_velocity * field_data[current_field].enemy_speed) / 100;
            if (obstacles[i].velocity < 1) obstacles[i].velocity = 1; // 最低速度保証
            
            obstacles[i].max_hp = 2 + ((count * 11) % 4); // HP 2-5
            obstacles[i].hp = obstacles[i].max_hp;
            obstacles[i].color = hue999_to_rgb565((count * 150) % 1000); // カラフルな色
            break;
        }
    }
}

// 敵更新（弾幕発射機能付き）
void update_obstacles()
{
    int i;
    
    // 既存の敵を移動
    for (i = 0; i < MAX_OBSTACLES; i++)
    {
        if (obstacles[i].active)
        {
            obstacles[i].x -= obstacles[i].velocity;
            
            // 敵が弾を発射する（確率的に）
            if ((count + i * 7) % 40 == 0 && obstacles[i].x > 50 && obstacles[i].x < 300)
            {
                spawn_enemy_bullet(obstacles[i].x, obstacles[i].y + obstacles[i].height/2);
            }
            
            // 画面外に出たら非アクティブに
            if (obstacles[i].x + obstacles[i].width < 0)
            {
                obstacles[i].active = 0;
            }
        }
    }
    
    // 新しい敵を生成するタイミング
    obstacle_spawn_timer++;
    
    // フィールドのスポーン率倍率を適用
    int adjusted_spawn_interval = (obstacle_spawn_interval * 100) / field_data[current_field].enemy_spawn_rate;
    if (adjusted_spawn_interval < 10) adjusted_spawn_interval = 10; // 最低間隔保証
    
    if (obstacle_spawn_timer >= adjusted_spawn_interval)
    {
        spawn_obstacle();
        obstacle_spawn_timer = 0;
        
        // 生成間隔を少しずつ変化させる
        obstacle_spawn_interval = 40 + (count % 60);
    }
}

// 障害物描画（HPバー付き）
void draw_obstacles()
{
    int i;
    for (i = 0; i < MAX_OBSTACLES; i++)
    {
        if (obstacles[i].active)
        {
            // HPに応じて色を変える
            uint16_t base_color = obstacles[i].color;
            if (obstacles[i].hp < obstacles[i].max_hp)
            {
                // ダメージを受けた敵は少し暗くする
                base_color = (base_color >> 1) & 0x7BEF; // 明度を下げる
            }
            
            // 敵本体を描画
            TFT_fill_rect(obstacles[i].x, obstacles[i].y, 
                         obstacles[i].width, obstacles[i].height, 
                         base_color);
            
            // HPバーを描画（敵の上部）
            if (obstacles[i].max_hp > 1)
            {
                int hp_bar_width = (obstacles[i].width * obstacles[i].hp) / obstacles[i].max_hp;
                
                // HPバー背景（赤）
                TFT_fill_rect(obstacles[i].x, obstacles[i].y - 3, 
                             obstacles[i].width, 2, 0xF800);
                
                // HPバー（緑）
                if (hp_bar_width > 0)
                {
                    TFT_fill_rect(obstacles[i].x, obstacles[i].y - 3, 
                                 hp_bar_width, 2, 0x07E0);
                }
            }
        }
    }
}

// 当たり判定チェック関数
int check_collision(int player_x, int player_y, int player_w, int player_h, 
                   int obstacle_x, int obstacle_y, int obstacle_w, int obstacle_h)
{
    // 矩形同士の当たり判定
    if (player_x < obstacle_x + obstacle_w &&
        player_x + player_w > obstacle_x &&
        player_y < obstacle_y + obstacle_h &&
        player_y + player_h > obstacle_y)
    {
        return 1; // 当たっている
    }
    return 0; // 当たっていない
}

// プレイヤーと敵の当たり判定処理
void check_player_collision(int player_x, int player_y, int player_w, int player_h)
{
    int i;
    for (i = 0; i < MAX_OBSTACLES; i++)
    {
        if (obstacles[i].active)
        {
            if (check_collision(player_x + player_w/4, player_y + player_h/4, 
                              player_w/2, player_h/2, // プレイヤーの当たり判定を小さく
                              obstacles[i].x, obstacles[i].y, 
                              obstacles[i].width, obstacles[i].height))
            {
                // 当たり判定が発生
                collision_count += 5; // 敵との接触は大ダメージ
                obstacles[i].hp--; // 敵のHPを1減らす
                
                if (obstacles[i].hp <= 0)
                {
                    obstacles[i].active = 0; // HPが0になったら敵を削除
                    score += 5; // 敵を倒したボーナス
                    ult_gauge += 5; // ウルトゲージを5増加
                    if (ult_gauge >= ult_max_gauge) {
                        ult_gauge = ult_max_gauge;
                        ult_available = 1; // ウルト使用可能
                    }
                }
                
                break; // 一フレームで複数の当たり判定を防ぐ
            }
        }
    }
}

// スキル初期化
void init_skills()
{
    int i;
    for (i = 0; i < MAX_SKILLS; i++)
    {
        skills[i].active = 0;
        skills[i].x = 0;
        skills[i].y = 0;
        skills[i].width = 8;
        skills[i].height = 4;
        skills[i].velocity = 8;
        skills[i].color = 0x07FF; // シアン色
    }
}

// 新しいスキルを生成
void spawn_skill(int player_x, int player_y, int player_w, int player_h)
{
    int c = 0;
    int i;
    for (i = 0; i < MAX_SKILLS; i++)
    {
        if (!skills[i].active)
        {
            skills[i].active = 1;
            skills[i].x = player_x + player_w; // プレイヤーの右端から発射
            if(c == 0){
                skills[i].y = player_y + player_h / 2; // プレイヤーの中央高さ
            }else if(c == 1){
                skills[i].y = player_y + player_h / 2 - 10; // 少し上向き
            }else{
                skills[i].y = player_y + player_h / 2 + 10; // 少し下向き
            }
            skills[i].width = 8;
            skills[i].height = 4;
            skills[i].velocity = 20;
            skills[i].color = 0x07FF; // シアン色
            c++;
        }
        if (c >= 3) {
            break; // 最大3発まで同時に発射
        }
    }
}

// スキル更新
void update_skills()
{
    int i;
    
    // 既存のスキルを移動
    for (i = 0; i < MAX_SKILLS; i++)
    {
        if (skills[i].active)
        {
            skills[i].x += skills[i].velocity;
            
            // 画面外に出たら非アクティブに
            if (skills[i].x > 320)
            {
                skills[i].active = 0;
            }
        }
    }
}

// スキル描画
void draw_skills()
{
    int i;
    for (i = 0; i < MAX_SKILLS; i++)
    {
        if (skills[i].active)
        {
            TFT_fill_rect(skills[i].x, skills[i].y, 
                         skills[i].width, skills[i].height, 
                         skills[i].color);
        }
    }
}

// スキルと障害物の当たり判定処理
void check_skill_collision()
{
    int i, j;
    
    for (i = 0; i < MAX_SKILLS; i++)
    {
        if (skills[i].active)
        {
            for (j = 0; j < MAX_OBSTACLES; j++)
            {
                if (obstacles[j].active)
                {
                    if (check_collision(skills[i].x, skills[i].y, skills[i].width, skills[i].height,
                                      obstacles[j].x, obstacles[j].y, 
                                      obstacles[j].width, obstacles[j].height))
                    {
                        // スキルと敵が当たった
                        skills[i].active = 0;     // スキルを削除
                        obstacles[j].hp--; // 敵のHPを1減らす
                        
                        // ウルト中のダメージカウント
                        if (ult_active) {
                            ult_damage_total++; // ウルト中のダメージをカウント
                        }
                        
                        if (obstacles[j].hp <= 0)
                        {
                            obstacles[j].active = 0;  // HPが0になったら敵を削除
                            score += 10; // 敵を倒したスコア
                            ult_gauge += 10; // ウルトゲージを10増加
                            if (ult_gauge >= ult_max_gauge) {
                                ult_gauge = ult_max_gauge;
                                ult_available = 1; // ウルト使用可能
                            }
                        }
                        else
                        {
                            score += 2; // ダメージを与えたスコア
                        }
                        
                        break; // このスキルの処理を終了
                    }
                }
            }
        }
    }
}

// 敵の弾幕初期化
void init_enemy_bullets()
{
    int i;
    for (i = 0; i < MAX_ENEMY_BULLETS; i++)
    {
        enemy_bullets[i].active = 0;
        enemy_bullets[i].x = 0;
        enemy_bullets[i].y = 0;
        enemy_bullets[i].width = 4;
        enemy_bullets[i].height = 4;
        enemy_bullets[i].velocity_x = -4;
        enemy_bullets[i].velocity_y = 0;
        enemy_bullets[i].color = 0xFFE0; // 黄色
    }
}

// 敵の弾を生成
void spawn_enemy_bullet(int enemy_x, int enemy_y)
{
    int i;
    for (i = 0; i < MAX_ENEMY_BULLETS; i++)
    {
        if (!enemy_bullets[i].active)
        {
            enemy_bullets[i].active = 1;
            enemy_bullets[i].x = enemy_x;
            enemy_bullets[i].y = enemy_y;
            enemy_bullets[i].width = 4;
            enemy_bullets[i].height = 4;
            
            // プレイヤー方向に向かう弾の計算（簡易版）
            int dx = picx - enemy_x;
            int dy = picy - enemy_y;
            
            // 弾幕パターン：複数方向に発射
            if (i % 3 == 0) {
                enemy_bullets[i].velocity_x = -3;
                enemy_bullets[i].velocity_y = 0;
            } else if (i % 3 == 1) {
                enemy_bullets[i].velocity_x = -3;
                enemy_bullets[i].velocity_y = (dy > 0) ? 2 : -2;
            } else {
                enemy_bullets[i].velocity_x = -3;
                enemy_bullets[i].velocity_y = (dy > 0) ? -2 : 2;
            }
            
            enemy_bullets[i].color = hue999_to_rgb565((count * 50) % 1000);
            break;
        }
    }
}

// 敵の弾更新
void update_enemy_bullets()
{
    int i;
    
    for (i = 0; i < MAX_ENEMY_BULLETS; i++)
    {
        if (enemy_bullets[i].active)
        {
            enemy_bullets[i].x += enemy_bullets[i].velocity_x;
            enemy_bullets[i].y += enemy_bullets[i].velocity_y;
            
            // 画面外に出たら非アクティブに
            if (enemy_bullets[i].x < 0 || enemy_bullets[i].x > 320 ||
                enemy_bullets[i].y < 0 || enemy_bullets[i].y > 180)
            {
                enemy_bullets[i].active = 0;
            }
        }
    }
}

// 敵の弾描画
void draw_enemy_bullets()
{
    int i;
    for (i = 0; i < MAX_ENEMY_BULLETS; i++)
    {
        if (enemy_bullets[i].active)
        {
            TFT_fill_rect(enemy_bullets[i].x, enemy_bullets[i].y, 
                         enemy_bullets[i].width, enemy_bullets[i].height, 
                         enemy_bullets[i].color);
        }
    }
}

// プレイヤーと敵の弾の当たり判定
void check_player_bullet_collision(int player_x, int player_y, int player_w, int player_h)
{
    int i;
    for (i = 0; i < MAX_ENEMY_BULLETS; i++)
    {
        if (enemy_bullets[i].active)
        {
            if (check_collision(player_x + player_w/4, player_y + player_h/4, 
                              player_w/2, player_h/2, // プレイヤーの当たり判定を小さく
                              enemy_bullets[i].x, enemy_bullets[i].y, 
                              enemy_bullets[i].width, enemy_bullets[i].height))
            {
                // プレイヤーと敵の弾が当たった
                enemy_bullets[i].active = 0; // 弾を削除
                collision_count++; // ダメージカウント
                
                // ダメージ演出（プレイヤーを一瞬白く）
                break; // 一フレームで複数の当たり判定を防ぐ
            }
        }
    }
}

// --------------------------------------------------
// ボス敵システム
// --------------------------------------------------

// ボス初期化
void init_boss()
{
    boss.active = 0;
    boss.x = 280;
    boss.y = 40;
    boss.width = 35;
    boss.height = 40;
    boss.color = 0xF81F; // マゼンタ
    
    // フィールドのボスHP倍率を適用
    int base_hp = 1000;
    boss.hp = (base_hp * field_data[current_field].boss_hp_multiplier) / 100;
    boss.max_hp = boss.hp;
    
    boss.direction = 1; // 初期は下向き
    boss.velocity = 1;
    boss.shoot_timer = 0;
    boss.shoot_pattern = 0;
    boss.invincible_timer = 0;
    boss_active = 0;
}

// ボス生成
void spawn_boss()
{
    if (!boss.active && score > 200) { // スコア200以上でボス出現
        // MTU2.TSTR.BIT.CST3 = 1;       // MTU2 CH3スタート
        boss.active = 1;
        boss.x = 280;
        boss.y = 40;
        boss.hp = boss.max_hp;
        boss.direction = 1;
        boss.shoot_timer = 0;
        boss.invincible_timer = 0;
        boss_active = 1;
    }
}

// ボス更新
void update_boss()
{
    if (!boss.active) return;
    
    // 無敵時間を減らす
    if (boss.invincible_timer > 0) {
        boss.invincible_timer--;
    }
    
    // 上下移動
    boss.y += boss.direction * boss.velocity;
    
    // 画面端で反転
    if (boss.y <= 0) {
        boss.y = 0;
        boss.direction = 1; // 下向きに
    }
    if (boss.y + boss.height >= 160) {
        boss.y = 160 - boss.height;
        boss.direction = -1; // 上向きに
    }
    
    // 弾発射
    boss_shoot_bullets();
    
    // HPが0になったら消去
    if (boss.hp <= 0) {
        boss.active = 0;
        boss_active = 0;
        score += 1000; // ボス撃破ボーナス
    }
}

// ボス描画
void draw_boss()
{
    if (!boss.active) return;
    
    // ボス画像を描画
    if (boss_img.Data != 0) {
        // 無敵時間中のエフェクト処理
        if (boss.invincible_timer > 0 && (boss.invincible_timer % 4 < 2)) {
            // 無敵時間中は画像の色調を変更（簡易的に白っぽくする）
            // 元の画像データを一時的に変更してから描画
            uint8_t *raw_data = (uint8_t*)boss_img.Data;
            int w = raw_data[0] + (raw_data[1] << 8);  // width
            int h = raw_data[3];  // height
            uint16_t *data_ptr = boss_img.Data + 1;  // 画像データの開始位置
            
            // 簡易的な白色フラッシュ効果：一時的に明度を上げる
            TFT_draw_pic(&boss_img, boss.x, boss.y);
            // 白いオーバーレイを半透明風に描画
            for (int i = 0; i < h; i += 2) {
                for (int j = 0; j < w; j += 2) {
                    int pos = (boss.y + i) * 320 + (boss.x + j);
                    if (pos < 320*240) {
                        framebuf[pos] = 0xFFFF; // 白いドット
                    }
                }
            }
        } else {
            // 通常の描画
            TFT_draw_pic(&boss_img, boss.x, boss.y);
        }
    } else {
        // 画像が読み込まれていない場合は従来の矩形描画
        uint16_t draw_color = boss.color;
        if (boss.invincible_timer > 0 && (boss.invincible_timer % 4 < 2)) {
            draw_color = 0xFFFF; // 白く点滅
        }
        TFT_fill_rect(boss.x, boss.y, boss.width, boss.height, draw_color);
    }
    
    // HPバーを描画（ボスの上部）
    int hp_bar_width = (boss.width * boss.hp) / boss.max_hp;
    
    // HPバー背景（赤）
    TFT_fill_rect(boss.x, boss.y - 5, boss.width, 3, 0xF800);
    
    // HPバー（緑→黄→赤と変化）
    if (hp_bar_width > 0) {
        uint16_t hp_color;
        if (boss.hp > boss.max_hp * 2 / 3) {
            hp_color = 0x07E0; // 緑
        } else if (boss.hp > boss.max_hp / 3) {
            hp_color = 0xFFE0; // 黄
        } else {
            hp_color = 0xF800; // 赤
        }
        
        TFT_fill_rect(boss.x, boss.y - 5, hp_bar_width, 3, hp_color);
    }
}

// ボスの弾発射
void boss_shoot_bullets()
{
    if (!boss.active) return;
    
    boss.shoot_timer++;
    
    // 弾幕パターン1: 直線発射
    if (boss.shoot_timer % 15 == 0) {
        spawn_enemy_bullet(boss.x, boss.y + boss.height/2);
    }
    
    // 弾幕パターン2: 3方向発射
    if (boss.shoot_timer % 45 == 0) {
        spawn_enemy_bullet(boss.x, boss.y + boss.height/4);
        spawn_enemy_bullet(boss.x, boss.y + boss.height/2);
        spawn_enemy_bullet(boss.x, boss.y + boss.height*3/4);
    }
    
    // 弾幕パターン3: 扇状発射（HPが少ない時）
    if (boss.hp < boss.max_hp / 2 && boss.shoot_timer % 30 == 0) {
        for (int i = 0; i < 5; i++) {
            spawn_enemy_bullet(boss.x, boss.y + (boss.height * i) / 4);
        }
    }
}

// プレイヤーの弾とボスの当たり判定
void check_skill_boss_collision()
{
    if (!boss.active || boss.invincible_timer > 0) return;
    
    for (int i = 0; i < MAX_SKILLS; i++) {
        if (skills[i].active) {
            if (check_collision(skills[i].x, skills[i].y, skills[i].width, skills[i].height,
                              boss.x, boss.y, boss.width, boss.height)) {
                // スキルとボスが当たった
                skills[i].active = 0;     // スキルを削除
                boss.hp--;               // ボスのHPを1減らす
                boss.invincible_timer = 3; // 3フレーム無敵
                score += 5;              // ダメージスコア
                
                // ウルト中のダメージカウント
                if (ult_active) {
                    ult_damage_total++; // ウルト中のダメージをカウント
                }
                
                break; // このスキルの処理を終了
            }
        }
    }
}

// --------------------------------------------------
// ウルトシステム
// --------------------------------------------------

// ウルトゲージ更新
void update_ult_gauge()
{
    // 自動的にゲージが少しずつ減少（時間経過でゲージ減少）
    if (ult_gauge > 0 && !ult_active) {
        if (count % 300 == 0) { // 5秒に1回、1ずつ減少
            ult_gauge--;
        }
    }
}

// ウルト発動
void activate_ult()
{
    if (ult_available && !ult_active) {
        ult_active = 1;
        ult_timer = ult_max_timer;
        ult_damage_total = 0;
        ult_gauge = 0; // ゲージをリセット
        ult_available = 0;
        
        // ビーム発動
        ult_beam.active = 1;
        ult_beam.x = picx + 20; // プレイヤーの右端から
        ult_beam.y = picy;      // プレイヤーと同じ高さから
        ult_beam.width = 300;   // 画面右端まで
        ult_beam.height = 30;   // ビーム太さ
        ult_beam.color = 0x07FF; // シアン色
        ult_beam.damage_per_frame = 2; // フレーム当たりのダメージ
        
        // ウルト発動エフェクト（画面全体に一瞬エフェクト）
        for (int i = 0; i < 320*120; i++) {
            framebuf[i] = 0x07FF; // シアン色で画面フラッシュ
        }
    }
}

// ウルト状態更新
void update_ult()
{
    if (!ult_active) return;
    
    ult_timer--;
    
    // ビームの位置をプレイヤーに追従
    if (ult_beam.active) {
        ult_beam.x = picx + 20;
        ult_beam.y = picy;
        ult_beam.width = 300 - (picx + 20); // 画面右端までの幅を計算
        if (ult_beam.width < 0) ult_beam.width = 0;
    }
    
    // ウルト終了判定
    if (ult_timer <= 0) {
        ult_active = 0;
        ult_timer = 0;
        ult_beam.active = 0; // ビーム停止
        
        // ウルト終了時のボーナス判定（300ダメージ達成チェック）
        if (ult_damage_total >= 300) {
            score += 500; // 完璧ボーナス
        } else if (ult_damage_total >= 200) {
            score += 300; // 良いボーナス
        } else if (ult_damage_total >= 100) {
            score += 100; // 普通ボーナス
        }
        
        ult_damage_total = 0;
    }
}
void TFT_fill_rect2(int x, int y, int width, int height, uint16_t color)
{
    int i, j;
    
    // 画面範囲チェック
    if (x < 0 || y < 0 || x >= 320 || y >= 240) return;
    if (x + width > 320) width = 320 - x;
    if (y + height > 240) height = 240 - y;
    
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            framebuf[(y + i) * 320 + (x + j)] = color;
        }
    }
}
// ウルトゲージ描画（左下に配置）
void draw_ult_gauge()
{
    int gauge_x = 10;
    int gauge_y = 210;
    int gauge_width = 100;
    int gauge_height = 8;
    
    // ゲージ背景（黒）
    TFT_fill_rect2(gauge_x - 2, gauge_y - 2, gauge_width + 4, gauge_height + 4, _COL_BLACK);
    
    // ゲージ外枠（白）
    TFT_fill_rect2(gauge_x - 1, gauge_y - 1, gauge_width + 2, gauge_height + 2, _COL_WHITE);
    
    // ゲージ内部（グレー背景）
    TFT_fill_rect2(gauge_x, gauge_y, gauge_width, gauge_height, 0x8410);
    
    // ゲージ本体
    int fill_width = (gauge_width * ult_gauge) / ult_max_gauge;
    // printf("fill_width:%d\n",fill_width);
    if (fill_width > 0) {
        uint16_t gauge_color;
        if (ult_available) {
            gauge_color = 0xFFE0; // 黄色（使用可能）
        } else if (ult_gauge > 70) {
            gauge_color = 0x07E0; // 緑
        } else if (ult_gauge > 30) {
            gauge_color = 0xFD60; // オレンジ
        } else {
            gauge_color = 0xF800; // 赤
        }
        
        TFT_fill_rect2(gauge_x, gauge_y, fill_width, gauge_height, gauge_color);
    }
    
    // ウルトゲージラベル
    TFT_draw_string(gauge_x, gauge_y - 12, "ULT", _COL_BLACK);
    
    // ウルト発動中のインジケーター
    if (ult_active) {
        TFT_draw_string(gauge_x + 40, gauge_y - 12, "ACTIVE!", 0xF800);
        // 残り時間表示
        int remaining_sec = (ult_timer / 60) + 1;
        TFT_draw_char(gauge_x + 80, gauge_y - 12, remaining_sec + '0', 0xF800);
    } else if (ult_available) {
        TFT_draw_string(gauge_x + 40, gauge_y - 12, "READY!", 0xFFE0);
    }else{
        TFT_fill_rect2(gauge_x + 40, gauge_y - 12, 60, 8, _COL_WHITE); // 空白で消す
    }
    
    // ウルト中の総ダメージ表示
    if (ult_active && ult_damage_total > 0) {
        TFT_draw_string(gauge_x, gauge_y + 12, "DMG:", _COL_BLACK);
        TFT_draw_char(gauge_x + 24, gauge_y + 12, ((ult_damage_total/100) % 10) + '0', _COL_BLACK);
        TFT_draw_char(gauge_x + 30, gauge_y + 12, ((ult_damage_total/10) % 10) + '0', _COL_BLACK);
        TFT_draw_char(gauge_x + 36, gauge_y + 12, (ult_damage_total % 10) + '0', _COL_BLACK);
        TFT_draw_string(gauge_x + 42, gauge_y + 12, "/300", _COL_BLACK);
    }
}

// ウルト入力チェック
void check_ult_input()
{
    // SW6ボタンでウルト発動
    static int sw6_pressed = 0;
    
    if (SW6 == 1 && !sw6_pressed && ult_available) {
        activate_ult();
        sw6_pressed = 1;
    } else if (SW6 == 0) {
        sw6_pressed = 0;
    }
}

// ビーム初期化
void init_beam()
{
    ult_beam.active = 0;
    ult_beam.x = 0;
    ult_beam.y = 0;
    ult_beam.width = 0;
    ult_beam.height = 30;
    ult_beam.color = 0x07FF; // シアン色
    ult_beam.damage_per_frame = 2;
}

// ビーム描画
void draw_beam()
{
    static int green = 0x03E0;
    if (!ult_beam.active) return;
    
    // ビーム本体を描画（グラデーション効果）
    for (int i = 0; i < ult_beam.height; i++) {
        uint16_t beam_color;
        
        // 中心部は明るく、端は暗くするグラデーション効果
        if (i < ult_beam.height / 4 || i >= ult_beam.height * 3 / 4) {
            beam_color = 0x0318; // 暗いシアン
        } else if (i < ult_beam.height / 2 || i >= ult_beam.height / 2) {
            beam_color = 0x05FF; // 中間シアン
        } else {
            beam_color = 0x07FF; // 明るいシアン
        }
        
        // TFT_fill_rect(ult_beam.x, ult_beam.y + i, ult_beam.width, 1, beam_color);
        switch (i%4){
            case 0:
                DMA0((void*)&beam_color,(void*)&framebuf[(ult_beam.y + i)*320 + ult_beam.x],1,0, ult_beam.width);
                break;
            case 1:
                DMA1((void*)&beam_color,(void*)&framebuf[(ult_beam.y + i)*320 + ult_beam.x],1,0, ult_beam.width);
                break;
            case 2:
                DMA2((void*)&beam_color,(void*)&framebuf[(ult_beam.y + i)*320 + ult_beam.x],1,0, ult_beam.width);
                break;
            case 3:
                DMA3((void*)&beam_color,(void*)&framebuf[(ult_beam.y + i)*320 + ult_beam.x],1,0, ult_beam.width);
                break;
        }
    }
    
    // ビームエフェクト（点滅）
    if ((count % 4) < 2) {
        // 外側のオーラ効果
        // TFT_fill_rect(ult_beam.x, ult_beam.y - 2, ult_beam.width, 1, 0x03E0); // 薄い緑
        DMA2((void*)&green,(void*)&framebuf[(ult_beam.y - 2)*320 + ult_beam.x],1,0, ult_beam.width);
        // TFT_fill_rect(ult_beam.x, ult_beam.y + ult_beam.height + 1, ult_beam.width, 1, 0x03E0);
        DMA3((void*)&green,(void*)&framebuf[(ult_beam.y + ult_beam.height + 1)*320 + ult_beam.x],1,0, ult_beam.width);
    }
}

// ビームの当たり判定処理
void check_beam_collision()
{
    if (!ult_beam.active) return;
    
    // 敵との当たり判定
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (obstacles[i].active) {
            if (check_collision(ult_beam.x, ult_beam.y, ult_beam.width, ult_beam.height,
                              obstacles[i].x, obstacles[i].y, obstacles[i].width, obstacles[i].height)) {
                // ビームと敵が当たった - 毎フレームダメージ
                obstacles[i].hp -= ult_beam.damage_per_frame;
                ult_damage_total += ult_beam.damage_per_frame; // ウルト中のダメージをカウント
                
                if (obstacles[i].hp <= 0) {
                    obstacles[i].active = 0;  // HPが0になったら敵を削除
                    score += 20; // ビームで倒したボーナス
                } else {
                    score += ult_beam.damage_per_frame; // ダメージスコア
                }
            }
        }
    }
    
    // ボスとの当たり判定
    if (boss.active) {
        if (check_collision(ult_beam.x, ult_beam.y, ult_beam.width, ult_beam.height,
                          boss.x, boss.y, boss.width, boss.height)) {
            // ビームとボスが当たった - 毎フレームダメージ
            boss.hp -= ult_beam.damage_per_frame;
            boss.invincible_timer = 1; // 1フレーム無敵（ビームは連続ダメージ）
            ult_damage_total += ult_beam.damage_per_frame; // ウルト中のダメージをカウント
            score += ult_beam.damage_per_frame; // ダメージスコア
        }
    }
}

// MIDIノート番号を周波数に変換
uint16_t midi_note_to_freq(uint8_t note)
{
    if (note >= 128) return 0;
    return midi_freq_table[note];
}

// 可変長数値の読み取り
uint32_t read_variable_length(uint8_t *data, int *offset)
{
    uint32_t value = 0;
    uint8_t byte;
    
    do {
        byte = data[(*offset)++];
        value = (value << 7) | (byte & 0x7F);
    } while (byte & 0x80);
    
    return value;
}

// MIDIトラックデータを解析してTONE配列に変換
int process_midi_track(uint8_t *track_data, int track_length, struct TONE *tone_array)
{
    int offset = 0;
    int tone_count = 0;
    uint32_t current_time = 0;
    uint32_t last_note_end_time = 0;  // 最後の音符が終わった時刻
    uint8_t running_status = 0;
    int event_count = 0;
    
    // アクティブなノートを追跡する配列
    uint8_t active_notes[128];
    uint32_t note_start_time[128];
    int i;
    
    // 初期化
    for (i = 0; i < 128; i++) {
        active_notes[i] = 0;
        note_start_time[i] = 0;
    }
    
    while (offset < track_length && tone_count < MAX_MIDI_EVENTS - 1) {
        event_count++;
        
        // デルタタイム読み取り
        int delta_offset_start = offset;
        uint32_t delta_time = read_variable_length(track_data, &offset);
        current_time += delta_time;
        
        if (offset >= track_length) {
            break;
        }
        
        // イベントタイプ読み取り
        uint8_t event_byte = track_data[offset];
        
        // ランニングステータスの処理
        if (event_byte & 0x80) {
            running_status = event_byte;
            offset++;
        } else {
            // ランニングステータスを使用
            event_byte = running_status;
        }
        
        if (offset >= track_length) {
            break;
        }
        
        // ノートオンイベント
        if ((event_byte & 0xF0) == MIDI_NOTE_ON) {
            if (offset + 1 >= track_length) break;
            uint8_t note = track_data[offset++];
            uint8_t velocity = track_data[offset++];
            
            if (velocity > 0 && note < 128) {
                active_notes[note] = 1;
                note_start_time[note] = current_time;
            } else if (note < 128) {
                // ベロシティ0はノートオフと同じ
                if (active_notes[note]) {
                    active_notes[note] = 0;
                    uint32_t duration = current_time - note_start_time[note];
                    uint32_t note_end_time = current_time;
                    
                    // 前の音符との間に休符があるかチェック
                    if (last_note_end_time > 0 && note_start_time[note] > last_note_end_time) {
                        uint32_t rest_duration = note_start_time[note] - last_note_end_time;
                        uint32_t rest_ms = (rest_duration * 1000) / TICKS_PER_QUARTER;
                        
                        if (rest_ms > 50 && tone_count < MAX_MIDI_EVENTS - 1) {  // 50ms以上の休符のみ
                            tone_array[tone_count].frequency = 0;  // 無音
                            tone_array[tone_count].duration = rest_ms;
                            tone_count++;
                        }
                    }
                    
                    if (tone_count < MAX_MIDI_EVENTS) {
                        tone_array[tone_count].frequency = midi_note_to_freq(note);
                        tone_array[tone_count].duration = (duration * 1000) / TICKS_PER_QUARTER;  // ms変換
                        tone_count++;
                        last_note_end_time = note_end_time;  // 最後の音符終了時刻を更新
                    }
                }
            }
        }
        // ノートオフイベント
        else if ((event_byte & 0xF0) == MIDI_NOTE_OFF) {
            if (offset + 1 >= track_length) break;
            uint8_t note = track_data[offset++];
            uint8_t velocity = track_data[offset++];
            
            if (note < 128 && active_notes[note]) {
                active_notes[note] = 0;
                uint32_t duration = current_time - note_start_time[note];
                uint32_t note_end_time = current_time;
                
                // 前の音符との間に休符があるかチェック
                if (last_note_end_time > 0 && note_start_time[note] > last_note_end_time) {
                    uint32_t rest_duration = note_start_time[note] - last_note_end_time;
                    uint32_t rest_ms = (rest_duration * 1000) / TICKS_PER_QUARTER;
                    
                    if (rest_ms > 50 && tone_count < MAX_MIDI_EVENTS - 1) {  // 50ms以上の休符のみ
                        tone_array[tone_count].frequency = 0;  // 無音
                        tone_array[tone_count].duration = rest_ms;
                        tone_count++;
                    }
                }
                
                if (tone_count < MAX_MIDI_EVENTS) {
                    tone_array[tone_count].frequency = midi_note_to_freq(note);
                    tone_array[tone_count].duration = (duration * 1000) / TICKS_PER_QUARTER;  // ms変換
                    tone_count++;
                    last_note_end_time = note_end_time;  // 最後の音符終了時刻を更新
                }
            }
        }
        // その他のイベント（スキップ）
        else if ((event_byte & 0xF0) == 0xC0 || (event_byte & 0xF0) == 0xD0) {
            // プログラムチェンジ、チャンネルプレッシャー（1バイト）
            if (offset < track_length) offset++;
        }
        else if ((event_byte & 0xF0) >= 0x80 && (event_byte & 0xF0) <= 0xE0) {
            // その他の2バイトメッセージ
            if (offset + 1 < track_length) offset += 2;
        }
        else if (event_byte == 0xFF) {
            // メタイベント
            if (offset >= track_length) break;
            uint8_t meta_type = track_data[offset++];
            uint32_t length = read_variable_length(track_data, &offset);
            
            if (meta_type == 0x2F) {
                // End of Track
                break;
            }
            
            offset += length;
        }
        else if (event_byte == 0xF0 || event_byte == 0xF7) {
            // システムエクスクルーシブ
            uint32_t length = read_variable_length(track_data, &offset);
            offset += length;
        }
    }
    
    return tone_count;
}

// MIDIファイルをTONE配列に変換するメイン関数
int parse_midi_to_tone(struct file_t *midi_file, struct TONE *tone_array)
{
    uint8_t *data = (uint8_t*)midi_file->Data;
    
    // 最小ファイルサイズチェック
    if (midi_file->FileSize < 14) {
        return 0;
    }
    
    // MIDIヘッダーの確認
    if (data[0] != 'M' || data[1] != 'T' || data[2] != 'h' || data[3] != 'd') {
        return 0;
    }
    
    // ヘッダー長の確認（通常は6）
    uint32_t header_length = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];
    
    // ヘッダー情報の読み取り（ビッグエンディアン）
    uint16_t format_type = (data[8] << 8) | data[9];
    uint16_t track_count = (data[10] << 8) | data[11];
    uint16_t time_division = (data[12] << 8) | data[13];
    
    // 値の妥当性チェック
    if (format_type > 2) {
        return 0;
    }
    if (track_count > 100) {
        return 0;
    }
    if (time_division == 0 || time_division > 32767) {
        return 0;
    }
    
    int offset = 14;  // ヘッダー後の位置
    int total_tones = 0;
    
    // 全てのトラックを処理
    for (int track_num = 0; track_num < track_count && track_num < 2; track_num++) {
        
        if (offset + 8 >= midi_file->FileSize) {
            break;
        }
               
        // トラックヘッダーの確認
        if (data[offset] == 'M' && data[offset+1] == 'T' && 
            data[offset+2] == 'r' && data[offset+3] == 'k') {
            
            uint32_t track_length = (data[offset+4] << 24) | (data[offset+5] << 16) |
                                  (data[offset+6] << 8) | data[offset+7];
            
            offset += 8;  // トラックヘッダーをスキップ
            
            if (offset + track_length <= midi_file->FileSize) {
                int track_tones = process_midi_track(&data[offset], track_length, 
                                                   &tone_array[total_tones]);
                total_tones += track_tones;
                offset += track_length;  // 次のトラックへ
            } else {
                break;
            }
        } else {
            break;
        }
    }
    
    return total_tones;
}

// --------------------------------------------------
void init_DMAC(void){
    STB.CR2.BIT._DMAC = 0;  // 0で有効化
    DMAC.DMAOR.BIT.DME = 1; //DMAC有効化
    //DMAC0
    DMAC0.CHCR.BIT.RS = 4;    //リソースセレクト4
    DMAC0.CHCR.BIT.TS = 1;    //16bit転送
    DMAC0.CHCR.BIT.TB = 0;    //サイクルモード
    DMAC0.CHCR.BIT.IE = 0;    //割り込み禁止
    //DMAC1
    DMAC1.CHCR.BIT.RS = 4;    //リソースセレクト4
    DMAC1.CHCR.BIT.TS = 1;    //16bit転送
    DMAC1.CHCR.BIT.TB = 0;    //サイクルモード
    DMAC1.CHCR.BIT.IE = 0;    //割り込み禁止
    //DMAC2
    DMAC2.CHCR.BIT.RS = 4;    //リソースセレクト4
    DMAC2.CHCR.BIT.TS = 1;    //16bit転送
    DMAC2.CHCR.BIT.TB = 0;    //サイクルモード
    DMAC2.CHCR.BIT.IE = 0;    //割り込み禁止
    //DMAC3
    DMAC3.CHCR.BIT.RS = 4;    //リソースセレクト4
    DMAC3.CHCR.BIT.TS = 1;    //16bit転送
    DMAC3.CHCR.BIT.TB = 0;    //サイクルモード
    DMAC3.CHCR.BIT.IE = 0;    //割り込み禁止

}

void TFT_clear2(void){
    for(int i = 0;i < 240*320;i++){
        framebuf[i] = _COL_WHITE;
    }
}

// void TFT_draw_pic_dma(uint16_t * frame,uint16_t * pic,int count){
//     // printf("frag:%d\n", DMAC2f);
//     if(DMAC2f)
//     {
//         while(DMAC2.CHCR.BIT.TE == 0)
//             ;
//         DMAC2.CHCR.BIT.DE = 0;
//     }else{
//         DMAC2f = 1;
//     }

//     // DMAC2設定 - 既に16bitデータなので直接転送
//     DMAC2.SAR = (void*)pic;   // ソースアドレス
//     DMAC2.DAR = (void *)frame;           // デスティネーションアドレス
//     DMAC2.DMATCR = count - 1; // 転送カウント（16bit単位）

//     // printf("DMAC2 Transfer Start\n");

//     //CHCRの設定
//     DMAC2.CHCR.BIT.DM = 1;    //デスティネーションアドレスインクリメント
//     DMAC2.CHCR.BIT.SM = 1;    //ソースアドレスインクリメント
//     DMAC2.CHCR.BIT.RS = 4;    //リソースセレクト4
//     DMAC2.CHCR.BIT.TS = 1;    //16bit転送
//     DMAC2.CHCR.BIT.TB = 0;    //サイクルモード
//     DMAC2.CHCR.BIT.IE = 0;    //割り込み禁止
//     DMAC2.CHCR.BIT.TE = 0;    //転送終了フラグクリア
//     DMAC2.CHCR.BIT.DE = 1;    //DMA有効化
// }
/**
 * @brief DMAC0設定
 * 
 * @param DM ディスティネーションアドレス．転送先 0→固定,1→インクリメント
 * @param SM ソースアドレス．転送元．転送先 0→固定,1→インクリメント
 * @param count 転送回数
 */
void DMA0(void *SAR, void *DAR, uint8_t DM, uint8_t SM, int count){
    // DMAC0動作停止
    if(DMAC0f){
    while(DMAC0.CHCR.BIT.TE == 0)
        ;
    DMAC0.CHCR.BIT.DE = 0;

    }else{
        DMAC0f=1;
    }
    // DMAC0設定
    DMAC0.SAR = (void*)SAR;   // ソースアドレス
    DMAC0.DAR = (void *)DAR;           // デスティネーションアドレス
    DMAC0.DMATCR = count - 1; // 転送カウント(16bit単位)
    //CHCRの設定
    DMAC0.CHCR.BIT.DM = DM;    //デスティネーションアドレス固定
    DMAC0.CHCR.BIT.SM = SM;    //ソースアドレスインクリメント

    DMAC0.CHCR.BIT.TE = 0;    //転送終了フラグクリア
    DMAC0.CHCR.BIT.DE = 1;    //DMA有効化
}
/**
 * @brief DMAC1設定
 * 
 * @param DM ディスティネーションアドレス．転送先 0→固定,1→インクリメント
 * @param SM ソースアドレス．転送元．転送先 0→固定,1→インクリメント
 * @param count 転送回数
 */
void DMA1(void *SAR, void *DAR, uint8_t DM, uint8_t SM, int count){
    // DMAC0動作停止
    if(DMAC1f){
    while(DMAC1.CHCR.BIT.TE == 0)
        ;
    DMAC1.CHCR.BIT.DE = 0;

    }else{
        DMAC1f=1;
    }
    // DMAC0設定
    DMAC1.SAR = (void*)SAR;   // ソースアドレス
    DMAC1.DAR = (void *)DAR;           // デスティネーションアドレス
    DMAC1.DMATCR = count - 1; // 転送カウント(16bit単位)
    //CHCRの設定
    DMAC1.CHCR.BIT.DM = DM;    //デスティネーションアドレス固定
    DMAC1.CHCR.BIT.SM = SM;    //ソースアドレスインクリメント

    DMAC1.CHCR.BIT.TE = 0;    //転送終了フラグクリア
    DMAC1.CHCR.BIT.DE = 1;    //DMA有効化
}
/**
 * @brief DMAC2設定
 * 
 * @param DM ディスティネーションアドレス．転送先 0→固定,1→インクリメント
 * @param SM ソースアドレス．転送元．転送先 0→固定,1→インクリメント
 * @param count 転送回数
 */
void DMA2(void *SAR, void *DAR, uint8_t DM, uint8_t SM, int count){
    // DMAC0動作停止
    if(DMAC2f){
    while(DMAC2.CHCR.BIT.TE == 0)
        ;
    DMAC2.CHCR.BIT.DE = 0;

    }else{
        DMAC2f=1;
    }
    // DMAC0設定
    DMAC2.SAR = (void*)SAR;   // ソースアドレス
    DMAC2.DAR = (void *)DAR;           // デスティネーションアドレス
    DMAC2.DMATCR = count - 1; // 転送カウント(16bit単位)
    //CHCRの設定
    DMAC2.CHCR.BIT.DM = DM;    //デスティネーションアドレス固定
    DMAC2.CHCR.BIT.SM = SM;    //ソースアドレスインクリメント

    DMAC2.CHCR.BIT.TE = 0;    //転送終了フラグクリア
    DMAC2.CHCR.BIT.DE = 1;    //DMA有効化
}
/**
 * @brief DMAC3設定
 * 
 * @param DM ディスティネーションアドレス．転送先 0→固定,1→インクリメント
 * @param SM ソースアドレス．転送元．転送先 0→固定,1→インクリメント
 * @param count 転送回数
 */
void DMA3(void *SAR, void *DAR, uint8_t DM, uint8_t SM, int count){
    // DMAC0動作停止
    if(DMAC3f){
    while(DMAC3.CHCR.BIT.TE == 0)
        ;
    DMAC3.CHCR.BIT.DE = 0;

    }else{
        DMAC3f=1;
    }
    // DMAC0設定
    DMAC3.SAR = (void*)SAR;   // ソースアドレス
    DMAC3.DAR = (void *)DAR;           // デスティネーションアドレス
    DMAC3.DMATCR = count - 1; // 転送カウント(16bit単位)
    //CHCRの設定
    DMAC3.CHCR.BIT.DM = DM;    //デスティネーションアドレス固定
    DMAC3.CHCR.BIT.SM = SM;    //ソースアドレスインクリメント

    DMAC3.CHCR.BIT.TE = 0;    //転送終了フラグクリア
    DMAC3.CHCR.BIT.DE = 1;    //DMA有効化
}


void TFT_send_draw(uint16_t *fr,int count){
    TFTCTRL = 0x4001;
    DMA0((void *)fr,(void*) 0x08000000,0,1,count);
}
void LCD_init(void)
{
    wait_us(45000);
    LCD_inst(0x30);
    wait_us(4100);
    LCD_inst(0x30);
    wait_us(100);
    LCD_inst(0x30);

    LCD_inst(0x38);
    LCD_inst(0x08);
    LCD_inst(0x01);
    wait_us(1640);
    LCD_inst(0x06);
    LCD_inst(0x0c);
}
void LCD_cursor(_UINT x, _UINT y)
{
    if (x > 15)
        x = 15;
    if (y > 1)
        y = 1;
    LCD_inst(0x80 | x | y << 6);
}

void LCD_putch(_SBYTE ch)
{
    LCD_data(ch);
}

void LCD_putstr(_SBYTE *str)
{
    _SBYTE ch;

    while (ch = *str++)
        LCD_putch(ch);
}
void LCD_inst(_SBYTE inst)
{
    LCD_E = 0;
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_E = 1;
    LCD_DATA = inst;
    wait_us(1);
    LCD_E = 0;
    wait_us(40);
}

void LCD_data(_SBYTE data)
{
    LCD_E = 0;
    LCD_RS = 1;
    LCD_RW = 0;
    LCD_E = 1;
    LCD_DATA = data;
    wait_us(1);
    LCD_E = 0;
    wait_us(40);
}
void show7seg(int val, int num, int visible)
{
    // printf("val:%d,num:%d\n",val,num);
    PA.DR.BYTE.HL &= 0xF0;
    if (visible == 1)
    {
        DIG1 = 0;
        DIG2 = 0;
        DIG3 = 0;
        return; // 表示しない
    }
    if (num == 0)
    {
        DIG1 = 1;
        DIG2 = 0;
        DIG3 = 0;
    }
    else if (num == 1)
    {
        DIG1 = 0;
        DIG2 = 1;
        DIG3 = 0;
    }
    else
    {
        DIG1 = 0;
        DIG2 = 0;
        DIG3 = 1;
    }
    PA.DR.BYTE.HL |= val;
}
// --------------------------------------------------
void main()
{
    int i;
    int j;
    PFC.PAIORH.BYTE.L |= 0x0F;
    PFC.PEIORL.BIT.B3 = 1;   // 1 の位
    PFC.PEIORL.BIT.B2 = 1;   // 10 の位
    PFC.PEIORL.BIT.B1 = 1;   // 100 の位
    printf("initializing...\n");
    init_MTU2(); //ADC
    printf("MTU2 initialized.\n");
    init_CMT1(); //FPS counter
    printf("CMT1 initialized.\n");
    // while(1)
    // ;
    init_CMT0(); //wait_us
    printf("CMT0 initialized.\n");
    init_SCI2();
    printf("SCI2 initialized.\n");
    init_CS2();
    printf("CS2 initialized.\n");
    init_DMAC();
    printf("DMAC initialized.\n");
    LCD_init();
    printf("LCD initialized.\n");
    TFT_On();
    printf("TFT On.\n");
    // TFT_clear();
    TFT_clear2();
    PFC.PEIORL.BIT.B0 = 1;
    DMAC.DMAOR.BIT.DME = 1; //DMAC有効化

    DMAC0f = 0;
    DMAC1f = 0;
    DMAC2f = 0;
    DMAC3f = 0;

    if (0)
        printf("No card found\n");
    else
    {
        printf("Card found\n");
        if (Enter_SPI_mode() < 0){
            printf("SPI mode Err\n");
            TFT_draw_string(0, 0, "SPI mode Err", _COL_BLACK);
    TFTCTRL = 0x4001;
    DMA0((void *)framebuf,(void*) 0x08000000,0,1,320 * 240);
        }
        else
        {
            TFT_draw_string(0, 0, "Loading SD Card...", _COL_BLACK);
    TFTCTRL = 0x4001;
    DMA0((void *)framebuf,(void*) 0x08000000,0,1,320 * 240);
            printf("SPI mode\n");

            SD_send_cmd(0x50, 512); // ブロックサイズ=512
            ex_MBR();
            ex_BPB();
            get_FAT();
            get_RDE(0);
            mari.n = 3; // RDE 5 番目のファイル情報
            mari.Data = FileData0;
            show_all_files();
            get_RDE(0);
            printf("Loading files...\n");
            
            // 自動ファイル読み込み関数を使用
            int loaded_files = load_files_by_name();
            
            if (loaded_files == 0) {
                printf("No files could be loaded! Check file names.\n");
            }
            // TFT_draw_pic(&buttle,picx,picy);
            // printf("showing2\n");
            uint16_t white = _COL_WHITE;
            // TFT_clear2();
            // TFT_fill_fast(framebuf, &white, 320 * 240);
            // printf("showing\n");
                // while(DMAC2.CHCR.BIT.TE == 0)
                //     ;
            // while(1);


            // .IMGファイル用の簡略化されたサイズ取得
            printf("loaded\n");
            MTU2.TSTR.BIT.CST3 = 1;       // MTU2 CH3スタート
                music_playing = 0;
                // TFT_draw_pic_dma(framebuf,FileData3 + 1,320*240);  // 背景クリア
                TFT_clear2();
                DMA0((void *)FileData3,(void*) framebuf,1,1,320 * 120);
                // while(DMAC0.CHCR.BIT.TE == 0);
                DMA1((void *)(&FileData3[320*120]),(void*) (&framebuf[320*120]),1,1,320 * 120);
                while(DMAC1.CHCR.BIT.TE == 0 || DMAC0.CHCR.BIT.TE == 0)
                    ;
                TFTCTRL = 0x4001;
                DMA0((void *)framebuf,(void*) 0x08000000,0,1,320 * 240);
                LCD_cursor(0, 0);
                LCD_putstr("shooting game");
                LCD_cursor(0, 1);
                LCD_putstr("SW4 to start");
                MTU2.TSTR.BIT.CST4 = 1;       // MTU2 CH4スタート
                while(SW4 == 0)
                    ;
                LCD_cursor(0, 1);
                LCD_putstr("                ");
                buttle.Data = FileData3;
                if (find_and_load_file(&buttle, "BUTTLE  .IMG")) {
                    printf("BUTTLE.IMG loaded\n");
                } else {
                    printf("BUTTLE.IMG not found\n");
                }
                MTU2.TSTR.BIT.CST0 = 1;       // MTU2 CH0スタート //ADC
                CMT.CMSTR.BIT.STR1 = 1;		// CMT1スタート //1秒間隔
            
            // --------------------------------------------------
            while(1){
                uint8_t *raw_mari = (uint8_t*)mari.Data;
                int w = raw_mari[0] + (raw_mari[1] << 8);  // width (1バイト目)
                int h = raw_mari[3];  // height (2バイト目)
                int hue = 0;
                        while(SW4 == 1)
                            ;
                
                // フィールド選択画面
                // current_field = select_field();
                // printf("Selected field: %s\n", field_data[current_field].name);
                
                // // 選択されたフィールドのアセットを読み込み
                // load_field_assets(current_field);
                
                // // フィールドに応じたパラメータ調整
                // apply_field_modifiers();
                
                LCD_cursor(0, 0);
                LCD_putstr("shooting game");
                LCD_cursor(0, 1);
                LCD_putstr("SW4 to end game");
                TFT_draw_string(0,200,"Loading...",_COL_BLACK);
                TFTCTRL = 0x4001;
                DMA0((void *)framebuf,(void*) 0x08000000,0,1,320 * 240);
                // BUTTLE.IMGを読み込み
                TFT_clear2();
                init_obstacles(); // 敵初期化
                init_skills(); // プレイヤーの弾初期化
                init_enemy_bullets(); // 敵の弾初期化
                init_boss(); // ボス初期化
                init_beam(); // ビーム初期化
                uint16_t cd = 0;
                
                // プレイヤーを画面左端に配置
                picx = 10;
                picy = 60;
                j = 0;
                music_playing = 1;
                timing = 0;
                while (1){
                    DMA1((void *)FileData3,(void*) framebuf,1,1,320 * 60);
                    DMA2((void *)(&FileData3[320*60]),(void*) (&framebuf[320*60]),1,1,320 * 60);
                    DMA3((void *)(&FileData3[320*120]),(void*) (&framebuf[320*120]),1,1,320 * 60);
                    // printf("ジョイスティック\n");
                    // ジョイスティック制御（横型シューティング用）
                    if ((AD0.ADDR0 >> 6) < 400)
                    {
                        // -- ジョイスティック上 --
                        picy -= (20 - (AD0.ADDR0 >> 6) / 20) + 1;
                        // 画像の高さを考慮した境界設定
                        if(picy <= 0){
                            picy = 0;
                        }
                    }
                    else if ((AD0.ADDR0 >> 6) > 600)
                    {
                        // -- ジョイスティック下 --
                        picy += ((AD0.ADDR0 >> 6) - 600) / 20 + 1;
                        // 画像の高さを考慮した境界設定
                        if(picy >= (180 - h)){
                            picy = 180 - h;
                        }
                    }
                    if ((AD0.ADDR1 >> 6) < 400)
                    {
                        // -- ジョイスティック右 --
                        picx += (20 - (AD0.ADDR1 >> 6) / 20) + 1;
                        // 画面中央付近まで移動可能
                        if(picx >= 160){
                            picx = 160;
                        }
                    }
                    else if ((AD0.ADDR1 >> 6) > 600)
                    {
                        // -- ジョイスティック左 --
                        picx -= ((AD0.ADDR1 >> 6) - 600) / 20 + 1;
                        if(picx <= 0){
                            picx = 0;
                        }
                    }

                    // プレイヤーの弾発射（SW5ボタンで連射）
                    if (SW5 == 1 && cd >= 5) // 連射間隔を短く
                    {
                        spawn_skill(picx, picy, w, h);
                        cd = 0;
                    }
                    cd++;
                    // printf("ウルトシステム\n");
                    // ウルトシステム更新
                    update_ult_gauge(); // ゲージ自然減少
                    check_ult_input(); // ウルト発動入力チェック
                    update_ult(); // ウルト状態更新
                    
                    // TFT_draw_pic_dma(framebuf,FileData1,320*120);  // 背景クリア
                    // DMA1((void *)FileData1,(void*) framebuf,1,1,320 * 120);
                    // printf("画面描画\n");
                    // printf("\n");
                    // DMA1((void *)FileData3,(void*) framebuf + 320*120,1,1,320 * 120);
                    // while(DMAC1.CHCR.BIT.TE == 0)
                    //     ;
                    
                    
                    // ビームシステム
                    check_beam_collision(); // ビームの当たり判定
                    
                    // 敵システム
                    // printf("enem\n");
                    update_obstacles(); // 敵更新
                    
                    // プレイヤーの弾システム
                    // printf("skill\n");
                    update_skills(); // プレイヤーの弾更新
                    check_skill_collision(); // プレイヤーの弾と敵の当たり判定
                    
                    // 敵の弾幕システム
                    // printf("enem_skill\n");
                    // printf("1\n");
                    update_enemy_bullets(); // 敵の弾更新
                    // printf("2\n");
                    check_player_bullet_collision(picx, picy, w, h); // プレイヤーと敵の弾の当たり判定
                    // printf("3\n");
                    
                    // ボスシステム
                    // printf("boss\n");
                    spawn_boss(); // ボス生成チェック
                    update_boss(); // ボス更新
                    check_skill_boss_collision(); // プレイヤーの弾とボスの当たり判定
                    
                    // プレイヤーと敵の当たり判定
                    check_player_collision(picx, picy, w, h);
                    
                    
                    // UI表示
                    // プレイヤー描画
                    while(DMAC1.CHCR.BIT.TE == 0 || DMAC2.CHCR.BIT.TE == 0 ||DMAC3.CHCR.BIT.TE == 0){
                        // printf(".");
                    }
                    draw_beam(); // ビーム描画
                    TFT_draw_pic(&mari,picx,picy);
                    draw_obstacles(); // 敵描画
                    draw_skills(); // プレイヤーの弾描画
                    draw_enemy_bullets(); // 敵の弾描画
                    draw_boss(); // ボス描画
                    TFT_draw_char(0,0,(lastcount/10)%10 + '0',_COL_BLACK);
                    TFT_draw_char(6,0,lastcount%10 + '0',_COL_BLACK);
                    TFT_draw_string(12,0,"FPS",_COL_BLACK);
                    while(DMAC1.CHCR.BIT.TE == 0 || DMAC2.CHCR.BIT.TE == 0 ||DMAC3.CHCR.BIT.TE == 0){
                        // printf(".");
                    }
                    
                    if(frag){
                        frag = 0;
                        
                        // スコア表示（120px以下に配置）
                        TFT_draw_string(10,190,"SCORE:",_COL_BLACK);
                        TFT_draw_char(46,190,((score/1000)%10) + '0',_COL_BLACK);
                        TFT_draw_char(52,190,((score/100)%10) + '0',_COL_BLACK);
                        TFT_draw_char(58,190,((score/10)%10) + '0',_COL_BLACK);
                        TFT_draw_char(64,190,(score%10) + '0',_COL_BLACK);

                        // ダメージ表示（120px以下に配置）
                        TFT_draw_string(90,190,"DMG:",_COL_BLACK);
                        TFT_draw_char(116,190,(collision_count/10)%10 + '0',_COL_BLACK);
                        TFT_draw_char(122,190,collision_count%10 + '0',_COL_BLACK);

                        // フィールド名表示
                        TFT_draw_string(160,230,field_data[current_field].name,_COL_BLACK);
                        
                        
                        // ボスHP表示（ボスが出現している時のみ）
                        if (boss.active) {
                            TFT_draw_string(180,190,"BOSS:",_COL_BLACK);
                            TFT_draw_char(216,190,((boss.hp/1000)%10) + '0',_COL_BLACK);
                            TFT_draw_char(222,190,((boss.hp/100)%10) + '0',_COL_BLACK);
                            TFT_draw_char(228,190,((boss.hp/10)%10) + '0',_COL_BLACK);
                            TFT_draw_char(234,190,(boss.hp%10) + '0',_COL_BLACK);
                        }
                        
                        // ウルトゲージ描画（常に表示）
                        draw_ult_gauge();
                        TFTCTRL = 0x4001;
                        DMA0((void *)framebuf,(void*) 0x08000000,0,1,320 * 240);
                    }else{
                        TFTCTRL = 0x4001;
                        DMA0((void *)framebuf,(void*) 0x08000000,0,1,320 * 180);
                    }

                    
                    
                    count++;
                    if(timing_frag){
                            timing_frag = 0;
                    }
                    if(SW4 == 1){
                        while(SW4 == 1)
                            ;
                        music_playing = 0;
                        break; // フィールド選択へ戻る
                    }
                }
            }
        }
    }

    SD_CS = 1; // CS negate

    while (1)
        ;
}

// --------------------------------------------------
// MIDI音楽再生関数
//
void play_tone_sequence(struct TONE *tone_array, int count)
{
    return;
}

// --------------------------------------------------
// TONE配列からTIMER_DATA配列への変換関数
// MTU2のタイマー値を事前計算して格納
void convert_tone_to_timer_data(struct TONE *tone_array, int count, struct TIMER_DATA *timer_data)
{
    int i;
    
    for (i = 0; i < count; i++) {
        uint16_t frequency = tone_array[i].frequency;
        uint16_t duration = tone_array[i].duration;
        
        if (frequency > 0) {
            // 音ありの場合
            timer_data[i].is_sound = 1;
            
            // MTU21用の周波数設定値を事前計算
            // 周波数からTGRA値を計算: (31250 / frequency) * 5 - 1
            uint16_t timer_count = (31250 / frequency) * 5 - 1;
            timer_data[i].tgra_sound = timer_count;
            
            // MTU23用の待機時間設定値を事前計算
            // 持続時間からTGRA値を計算: 31250 / (2000 / duration) - 1
            uint32_t wait_count = 31250 / (2000 / duration) - 1;
            wait_count = wait_count > 65535 ? 65535 : wait_count;
            timer_data[i].tgra_wait = (uint16_t)wait_count;
        } else {
            // 無音の場合
            timer_data[i].is_sound = 0;
            timer_data[i].tgra_sound = 0;  // 使用しない
            
            // MTU23用の待機時間設定値のみ計算
            uint32_t wait_count = 31250 / (2000 / duration) - 1;
            wait_count = wait_count > 65535 ? 65535 : wait_count;
            timer_data[i].tgra_wait = (uint16_t)wait_count;
        }
    }
}

// フィールド選択関数
int select_field() {
    int selected = 0;
    int prev_sw4 = SW4;
    int prev_sw5 = SW5;
    int prev_sw6 = SW6;
    
    while (1) {
        // フィールド選択画面の描画
        TFT_clear2();
        TFT_draw_string(80, 20, "SELECT FIELD", _COL_BLACK);
        
        // フィールド情報の表示
        for (int i = 0; i < MAX_FIELDS; i++) {
            uint16_t color = (i == selected) ? 0xF800 : _COL_BLACK; // 選択中は赤色
            int y_pos = 60 + i * 40;
            
            if (i == selected) {
                TFT_draw_string(20, y_pos, ">", color);
            }
            TFT_draw_string(40, y_pos, field_data[i].name, color);
            
            // フィールド詳細情報
            TFT_draw_string(40, y_pos + 15, "Speed:", _COL_BLACK);
            TFT_draw_char(82, y_pos + 15, (field_data[i].enemy_speed / 100) + '0', _COL_BLACK);
            TFT_draw_char(88, y_pos + 15, (field_data[i].enemy_speed / 10) % 10 + '0', _COL_BLACK);
            TFT_draw_char(94, y_pos + 15, field_data[i].enemy_speed % 10 + '0', _COL_BLACK);
            TFT_draw_string(100, y_pos + 15, "%", _COL_BLACK);
        }
        
        TFT_draw_string(60, 200, "SW4:Up SW5:Down SW6:Select", _COL_BLACK);
        
        TFTCTRL = 0x4001;
        DMA0((void *)framebuf,(void*) 0x08000000,0,1,320 * 240);
        
        // ボタン入力処理（チャタリング対策あり）
        if (SW4 == 1 && prev_sw4 == 0) {
            selected = (selected - 1 + MAX_FIELDS) % MAX_FIELDS;
            wait_us(200); // 200ms待機
        }
        if (SW5 == 1 && prev_sw5 == 0) {
            selected = (selected + 1) % MAX_FIELDS;
            wait_us(200); // 200ms待機
        }
        if (SW6 == 1 && prev_sw6 == 0) {
            current_field = selected;
            wait_us(200); // 200ms待機
            break;
        }
        
        prev_sw4 = SW4;
        prev_sw5 = SW5;
        prev_sw6 = SW6;
        wait_us(50); // 50ms待機
    }
    
    return current_field;
}

// フィールド専用のファイルロード関数
void load_field_assets(int field_id) {
    // 背景ファイルをFileData3に読み込む
    struct file_t field_bg;
    field_bg.Data = FileData3;
    
    if (find_and_load_file(&field_bg, field_data[field_id].bg_file)) {
        printf("Field background loaded: %s\n", field_data[field_id].bg_file);
    } else {
        printf("Field background not found: %s, using default\n", field_data[field_id].bg_file);
        // デフォルト背景を使用
    }
}

// フィールド情報に基づいてゲームパラメータを調整する関数
void apply_field_modifiers() {
    // 敵の移動速度調整（グローバル変数があれば調整）
    // enemy_speed_modifier = field_data[current_field].enemy_speed;
    
    // 敵の出現率調整（グローバル変数があれば調整）
    // enemy_spawn_rate_modifier = field_data[current_field].enemy_spawn_rate;
    
    // ボスHP調整（ボス初期化時に使用）
    // boss_hp_modifier = field_data[current_field].boss_hp_multiplier;
}

// --------------------------------------------------
