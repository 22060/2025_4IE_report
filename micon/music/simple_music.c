/*
 * SH7085 シンプル音楽生成システム - MTU23直接制御版
 * 
 * 動作確認済みのシンプルなMTU23タイマー制御によるスピーカー駆動
 * 複雑な正弦波テーブルやサンプリングは使わず、直接的な方形波生成
 * 
 * 操作方法:
 * - SW4: コード切り替え（C → F → G → Am → C...）
 * - SW5: 和音再生/停止切り替え  
 * - SW6: 緊急停止
 * 
 * LED表示:
 * - LED5: コード切り替え表示
 * - LED6: 再生状態表示
 */

#include "libmemes.h"
#include <7080S.H>

#define SW6 (PD.DR.BIT.B18)
#define SW5 (PD.DR.BIT.B17)
#define SW4 (PD.DR.BIT.B16)

#define LED6 (PE.DR.BIT.B11)
#define LED5 (PE.DR.BIT.B9)
#define LED_ON (0)
#define LED_OFF (1)

#define SPK (PE.DR.BIT.B0)

/* ===== 基本設定 ===== */

#define CPU_CLOCK     20000000UL

/* ===== 音程定義（周波数Hz） ===== */

#define NOTE_C4  262   // ド
#define NOTE_D4  294   // レ  
#define NOTE_E4  330   // ミ
#define NOTE_F4  349   // ファ
#define NOTE_G4  392   // ソ
#define NOTE_A4  440   // ラ
#define NOTE_B4  494   // シ
#define NOTE_C5  523   // 高いド

/* ===== 和音定義 ===== */

typedef struct {
    uint16_t note1;
    uint16_t note2; 
    uint16_t note3;
    const char* name;
} chord_t;

// 基本的な4和音
static const chord_t chords[] = {
    {NOTE_C4, NOTE_E4, NOTE_G4, "C "},     // Cメジャー
    {NOTE_F4, NOTE_A4, NOTE_C5, "F "},     // Fメジャー  
    {NOTE_G4, NOTE_B4, NOTE_D4, "G "},     // Gメジャー
    {NOTE_A4, NOTE_C5, NOTE_E4, "Am"}      // Aマイナー
};

#define CHORD_COUNT (sizeof(chords) / sizeof(chord_t))

/* ===== 制御変数 ===== */

static volatile uint8_t current_chord = 0;
static volatile uint8_t is_playing = 0;
static volatile uint8_t current_note = 0;
static volatile uint32_t note_timer = 0;
static volatile uint32_t note_duration = 500; // 各音の長さ(ms)

/* ===== MTU23 タイマー制御関数 ===== */

// 指定周波数でMTU23タイマーを設定（スピーカー直接駆動）
void set_mtu23_frequency(uint16_t freq_hz)
{
    uint32_t timer_count;
    
    if (freq_hz == 0) {
        // 停止
        MTU23.TSTR.BIT.CST = 0;
        SPK = 0;
        return;
    }
    
    // MTU23停止
    MTU23.TSTR.BIT.CST = 0;
    
    // タイマーカウント値計算
    // 20MHz / 64分周 / freq_hz / 2 (トグル動作のため)
    timer_count = (CPU_CLOCK / 64 / freq_hz / 2) - 1;
    
    if (timer_count > 65535) {
        // 低周波数の場合、分周比を変更
        MTU23.TCR.BIT.TPSC = 4; // 1/256分周
        timer_count = (CPU_CLOCK / 256 / freq_hz / 2) - 1;
        if (timer_count > 65535) timer_count = 65535;
    } else {
        MTU23.TCR.BIT.TPSC = 3; // 1/64分周
    }
    
    // カウンター設定
    MTU23.TCNT = 0;
    MTU23.TGRA = timer_count;
    
    // コンペアマッチ割り込み有効
    MTU23.TIER.BIT.TGIEA = 1;
    
    // タイマー開始
    MTU23.TSTR.BIT.CST = 1;
}

// MTU23コンペアマッチ割り込みハンドラ
void interrupt_mtu23_tgra(void)
{
    // フラグクリア
    MTU23.TSR.BIT.TGFA = 0;
    
    // スピーカートグル
    SPK = !SPK;
    LED6 = SPK; // デバッグ用LED表示
}

/* ===== 和音制御関数 ===== */

// 単音再生
void play_single_note(uint16_t freq)
{
    set_mtu23_frequency(freq);
}

// 和音再生（順次発音方式）
void play_chord(uint8_t chord_index)
{
    if (chord_index >= CHORD_COUNT) return;
    
    current_note = 0;
    note_timer = 0;
    is_playing = 1;
    
    // 最初の音を開始
    play_single_note(chords[chord_index].note1);
    
    LED6 = LED_ON; // 再生状態表示
}

// 和音停止
void stop_chord(void)
{
    is_playing = 0;
    set_mtu23_frequency(0); // タイマー停止
    
    LED6 = LED_OFF; // 再生停止表示
}

/* ===== 和音シーケンス処理 ===== */

