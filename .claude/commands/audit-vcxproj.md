---
description: Audit every .vcxproj in this solution for files that are unregistered, or whose physical location doesn't match their vcxproj.filters <Filter>, and fix them
---

このソリューション（`src/MebukiEngine.slnx`）配下の全プロジェクト（`MebukiEngine.vcxproj`, `SampleGame.vcxproj`）について、プロジェクトファイルの衛生状態を監査し、見つかった問題を直してください。ThirdParty/packages配下のサードパーティコードは対象外です。

## チェック1: 未登録ファイル

各プロジェクトについて:

1. そのプロジェクトのディレクトリ配下にある `.h`/`.cpp`/`.hlsl` の実ファイル一覧を取る（`x64/`, `.vs/` などのビルド生成物は除外）
2. `.vcxproj` 内の `ClCompile`/`ClInclude`/`None`/`FxCompile` の `Include="..."` を全部抽出する
3. 実ファイルにあって `.vcxproj` に無いものを洗い出す（`comm -23` などで差分を取ると早いです）
4. 見つかったら、拡張子に応じて適切な要素で `.vcxproj` に追加する（`.cpp` → `ClCompile`、`.h` → `ClInclude`、`.hlsl` → `None`、Shaders配下のものは既存の`Shaders\*.hlsl`の並びに揃える）
5. `.vcxproj.filters` にも対応するフィルター（ファイルが置かれているサブディレクトリ名に対応するフィルター。無ければ作る）付きで追加する

## チェック2: 実体とフィルター表示の不一致

各プロジェクトについて:

1. `.vcxproj.filters` の中で `<Filter>` タグを持つ全項目を確認する
2. その項目の `Include="..."` パス（＝実際のファイルの場所のはず）が、実体のファイルシステム上の場所と一致しているか確認する
3. 食い違っていたら（例: 実体は `Shaders\` 配下にあるのに `Include` がルート直下を指している、あるいはその逆）、実体をあるべき場所に `mv`（追跡済みなら `git mv`）で移動し、`.vcxproj` と `.vcxproj.filters` 両方の `Include` パスを実体の場所に揃える。`<Filter>` タグの値自体は変えない

## 注意点

- XMLの書き換えは Edit ツールを使うこと。`sed` はバックスラッシュのエスケープで事故りやすいので使わない
- `<FxCompile>` は使わない（このプロジェクトはシェーダーを `D3DCompileFromFile` で実行時コンパイルする方式なので、`.hlsl` は常に `<None>` で登録する。もし `<FxCompile>` になっているものを見つけたら `<None>` に直す）
- 全部直したら、`MebukiEngine.slnx` を MSBuild でビルドして確認する
  - MSBuildの場所が不明な場合は `Get-ChildItem "C:\Program Files\Microsoft Visual Studio\" -Recurse -Filter "MSBuild.exe"` で探す
  - 個別の `.vcxproj` を直接ビルドしない（`$(SolutionDir)` が正しく解決されないため）。必ず `.slnx` 経由でビルドする
- 見つかった問題と直した内容を一覧で報告すること
