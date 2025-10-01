#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
MODEコマンド 包括的テストスクリプト
全パターンを自動実行してテストする
使用方法: python3 mode_comprehensive_test.py <port>
"""

import socket
import sys
import time
import threading

class IRCTestClient:
    def __init__(self, host='localhost', port=6667, nickname='testuser'):
        self.host = host
        self.port = port
        self.nickname = nickname
        self.socket = None
        self.connected = False
        self.responses = []
        
    def connect(self):
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.connect((self.host, self.port))
            self.connected = True
            print(f"[{self.nickname}] サーバー {self.host}:{self.port} に接続しました")
            
            # 受信スレッドを開始
            self.receive_thread = threading.Thread(target=self.receive_messages)
            self.receive_thread.daemon = True
            self.receive_thread.start()
            
            return True
        except Exception as e:
            print(f"[{self.nickname}] 接続エラー: {e}")
            return False
    
    def send_command(self, command):
        if not self.connected:
            print(f"[{self.nickname}] サーバーに接続されていません")
            return False
        
        try:
            message = command + "\r\n"
            self.socket.send(message.encode('utf-8'))
            print(f"[{self.nickname}] 送信: {command}")
            return True
        except Exception as e:
            print(f"[{self.nickname}] 送信エラー: {e}")
            return False
    
    def receive_messages(self):
        buffer = ""
        while self.connected:
            try:
                data = self.socket.recv(1024).decode('utf-8')
                if not data:
                    break
                
                buffer += data
                while '\r\n' in buffer:
                    line, buffer = buffer.split('\r\n', 1)
                    if line:
                        self.responses.append(line)
                        print(f"[{self.nickname}] 受信: {line}")
                        
            except Exception as e:
                if self.connected:
                    print(f"[{self.nickname}] 受信エラー: {e}")
                break
    
    def disconnect(self):
        self.connected = False
        if self.socket:
            self.socket.close()
        print(f"[{self.nickname}] 接続を切断しました")

def setup_client(client, nickname, channel="#test"):
    """クライアントの基本セットアップ"""
    client.send_command(f"NICK {nickname}")
    time.sleep(0.2)
    client.send_command(f"USER {nickname} 0 * :{nickname} User")
    time.sleep(0.2)
    client.send_command(f"JOIN {channel}")
    time.sleep(0.5)

def test_basic_modes(client, channel="#test"):
    """基本モードのテスト"""
    print("\n=== 基本モードテスト ===")
    
    basic_modes = [
        f"MODE {channel} +i",
        f"MODE {channel} -i",
        f"MODE {channel} +t",
        f"MODE {channel} -t",
        f"MODE {channel} +l 10",
        f"MODE {channel} -l",
        f"MODE {channel} +k secret123",
        f"MODE {channel} -k",
    ]
    
    for mode_cmd in basic_modes:
        client.send_command(mode_cmd)
        time.sleep(0.3)

def test_user_limit_variations(client, channel="#test"):
    """ユーザー制限の様々な値をテスト"""
    print("\n=== ユーザー制限バリエーションテスト ===")
    
    limits = [1, 5, 10, 20, 50, 100, 999]
    
    for limit in limits:
        client.send_command(f"MODE {channel} +l {limit}")
        time.sleep(0.2)
    
    client.send_command(f"MODE {channel} -l")
    time.sleep(0.2)

def test_channel_key_variations(client, channel="#test"):
    """チャンネルキーの様々なパターンをテスト"""
    print("\n=== チャンネルキーバリエーションテスト ===")
    
    keys = [
        "password",
        "secret123",
        "mykey",
        "test",
        "12345",
        "abcdefghijklmnopqrstuvwxyz",
        "password_with_underscores",
        "password-with-hyphens",
        "PASSWORD123",
        "a",  # 最短
        "abcdefghijklmnopqrstuvwxyz1234567890abcdefghij",  # 50文字
    ]
    
    for key in keys:
        client.send_command(f"MODE {channel} +k {key}")
        time.sleep(0.2)
        client.send_command(f"MODE {channel} -k")
        time.sleep(0.2)

def test_error_cases(client, channel="#test"):
    """エラーケースのテスト"""
    print("\n=== エラーケーステスト ===")
    
    error_commands = [
        # パラメータ不足
        "MODE",
        f"MODE {channel}",
        
        # 存在しないチャンネル
        "MODE #nonexistent +i",
        "MODE #notfound +t",
        "MODE #missing +l 10",
        
        # 複数モード指定
        f"MODE {channel} +it",
        f"MODE {channel} +il",
        f"MODE {channel} +ik",
        f"MODE {channel} +io",
        f"MODE {channel} +tl",
        f"MODE {channel} +tk",
        f"MODE {channel} +to",
        f"MODE {channel} +lk",
        f"MODE {channel} +lo",
        f"MODE {channel} +ko",
        
        # 無効なモード文字
        f"MODE {channel} +a",
        f"MODE {channel} +b",
        f"MODE {channel} +x",
        f"MODE {channel} +y",
        f"MODE {channel} +z",
        
        # モード指定なし
        f"MODE {channel} +",
        f"MODE {channel} -",
        
        # +l エラー
        f"MODE {channel} +l",
        f"MODE {channel} +l abc",
        f"MODE {channel} +l -1",
        f"MODE {channel} +l 0",
        f"MODE {channel} +l 1000",
        
        # +k エラー
        f"MODE {channel} +k",
        f"MODE {channel} +k verylongpasswordthatexceedsfiftycharacterslimitabcdefghijklmnopqrstuvwxyz",
        
        # +o/-o エラー
        f"MODE {channel} +o",
        f"MODE {channel} -o",
        f"MODE {channel} +o nonexistentuser",
        f"MODE {channel} -o nonexistentuser",
    ]
    
    for cmd in error_commands:
        client.send_command(cmd)
        time.sleep(0.2)

def test_operator_privileges(client1, client2, channel="#test"):
    """オペレーター権限のテスト"""
    print("\n=== オペレーター権限テスト ===")
    
    # client1がclient2にオペレーター権限を付与
    client1.send_command(f"MODE {channel} +o {client2.nickname}")
    time.sleep(0.3)
    
    # client2がモードを変更できるかテスト
    client2.send_command(f"MODE {channel} +i")
    time.sleep(0.3)
    
    # client1がclient2からオペレーター権限を剥奪
    client1.send_command(f"MODE {channel} -o {client2.nickname}")
    time.sleep(0.3)
    
    # client2がモードを変更できないかテスト（エラーになるはず）
    client2.send_command(f"MODE {channel} +t")
    time.sleep(0.3)

def test_boundary_values(client, channel="#test"):
    """境界値テスト"""
    print("\n=== 境界値テスト ===")
    
    # +l 境界値
    client.send_command(f"MODE {channel} +l 1")
    time.sleep(0.2)
    client.send_command(f"MODE {channel} +l 999")
    time.sleep(0.2)
    
    # +k 境界値
    client.send_command(f"MODE {channel} +k a")  # 1文字
    time.sleep(0.2)
    client.send_command(f"MODE {channel} +k abcdefghijklmnopqrstuvwxyz1234567890abcdefghij")  # 50文字
    time.sleep(0.2)

def test_consecutive_operations(client, channel="#test"):
    """連続操作テスト"""
    print("\n=== 連続操作テスト ===")
    
    # 同じモードの連続設定
    for _ in range(3):
        client.send_command(f"MODE {channel} +i")
        time.sleep(0.1)
    
    # 同じモードの連続解除
    for _ in range(3):
        client.send_command(f"MODE {channel} -i")
        time.sleep(0.1)
    
    # 設定→解除の繰り返し
    for _ in range(3):
        client.send_command(f"MODE {channel} +i")
        time.sleep(0.1)
        client.send_command(f"MODE {channel} -i")
        time.sleep(0.1)

def main():
    if len(sys.argv) != 2:
        print("使用方法: python3 mode_comprehensive_test.py <port>")
        sys.exit(1)
    
    try:
        port = int(sys.argv[1])
    except ValueError:
        print("ポート番号は数値で指定してください")
        sys.exit(1)
    
    # メインクライアント（オペレーター）
    client1 = IRCTestClient(port=port, nickname="testuser1")
    if not client1.connect():
        sys.exit(1)
    
    # サブクライアント（一般ユーザー）
    client2 = IRCTestClient(port=port, nickname="testuser2")
    if not client2.connect():
        client1.disconnect()
        sys.exit(1)
    
    try:
        # 基本セットアップ
        print("=== 基本セットアップ ===")
        setup_client(client1, "testuser1")
        setup_client(client2, "testuser2")
        
        # 各種テスト実行
        test_basic_modes(client1)
        test_user_limit_variations(client1)
        test_channel_key_variations(client1)
        test_boundary_values(client1)
        test_consecutive_operations(client1)
        test_operator_privileges(client1, client2)
        test_error_cases(client1)
        
        print("\n=== 全テスト完了 ===")
        print(f"client1 受信メッセージ数: {len(client1.responses)}")
        print(f"client2 受信メッセージ数: {len(client2.responses)}")
        
    except KeyboardInterrupt:
        print("\n\nテストを中断しました")
    finally:
        client1.disconnect()
        client2.disconnect()

if __name__ == "__main__":
    main()