void update_chord_sequence(void)
{
    static uint32_t last_time = 0;
    uint32_t current_time = 0; // システムタイマーを使用する場合
    
    if (!is_playing) return;
    
    note_timer++;
    
    // 音の切り替えタイミング（簡易実装）
    if (note_timer >= note_duration) {
        note_timer = 0;
        current_note = (current_note + 1) % 3;
        
        // 次の音を再生
        switch (current_note) {
            case 0:
                play_single_note(chords[current_chord].note1);
                break;
            case 1:
                play_single_note(chords[current_chord].note2);
                break;
            case 2:
                play_single_note(chords[current_chord].note3);
                break;
        }
    }
}

/* ===== 基本ディレイ関数 ===== */

void music_delay(uint32_t us)
{
    volatile uint32_t i;
    // 20MHzクロックで約50サイクル/μs
    for (i = 0; i < us * 5; i++) {
        __asm("nop");
    }
}

/* ===== スピーカーテスト関数 ===== */

// 基本的なクリック音テスト
void test_speaker_click(void)
{
    uint8_t i;
    
    LED5 = LED_ON;
    
    for (i = 0; i < 5; i++) {
        SPK = 1;
        music_delay(50000);  // 50ms ON
        SPK = 0;
        music_delay(50000);  // 50ms OFF
    }
    
    LED5 = LED_OFF;
}

// 1000Hz方形波テスト
void test_speaker_tone(void)
{
    LED5 = LED_ON;
    
    // MTU23で1000Hzトーン生成
    play_single_note(1000);
    music_delay(1000000); // 1秒再生
    set_mtu23_frequency(0); // 停止
    
    LED5 = LED_OFF;
}

/* ===== ハードウェア初期化 ===== */

void init_hardware(void)
{
    // MTU23モジュール有効化
    STB.CR4.BIT._MTU2 = 0;
    
    // スピーカー出力設定（PE0）
    PFC.PEIORL.BIT.B0 = 1;  // PE0を出力に設定
    PE.DR.BIT.B0 = 0;       // 初期値は0
    
    // LED設定
    PFC.PEIORL.BIT.B11 = 1; // PE11(LED6)を出力に設定
    PFC.PEIORL.BIT.B9 = 1;  // PE9(LED5)を出力に設定
    
    LED6 = LED_OFF;
    LED5 = LED_OFF;
    
    // MTU23設定
    MTU23.TMDR.BIT.MD = 0;   // ノーマルモード
    MTU23.TCR.BIT.CCLR = 1;  // TGRAでクリア
    MTU23.TIER.BIT.TGIEA = 0; // 初期は割り込み無効
    
    // 割り込み優先度設定（適宜調整）
    INTC.IPR19.BIT._MTU23_TGI23A = 8;
    
    // 割り込み有効化
    INTC.MSK1.BIT._MTU23_TGI23A = 0;
}

/* ===== メイン関数 ===== */

void main(void)
{
    uint8_t sw4_prev = 1, sw5_prev = 1, sw6_prev = 1;
    uint32_t main_timer = 0;
    
    // ハードウェア初期化
    init_hardware();
    
    // 起動直後のスピーカーテスト
    music_delay(1000000);  // 1秒待機
    
    LED5 = LED_ON;
    music_delay(500000);   // 0.5秒表示
    LED5 = LED_OFF;
    
    // クリック音テスト
    test_speaker_click();
    
    music_delay(2000000);  // 2秒間隔
    
    // 1000Hzトーンテスト
    test_speaker_tone();
    
    // メインループ
    while(1)
    {
        main_timer++;
        
        // 和音シーケンス更新（約1msごと）
        if (main_timer >= 1000) {
            main_timer = 0;
            update_chord_sequence();
        }
        
        // SW4: コード切り替え
        if (SW4 == 0 && sw4_prev == 1) {
            current_chord = (current_chord + 1) % CHORD_COUNT;
            
            if (is_playing) {
                stop_chord();
                music_delay(100000); // 100ms間隔
                play_chord(current_chord);
            }
            
            // LED5でコード切り替え表示
            LED5 = LED_ON;
        } else if (SW4 == 1 && sw4_prev == 0) {
            LED5 = LED_OFF;
        }
        sw4_prev = SW4;
        
        // SW5: 和音再生/停止切り替え
        if (SW5 == 0 && sw5_prev == 1) {
            if (is_playing) {
                stop_chord();
            } else {
                play_chord(current_chord);
            }
        }
        sw5_prev = SW5;
        
        // SW6: 緊急停止
        if (SW6 == 0 && sw6_prev == 1) {
            stop_chord();
        }
        sw6_prev = SW6;
        
        // 短いディレイ（デバウンス）
        music_delay(1000); // 1ms
    }
}

/* ===== 従来互換関数 ===== */

// 従来のmusic()関数との互換性
void music(int hz)
{
    if (hz == 0) {
        stop_chord();
    } else {
        is_playing = 0; // 和音モードを停止
        play_single_note(hz);
    }
}
