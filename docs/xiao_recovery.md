# Xiao nRF52840 BLE ブートローダー復旧手順

Xiao nRF52840 が起動しなくなった場合（ダブルタップでブートローダーに入れない、シングルタップでしかブートローダーに入れない等）の復旧手順です。

## 症状

- ファームウェア書き込み後、デバイスが起動しない
- ダブルタップリセットが効かず、シングルタップでブートローダーに入る
- `reset.uf2` を書き込んでも復旧しない

## 原因

- **誤ったボード用のファームウェア/リセットファームウェアを書き込んだ**
- ブートローダーの失敗カウンタが上限に達した
- フラッシュの設定領域が破損した

### よくある間違い

**Cornix 用の `reset_cornix.uf2` を Xiao に書き込んでしまう**

GitHub Actions でビルドされるリセットファームウェアは複数あります：

| ファイル名 | 対象デバイス | MCU |
|-----------|-------------|-----|
| `reset_cornix.uf2` | Cornix キーボード | nRF52840 (E73モジュール) |
| `reset_xiao.uf2` | Xiao nRF52840 BLE | nRF52840 (Seeed Xiao) |

**同じ nRF52840 でもボード設定が異なるため、互換性がありません。**

Cornix 用のリセットファームウェアを Xiao に書き込むと、設定領域が不正に書き換えられ、ブートローダーが正常に動作しなくなります。

## 復旧手順

### 1. 必要なツールのインストール

```bash
pip install adafruit-nrfutil
```

### 2. ブートローダーのダウンロード

[Adafruit nRF52 Bootloader Releases](https://github.com/adafruit/Adafruit_nRF52_Bootloader/releases) から、Xiao nRF52840 用のブートローダーをダウンロード：

| ファイル名 | DFU時のデバイス名 | 備考 |
|-----------|------------------|------|
| `xiao_nrf52840_ble_bootloader-X.X.X_s140_7.3.0.zip` | **XIAO-BOOT** | 標準版 |
| `xiao_nrf52840_ble_sense_bootloader-X.X.X_s140_7.3.0.zip` | **XIAO-SENSE** | 購入時と同じ |

**補足:** XIAO BLE は購入時のデバイス名が `XIAO-SENSE` です。復旧後も同じデバイス名にしたい場合は `sense` 版を使用してください。

### 3. ブートローダーモードに入る

Xiao をブートローダーモードにします：

- **シングルタップ** または **ダブルタップ** でリセット
- PCにドライブとして認識されればOK

### 4. COMポートの確認

Windows の場合、デバイスマネージャーで COM ポートを確認：

1. デバイスマネージャーを開く
2. 「ポート (COM と LPT)」を展開
3. 「USB Serial Device (COMXX)」を確認

### 5. ブートローダーの書き込み

```powershell
# Windows PowerShell
adafruit-nrfutil --verbose dfu serial --package .\xiao_nrf52840_ble_bootloader-0.9.2_s140_7.3.0.zip --port COM25 -b 115200
```

```bash
# Linux / macOS
adafruit-nrfutil --verbose dfu serial --package ./xiao_nrf52840_ble_bootloader-0.9.2_s140_7.3.0.zip --port /dev/ttyACM0 -b 115200
```

※ `COM25` や `/dev/ttyACM0` は実際のポート名に置き換えてください。

### 6. 復旧確認

書き込み完了後：

1. USB を抜き差し
2. **ダブルタップ**でブートローダーに入れることを確認
3. 正しいファームウェアを書き込む

## 注意事項

- 必ず `xiao_nrf52840_ble` 用のブートローダーを使用すること
- 他のボード用（Cornix 等）のファームウェアを誤って書き込まないこと
- `reset.uf2` も**ボードごとに異なる**ため、正しいボード用を使用すること

## 参考リンク

- [Adafruit nRF52 Bootloader](https://github.com/adafruit/Adafruit_nRF52_Bootloader)
- [Seeed Studio Xiao nRF52840](https://wiki.seeedstudio.com/XIAO_BLE/)
- [adafruit-nrfutil (PyPI)](https://pypi.org/project/adafruit-nrfutil/)
