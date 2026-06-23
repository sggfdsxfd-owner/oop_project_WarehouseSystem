#include "Warehouse.h"
#include "Database.h"
#include "TUI.h"
#include <cstdlib>

int main() {
#ifdef _WIN32
    // 設定 Windows 終端機編碼為 UTF-8，以正確顯示中文防止亂碼
    std::system("chcp 65001 > nul");
#endif

    // 初始化系統與資料庫
    Warehouse warehouse;
    Database database("data/users.txt", "data/cargo.txt");
    
    // 初始化終端機介面並啟動主循環
    TUI tui(warehouse, database);
    tui.run();
    
    return 0;
}
