# ZMK Config for Cornix TB (Trackball)

Cornixとモジュール型トラックボールを連携させ、一つのキーボードとして利用できるようにするZMKファームウェアです。

## モジュール型トラックボール

 ![トラックボールモジュール搭載例](docs/images/trackball_mount.jpg)
 親指キーの一番外側のキースイッチの箇所にマウントすることを想定
 一応左でも実装可能と思うが調整が必要

 ### 必要部品
 - **PAW3222ブレイクアウトボード**: https://nogikes.booth.pm/items/6520217
 - **FFCコネクタ変換基板**: 上記からセット購入推奨
 - **FFCケーブル**: 15cm以上で取り回ししやすくなります
 - **XIAO BLE nrf52840**: 
 - **上記配線用ケーブル**:
 - **トラックボールケース**: https://github.com/sekigon-gonnoc/torabo-tsuki-lp/blob/master/3d-models/STL/trackball-case-19mm-Body.stl
 - **トラックボールケース用マウント台**: https://github.com/t-ogura/zmk-config-cornix-tb/tree/master/3dmodel/cornix_trackball_case_mount.step
 - **19mmトラックボール**: PTFE球がおすすめ 

  ### 配線： PAW3222センサー ⇔ Seeeduino XIAO nRF52840

  | ピン番号 | PAW3222信号 | 機能 | XIAO nRF52840ピン | 備考 |
  |---------|------------|------|------------------|------|
  | 1 | 3.3V | 電源 | 3V3 | 3.3V電源 |
  | 2 | CS | Chip Select | D10 | SPI通信 |
  | 3 | MOTION | モーション検出 | D9 | 割り込み信号 |
  | 4 | SDIO | SPI Data I/O | D8 | 双方向データ |
  | 5 | SCLK | SPI Clock | D7 | クロック |
  | 6 | GND | グランド | GND | グランド |

  バッテリーを配線してもいいと思います。
  現段階ではUSB給電を想定しています。

  配線例
 ![配線例](docs/images/trackball_wiring.jpg)

## Cornix TBの構成
- **Left Cornix**: Central
- **Right Cornix**: Peripheral
- **Trackball module**: Peripheral

Peripheralが2台になる3台構成になります。

## ファームウェア書き込み時の注意

> **⚠️ 重要: リセットファームウェアを間違えないでください**

GitHub Actions でビルドされるファームウェアには、**デバイスごとに異なるリセットファームウェア**が含まれます：

| ファイル名 | 対象デバイス | 書き込み先 |
|-----------|-------------|-----------|
| `cornix_left_central.uf2` | Cornix Left | Cornix Left |
| `cornix_right.uf2` | Cornix Right | Cornix Right |
| `trackball_peripheral.uf2` | トラックボール | **Xiao nRF52840** |
| `reset_cornix.uf2` | Cornix用リセット | **Cornix のみ** |
| `reset_xiao.uf2` | Xiao用リセット | **Xiao のみ** |

**Cornix 用の `reset_cornix.uf2` を Xiao に書き込むとブートローダーが破損します。**

同じ nRF52840 チップでもボード設定が異なるため互換性がありません。誤って書き込んでしまった場合は [Xiao 復旧手順](docs/xiao_recovery.md) を参照してください。

## Prospectorについて

現段階でProspectorを使用するように設定されていますが不要であればconfig/west.ymlとconfig/cornix_left.confを編集してください。

## 参考資料
下記情報を大いに活用させていただきました。ありがとうございます。
- https://github.com/hitsmaxft/zmk-keyboard-cornix/
- https://zenn.dev/kot149/articles/zmk-workspace
- https://zenn.dev/kot149/articles/zmk-input-processor-cheat-sheet
- https://zenn.dev/kot149/articles/zmk-auto-mouse-layer
- https://note.com/pooh_polo/n/n133ad59486ea
- https://github.com/sekigon-gonnoc/small-mouse-sensor-module

## License

This project is licensed under the [MIT License](LICENSE).

### Third-Party Components

This project is built upon the following open-source projects:

| Project | License | Usage |
|---------|---------|-------|
| [zmk-keyboard-cornix](https://github.com/hitsmaxft/zmk-keyboard-cornix) | Apache-2.0 | Cornix board definitions |
| [small-mouse-sensor-module](https://github.com/sekigon-gonnoc/small-mouse-sensor-module) | MIT | Sensor module design reference |
| [zmk-driver-paw3222](https://github.com/sekigon-gonnoc/zmk-driver-paw3222) | MIT | PAW3222 trackball driver |

See [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) for full license texts and attribution details.
