# Emue
2023/11/3
全面的に見直し。
mapper30もセーブ対応

2022/10/6
セーブ機能に対応
ただしmapper30は除く

2022/9/25
マッパー4に対応
色々見直し。

2022/9/17
マッパー1,30に対応

2022/9/11
マッパー2,3に対応
バグフィックス

2022/9/4
コントローラを大きくした。
リセットと終了を追加した。(ゲーム画面上半分が終了、下半分がリセット)

2022/9/2
ゲーム画面の描画を少し修正


2022/8/31
処理落ちしたときに実行速度優先のため自動でフレームスキップする機能を追加
8*16サイズのスプライトに対応


自分の練習の題材として作ったよ。
ファミコンだよ。
ライブラリはsdl2とsdl_ttfが必要だよ。
readrom.cppでttfファイルを読み込むけど、環境によってttfファイルがあるフォルダは違うので修正が必要だよ。
romはromフォルダに入れるよ。
romフォルダは実行ファイルが生成されるフォルダに入れるよ。
スマホアプリのcxxdroidなら修正なしで動くよ。
makelist.txtは環境に合わせて修正がいると思うよ。

音は実装しないよ。
Mapperは実装してないよ。
ROM選択は難しいよ。
バグは残ってるよ。

最初の黒い画面で読み込むrom名を入力して決定を押すと始まるよ。
入力訂正機能は実装してないのでタイプミスに注意。
