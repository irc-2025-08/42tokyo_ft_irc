#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
MODEコマンド インタラクティブテストスクリプト
使用方法: python3 mode_interactive_test.py <port>
"""

import socket
import sys
import time
import threading

class IRCTestClient:
    def __init__(self, host='localhost', port=6667):
        self.host = host
        self.port = port
        self.socket = None
        self.connected = False
        
    def connect(self):
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.connect((self.host, self.port))
            self.connected = True
            print(f"サーバー {self.host}:{self.port} に接続しました")
            
            # 受信スレッドを開始
            self.receive_thread = threading.Thread(target=self.receive_messages)
            self.receive_thread.daemon = True
            self.receive_thread.start()
            
            return True
        except Exception as e:
            print(f"接続エラー: {e}")
            return False
    
    def send_command(self, command):
        if not self.connected:
            print("サーバーに接続されていません")
            return False
        
        try:
            message = command + "\r\n"
            self.socket.send(message.encode('utf-8'))
            print(f"送信: {command}")
            return True
        except Exception as e:
            print(f"送信エラー: {e}")
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
                        print(f"受信: {line}")
                        
            except Exception as e:
                if self.connected:
                    print(f"受信エラー: {e}")
                break
    
    def disconnect(self):
        self.connected = False
        if self.socket:
            self.socket.close()
        print("接続を切断しました")

def print_menu():
    print("\n=== MODEコマンド テストメニュー ===")
    print("1. 基本セットアップ (NICK, USER, JOIN)")
    print("2. +i (招待制設定)")
    print("3. -i (招待制解除)")
    print("4. +t (トピック制限設定)")
    print("5. -t (トピック制限解除)")
    print("6. +l <数値> (ユーザー制限設定)")
    print("7. -l (ユーザー制限解除)")
    print("8. +k <キー> (チャンネルキー設定)")
    print("9. -k (チャンネルキー削除)")
    print("10. +o <ニックネーム> (オペレーター権限付与)")
    print("11. -o <ニックネーム> (オペレーター権限剥奪)")
    print("12. カスタムコマンド入力")
    print("13. エラーケーステスト")
    print("0. 終了")
    print("================================")

def run_error_tests(client):
    print("\n=== エラーケーステスト ===")
    
    error_tests = [
        ("パラメータ不足", "MODE #test"),
        ("存在しないチャンネル", "MODE #nonexistent +i"),
        ("複数モード指定", "MODE #test +it"),
        ("無効なモード文字", "MODE #test +x"),
        ("モード指定なし", "MODE #test +"),
        ("+l パラメータなし", "MODE #test +l"),
        ("+l 無効な値", "MODE #test +l abc"),
        ("+l 範囲外の値", "MODE #test +l 1000"),
        ("+k パラメータなし", "MODE #test +k"),
        ("+o パラメータなし", "MODE #test +o"),
        ("+o 存在しないユーザー", "MODE #test +o nonexistentuser"),
    ]
    
    for test_name, command in error_tests:
        print(f"\nテスト: {test_name}")
        client.send_command(command)
        time.sleep(1)  # レスポンスを待つ

def main():
    if len(sys.argv) != 2:
        print("使用方法: python3 mode_interactive_test.py <port>")
        sys.exit(1)
    
    try:
        port = int(sys.argv[1])
    except ValueError:
        print("ポート番号は数値で指定してください")
        sys.exit(1)
    
    client = IRCTestClient(port=port)
    
    if not client.connect():
        sys.exit(1)
    
    try:
        while True:
            print_menu()
            choice = input("選択してください (0-13): ").strip()
            
            if choice == '0':
                break
            elif choice == '1':
                print("基本セットアップを実行します...")
                client.send_command("NICK testuser")
                time.sleep(0.5)
                client.send_command("USER testuser 0 * :Test User")
                time.sleep(0.5)
                client.send_command("JOIN #test")
            elif choice == '2':
                client.send_command("MODE #test +i")
            elif choice == '3':
                client.send_command("MODE #test -i")
            elif choice == '4':
                client.send_command("MODE #test +t")
            elif choice == '5':
                client.send_command("MODE #test -t")
            elif choice == '6':
                limit = input("ユーザー制限数を入力してください: ").strip()
                client.send_command(f"MODE #test +l {limit}")
            elif choice == '7':
                client.send_command("MODE #test -l")
            elif choice == '8':
                key = input("チャンネルキーを入力してください: ").strip()
                client.send_command(f"MODE #test +k {key}")
            elif choice == '9':
                client.send_command("MODE #test -k")
            elif choice == '10':
                nickname = input("オペレーター権限を付与するニックネームを入力してください: ").strip()
                client.send_command(f"MODE #test +o {nickname}")
            elif choice == '11':
                nickname = input("オペレーター権限を剥奪するニックネームを入力してください: ").strip()
                client.send_command(f"MODE #test -o {nickname}")
            elif choice == '12':
                command = input("カスタムコマンドを入力してください: ").strip()
                client.send_command(command)
            elif choice == '13':
                run_error_tests(client)
            else:
                print("無効な選択です")
            
            time.sleep(1)  # レスポンスを待つ
            
    except KeyboardInterrupt:
        print("\n\nテストを中断しました")
    finally:
        client.disconnect()

if __name__ == "__main__":
    main()
