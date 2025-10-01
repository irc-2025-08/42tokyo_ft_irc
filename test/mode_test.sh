#!/bin/bash

# MODEコマンド自動テストスクリプト
# 使用方法: ./mode_test.sh <port>

if [ $# -ne 1 ]; then
    echo "使用方法: $0 <port>"
    exit 1
fi

PORT=$1
TEST_LOG="mode_test_results.log"

echo "=== MODEコマンド テスト開始 ===" > $TEST_LOG
echo "ポート: $PORT" >> $TEST_LOG
echo "開始時刻: $(date)" >> $TEST_LOG
echo "" >> $TEST_LOG

# テスト関数
run_test() {
    local test_name="$1"
    local commands="$2"
    local expected="$3"
    
    echo "テスト: $test_name" >> $TEST_LOG
    echo "コマンド: $commands" >> $TEST_LOG
    echo "期待結果: $expected" >> $TEST_LOG
    
    # テストクライアントにコマンドを送信
    echo -e "$commands" | timeout 5 ./testClient $PORT >> $TEST_LOG 2>&1
    
    echo "---" >> $TEST_LOG
    echo ""
}

# テストクライアントをビルド
echo "テストクライアントをビルド中..."
g++ -o testClient testClient.cpp
if [ $? -ne 0 ]; then
    echo "テストクライアントのビルドに失敗しました"
    exit 1
fi

echo "テスト実行中... (結果は $TEST_LOG に保存されます)"

# 基本セットアップ
SETUP="NICK testuser\r\nUSER testuser 0 * :Test User\r\nJOIN #test\r\n"

# 1. +i/-i テスト
run_test "招待制設定 (+i)" \
    "${SETUP}MODE #test +i\r\n" \
    "MODE #test +i"

run_test "招待制解除 (-i)" \
    "${SETUP}MODE #test +i\r\nMODE #test -i\r\n" \
    "MODE #test -i"

# 2. +t/-t テスト
run_test "トピック制限設定 (+t)" \
    "${SETUP}MODE #test +t\r\n" \
    "MODE #test +t"

run_test "トピック制限解除 (-t)" \
    "${SETUP}MODE #test +t\r\nMODE #test -t\r\n" \
    "MODE #test -t"

# 3. +l/-l テスト
run_test "ユーザー制限設定 (+l)" \
    "${SETUP}MODE #test +l 10\r\n" \
    "MODE #test +l 10"

run_test "ユーザー制限解除 (-l)" \
    "${SETUP}MODE #test +l 10\r\nMODE #test -l\r\n" \
    "MODE #test -l"

# 4. +k/-k テスト
run_test "チャンネルキー設定 (+k)" \
    "${SETUP}MODE #test +k secret123\r\n" \
    "MODE #test +k secret123"

run_test "チャンネルキー削除 (-k)" \
    "${SETUP}MODE #test +k secret123\r\nMODE #test -k\r\n" \
    "MODE #test -k"

# 5. エラーケーステスト
run_test "パラメータ不足エラー" \
    "${SETUP}MODE #test\r\n" \
    "461 testuser MODE :Not enough parameters"

run_test "存在しないチャンネルエラー" \
    "${SETUP}MODE #nonexistent +i\r\n" \
    "403 testuser #nonexistent :No such channel"

run_test "チャンネルメンバーでないエラー" \
    "NICK testuser\r\nUSER testuser 0 * :Test User\r\nMODE #test +i\r\n" \
    "442 testuser #test :You're not on that channel"

run_test "複数モード指定エラー" \
    "${SETUP}MODE #test +it\r\n" \
    "461 testuser MODE :Only one mode change allowed per command"

run_test "無効なモード文字エラー" \
    "${SETUP}MODE #test +x\r\n" \
    "472 testuser x :is unknown mode char to me"

run_test "+l パラメータなしエラー" \
    "${SETUP}MODE #test +l\r\n" \
    "461 testuser MODE :Not enough parameters for +l"

run_test "+l 無効な値エラー" \
    "${SETUP}MODE #test +l abc\r\n" \
    "461 testuser MODE :Invalid limit value"

run_test "+l 範囲外の値エラー" \
    "${SETUP}MODE #test +l 1000\r\n" \
    "461 testuser MODE :Invalid limit value (1-999)"

run_test "+k パラメータなしエラー" \
    "${SETUP}MODE #test +k\r\n" \
    "461 testuser MODE :Not enough parameters for +k"

run_test "+o パラメータなしエラー" \
    "${SETUP}MODE #test +o\r\n" \
    "461 testuser MODE :Not enough parameters for +o"

run_test "+o 存在しないユーザーエラー" \
    "${SETUP}MODE #test +o nonexistentuser\r\n" \
    "441 testuser nonexistentuser #test :They aren't on that channel"

echo "=== テスト完了 ===" >> $TEST_LOG
echo "終了時刻: $(date)" >> $TEST_LOG

echo "テスト完了！結果は $TEST_LOG を確認してください。"

# クリーンアップ
rm -f testClient
