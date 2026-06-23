# 智慧物流與倉庫管理系統 (Smart Warehouse & Logistics Management System)

這是一個以 C++17 開發的終端機互動式 (TUI) 物流與倉庫管理系統。本系統專門設計用以展示 C++ 的**物件導向程式設計 (OOP)**、**類別繼承與多型**、**STL 類別庫**以及**檔案 I/O（持久化儲存）** 等核心技術。

---

## 🚀 專案特點與 C++ 技術呈現

### 1. 物件導向繼承與多型 (Inheritance & Polymorphism)
專案內的核心架構展現了深刻的 OOP 繼承與多型特性：
*   **貨物類別關係**：
    *   `Cargo`（基底類別）：定義貨物基本屬性（名稱、重量、容積、基本費率、擁有者）。
    *   `DangerousCargo`（危險品）：繼承自 `Cargo`，新增危險等級與 UN 編號。重寫 `calculateStorageFee()` 加上安全保險與監控費，重寫 `printDetails()` 顯示危險品警告。
    *   `PerishableCargo`（易腐品）：繼承自 `Cargo`，新增過期日與溫控 (°C) 屬性。依據溫控區間（如冷凍/冷藏）動態加收冷鏈電費。
    *   `FragileCargo`（易碎品）：繼承自 `Cargo`，新增包裝型態與堆疊層數限制。若高度限制太低，則加收倉庫地板佔用費。
*   **多型應用**：
    在 `Warehouse::calculateTotalRevenue()` 與 `TUI::listInventory()` 中，系統只需維護一個統一的基底指標容器 `std::vector<std::shared_ptr<Cargo>>`，便能在執行期（Runtime）透過**動態綁定 (Dynamic Binding)**，自動呼叫各衍生類別特定的費率計算與明細輸出。

### 2. 使用者權限管制 (Role-Based Access Control)
*   `User` 作為基底類別，衍生出 `AdminUser`（管理員）與 `RegularUser`（一般作業員）。
*   透過 `canManageInventory()` 與 `canManageUsers()` 虛擬方法，管制不同角色登入後的終端機選單權限。

### 3. STL 類別庫應用
*   **容器 (Containers)**：使用 `std::vector` 存放庫存列表，使用 `std::string` 處理字串。
*   **智慧指標 (Smart Pointers)**：全面使用 `std::shared_ptr` 與 `std::make_shared` 管理動態記憶體，防止記憶體洩漏 (Memory Leaks)。
*   **演算法 (Algorithms)**：使用 `std::find_if` 與 `Lambda 運算式` 進行高效率的貨物 ID 與帳號查找。

### 4. 檔案讀寫與持久化儲存 (File I/O)
*   使用 `std::ifstream` 與 `std::ofstream` 實作資料的儲存與讀取。
*   採用簡易的 **CSV 格式**，程式啟動時會解析逗號分隔值並**動態重構 (Downcasting/Deserialization)** 回正確的子類別物件實例；關閉時會自動寫入 `data/cargo.txt` 與 `data/users.txt`。
*   **防呆機制**：若系統找不到 `users.txt`，會自動寫入預設的管理員帳密，避免系統崩潰。

### 5. 精美終端機 UI (TUI) 與防呆
*   使用 **ANSI Escape Codes** 彩色化輸出終端機介面（危險品紅字警示、成功綠字提示等）。
*   實作健壯的輸入防呆函式（`readInt` 與 `readDouble`），防止使用者輸入中文字元或英文字母時，導致標準輸入流 `cin` 發生無窮迴圈或崩潰。

---

## 📁 檔案結構說明

```text
├── CMakeLists.txt         # CMake 編譯設定檔
├── README.md              # 系統開發說明文件 (本檔案)
├── data/
│   ├── cargo.txt          # 貨物資料庫 (CSV 格式)
│   └── users.txt          # 使用者帳號資料庫 (CSV 格式)
└── src/
    ├── Cargo.h / .cpp           # 貨物基底類別
    ├── DangerousCargo.h / .cpp  # 危險品衍生類別
    ├── PerishableCargo.h / .cpp # 易腐品衍生類別
    ├── FragileCargo.h / .cpp    # 易碎品衍生類別
    ├── User.h / .cpp            # 使用者權限管制類別 (基底與衍生)
    ├── Warehouse.h / .cpp       # 倉庫核心管理類別 (STL 容器與統計邏輯)
    ├── Database.h / .cpp        # 檔案 I/O 資料讀寫處理器
    ├── TUI.h / .cpp             # 互動式彩色終端機介面
    └── main.cpp                 # 系統程式主入口
```

---

## 🛠️ 如何編譯與執行

本專案支援 CMake 編譯，或直接使用 g++ 編譯。

### 方法一：使用 g++ 直接編譯 (推薦，極速簡便)
在專案根目錄下打開終端機（PowerShell 或 CMD），輸入以下指令：
```bash
g++ -std=c++17 src/*.cpp -o WarehouseSystem
```
編譯完成後，執行產生的執行檔：
```bash
.\WarehouseSystem.exe
```

### 方法二：使用 CMake 編譯
```bash
cmake -B build
cmake --build build
```
編譯完成後，執行檔將位於 `build/` 目錄中。

---

## 🔑 預設登入帳密

系統已預載兩組帳號供您展示：

1.  **管理員帳密** (具備完整權限、新增使用者與檢視統計圖表功能)：
    *   帳號：`admin`
    *   密碼：`admin123`
2.  **一般作業員帳密** (僅能查看/申報/出庫自己名下的貨物)：
    *   帳號：`user`
    *   密碼：`user123`
