# MODEコマンド テストケース

## 前提条件
- サーバーが起動していること
- テストクライアントが接続済みであること
- テストチャンネル #test が作成済みであること
- テストユーザーがチャンネルオペレーターであること

## 基本セットアップ
```
NICK testuser
USER testuser 0 * :Test User
JOIN #test
```

## 1. +i/-i オプション (Invitation Only)

### +i: チャンネルを招待制にする
```
MODE #test +i
```
**期待される結果:**
- チャンネルメンバー全員に `:testuser MODE #test +i` が送信される
- チャンネルが招待制になる

### -i: 招待制を解除する
```
MODE #test -i
```
**期待される結果:**
- チャンネルメンバー全員に `:testuser MODE #test -i` が送信される
- 招待制が解除される

## 2. +t/-t オプション (Topic Restriction)

### +t: トピック変更をオペレーターのみに制限
```
MODE #test +t
```
**期待される結果:**
- チャンネルメンバー全員に `:testuser MODE #test +t` が送信される
- トピック変更がオペレーターのみに制限される

### -t: 全メンバーがトピック変更可能
```
MODE #test -t
```
**期待される結果:**
- チャンネルメンバー全員に `:testuser MODE #test -t` が送信される
- 全メンバーがトピック変更可能になる

## 3. +l/-l オプション (User Limit)

### +l: ユーザー数制限を設定
```
MODE #test +l 10
```
**期待される結果:**
- チャンネルメンバー全員に `:testuser MODE #test +l 10` が送信される
- チャンネルの最大ユーザー数が10人に設定される

### -l: ユーザー数制限を解除
```
MODE #test -l
```
**期待される結果:**
- チャンネルメンバー全員に `:testuser MODE #test -l` が送信される
- ユーザー数制限が解除される

### +l エラーケース: パラメータなし
```
MODE #test +l
```
**期待される結果:**
- `:myserver 461 testuser MODE :Not enough parameters for +l`

### +l エラーケース: 無効な値
```
MODE #test +l abc
```
**期待される結果:**
- `:myserver 461 testuser MODE :Invalid limit value`

### +l エラーケース: 範囲外の値
```
MODE #test +l 1000
```
**期待される結果:**
- `:myserver 461 testuser MODE :Invalid limit value (1-999)`

## 4. +k/-k オプション (Channel Key)

### +k: チャンネルキー（パスワード）を設定
```
MODE #test +k secret123
```
**期待される結果:**
- チャンネルメンバー全員に `:testuser MODE #test +k secret123` が送信される
- チャンネルにパスワードが設定される

### -k: チャンネルキーを削除
```
MODE #test -k
```
**期待される結果:**
- チャンネルメンバー全員に `:testuser MODE #test -k` が送信される
- チャンネルキーが削除される

### +k エラーケース: パラメータなし
```
MODE #test +k
```
**期待される結果:**
- `:myserver 461 testuser MODE :Not enough parameters for +k`

### +k エラーケース: 空のキー
```
MODE #test +k ""
```
**期待される結果:**
- `:myserver 461 testuser MODE :Channel key cannot be empty`

### +k エラーケース: 長すぎるキー
```
MODE #test +k verylongpasswordthatexceedsfiftycharacterslimit123456789
```
**期待される結果:**
- `:myserver 461 testuser MODE :Channel key too long (max 50 characters)`

## 5. +o/-o オプション (Operator Privilege)

### セットアップ: 別のユーザーをチャンネルに追加
```
# 別のクライアントで:
NICK testuser2
USER testuser2 0 * :Test User 2
JOIN #test
```

### +o: オペレーター権限を付与
```
MODE #test +o testuser2
```
**期待される結果:**
- チャンネルメンバー全員に `:testuser MODE #test +o testuser2` が送信される
- testuser2にオペレーター権限が付与される

### -o: オペレーター権限を剥奪
```
MODE #test -o testuser2
```
**期待される結果:**
- チャンネルメンバー全員に `:testuser MODE #test -o testuser2` が送信される
- testuser2からオペレーター権限が剥奪される

### +o エラーケース: パラメータなし
```
MODE #test +o
```
**期待される結果:**
- `:myserver 461 testuser MODE :Not enough parameters for +o`

### +o エラーケース: 存在しないユーザー
```
MODE #test +o nonexistentuser
```
**期待される結果:**
- `:myserver 441 testuser nonexistentuser #test :They aren't on that channel`

## 6. エラーケース

### パラメータ不足
```
MODE #test
```
**期待される結果:**
- `:myserver 461 testuser MODE :Not enough parameters`

### 存在しないチャンネル
```
MODE #nonexistent +i
```
**期待される結果:**
- `:myserver 403 testuser #nonexistent :No such channel`

### チャンネルメンバーでない
```
# チャンネルから退出後:
PART #test
MODE #test +i
```
**期待される結果:**
- `:myserver 442 testuser #test :You're not on that channel`

### オペレーター権限なし
```
# 別のクライアント（非オペレーター）で:
MODE #test +i
```
**期待される結果:**
- `:myserver 482 testuser2 #test :You're not channel operator`

### 複数モード指定
```
MODE #test +it
```
**期待される結果:**
- `:myserver 461 testuser MODE :Only one mode change allowed per command`

### 無効なモード文字
```
MODE #test +x
```
**期待される結果:**
- `:myserver 472 testuser x :is unknown mode char to me`

### モード指定なし
```
MODE #test +
```
**期待される結果:**
- `:myserver 461 testuser MODE :No valid mode specified`
