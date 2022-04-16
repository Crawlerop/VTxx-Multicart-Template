# dpcmc

waveファイルを、ファミコン仕様相当のDPCMデータに変換します。
聴覚特性に基づいた最適化処理を行う事で、高音質なDPCMデータを生成する事を目指しています。

以下のような特徴があります。
* 波形エンベロープ検出により、任意の中心値への自動追従
* 非線形DAC特性の補償
* ディザリング、ノイズシェーピング機能
* dmcファイルからwavデータへの変換

# 使用方法
DOSプロンプトまたは、ターミナルから以下のように実行します。

    dpcmc [options] input ..

* inputには無圧縮のwav,aiffファイルを指定する事ができます。
* カレントフォルダにdmcファイルが出力されます。
* 複数のinputを指定した場合、全ての入力ファイルに同じオプションが適用されます。
* マルチチャンネルオーディオデータの場合、自動的にモノラルに変換されます。
* 指定された出力サンプリングレートに自動的に変換されます。
* 16byte境界にパディングされます。満たない場合は、中心値に向かうサンプルが追加されます。

また、dmcファイルをwavファイルに変換するときは以下のように実行します。

    dpcmc -f -i[0..127] -r[rate] input ..

* -i, -r オプションを省略した場合は、-i64, -r15 が指定されたとみなされます。
    
# オプションの詳細
* -g[gain]
    * 入力波形の音量増幅率を、実数値で指定します。
    * 変換後の波形の振幅が小さい場合に音量を上げる、あるいはクリップしている場合に音量を下げたい場合などに使用します。
* -i[0..127]
    * DPCM波形の初期ボリュームを0から127の値で指定します。
    * ここで指定した値を開始点とする波形が生成されます。
    * 生成されるdmcファイル名の末尾に、初期ボリューム値が付加されます。
    * 指定しない場合、自動的に設定されます。
* -c[0..127]
    * 波形の中心点を任意の値に自動追従させます。
    * 0だと下方に張り付いた波形になり、127だと上方に張り付いた波形、64だと元の波形に近い波形が生成されます。
    * 非線形特性の補正機能が有効な場合、波形の0に近い部分では解像度が低くなります。そのため、中心位置が高い方がノイズが少なくなります。ただし、中心位置を高く設定した場合、波形の応答速度が相対的に低下するため、低い中心位置の場合よりも波形の歪みは大きくなります。
* -w[weight]
    * 検出された波形エンベロープから得られた中心点補正曲線の重み付けの係数を変更します。
    * 出力波形の中心位置からのずれが大きい場合は、数値を小さくし、逆に、波形が上や下に張り付きすぎる場合は、値を大きくしてみてください。
* -r[rate]
    * 出力波形のサンプリングレートを選択します。
        *  0:4181.71Hz  1:4709.93Hz  2:5264.04Hz  3:5593.04Hz 
        *  4:6257.95Hz  5:7046.35Hz  6:7919.35Hz  7:8363.42Hz 
        *  8:9419.86Hz  9:11186.1Hz 10:12604.0Hz 11:13982.6Hz 
        * 12:16884.6Hz 13:21056.1Hz 14:24858.0Hz 15:33143.9Hz 
    * この設定値は、エンベロープ検出処理に影響するため、最終的に再生するサンプリングレートを指定してください。
* -n
    * このオプションが指定された場合、入力波形のサンプリングレートの変換を行いません。つまり、入力波形のサンプリングレートを無視して、出力と同じとみなします。
* -d[mode]
    * ディザリングの設定を行います。ノイズシェーピングと組み合わせて使用する事を推奨します。
        * [0 : off]
            * ディザ信号を付加しません。
        * [1 : Rectangle]
            * ディザ信号に方形確率密度関数を使用します。効果が高い代わりに、付加されるノイズ信号レベルが大きいです。ディザリングの効果が薄い場合に使用してください。
        * [2 : Triangle]
            * ディザ信号に三角形確率密度関数を使用します。方形確率密度関数よりも効果が薄いですが、付加されるノイズの量が小さいです。
        * [3 : Highpassed Triangle]
            * 低域成分が少なく、高域成分の多くなるように調整された三角形確率密度関数を使用します。多くの場合、ノイズシェーピングと組み合わせる事で、知覚されやすいノイズ成分が減少します。（デフォルト）
* -s[mode]
    * ノイズシェーピングの設定を行います。量子化ノイズとディザ信号の周波数帯域を移動させる効果があります。ただし、非線形特性の補正が有効で、かつ波形の中心点が高い位置に設定されている場合には効果が薄くなります。
        * [0 : off]
            * ノイズシェーピングを無効にします。
        * [1 : Lowpass]
            * ノイズ成分を低域側に移動させます。高域のノイズが目立つ場合に有効な場合があります。
        * [2 : Highpass]
            * ノイズ成分を高域側に移動させます。低域のノイズが目立つ場合に有効な場合があります。
        * [3 : equal-loudness]
            * 聴覚の感度の高い4kHz付近のノイズを減少させます。低域や超高域のノイズが若干増えますが、ほとんどの場合、最も効果があります。（デフォルト）
* -l
    * ファミコンのDACの非線形特性を補正する処理を無効にします。
* -e
    * 途中処理で検出されたエンベロープ波形をwavファイルとして書き出します。
* -p
    * 中心位置の補正された波形をwavファイルとして書き出します。
* -o
    * コンバート後の波形をwavファイルとして書き出します。
* -f
    * dmcファイルをwavファイルに変換する動作モードになります。

# ビルド方法
cmake(3.1以降)が必要です。

    mkdir build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make

## 以下のソフトウェアを利用しています
* The Synthesis ToolKit in C++ (STK)
    * https://ccrma.stanford.edu/software/stk/)
* 汎用 FFT (高速 フーリエ/コサイン/サイン 変換) パッケージ
    * http://www.kurims.kyoto-u.ac.jp/~ooura/fft-j.html