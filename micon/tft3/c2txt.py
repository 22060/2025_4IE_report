from PIL import Image

def rgb888_to_rgb565(r, g, b):
    """RGB888 → RGB565 変換"""
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

def convert_jpg_to_rgb565_2d_header(jpg_path, header_path, max_size=128):
    # 画像読み込み
    img = Image.open(jpg_path).convert('RGB')
    original_width, original_height = img.size

    # スケーリング（アスペクト比を維持し長辺をmax_sizeに）
    if original_width > original_height:
        scale = max_size / original_width
    else:
        scale = max_size / original_height

    new_width = int(original_width * scale)
    new_height = int(original_height * scale)
    img = img.resize((new_width, new_height), Image.LANCZOS)

    # ピクセルデータ取得
    pixels = list(img.getdata())

    # 2次元配列に変換
    pixel_rows = [pixels[y * new_width : (y + 1) * new_width] for y in range(new_height)]

    # ヘッダファイルとして出力
    with open(header_path, 'wb') as f:
        f.write(bytes([(new_width >> 8) & 0xFF, new_width & 0xFF, new_height & 0xFF]))

        for row in pixel_rows:
            for r, g, b in row:
                value = rgb888_to_rgb565(r, g, b)
                high = (value >> 8) & 0xFF
                low = value & 0xFF
                f.write(bytes([high, low]))

    print(f"[完了] ヘッダファイル '{header_path}' を生成しました（{new_width}x{new_height}）")

# 使用例
convert_jpg_to_rgb565_2d_header("input.jpg", "rgb565.img",max_size=240)
