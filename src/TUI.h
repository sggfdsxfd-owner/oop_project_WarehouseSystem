#ifndef TUI_H
#define TUI_H

#include "Warehouse.h"
#include "Database.h"

class TUI {
private:
    Warehouse& warehouse;
    Database& database;

    // ANSI 色彩與格式輔助字串
    static const std::string COLOR_RESET;
    static const std::string COLOR_BOLD;
    static const std::string COLOR_RED;
    static const std::string COLOR_GREEN;
    static const std::string COLOR_YELLOW;
    static const std::string COLOR_BLUE;
    static const std::string COLOR_CYAN;

    // 畫面控制
    void clearScreen() const;
    void printHeader(const std::string& title, bool center = false) const;
    void printBanner() const;
    int selectMenu(const std::string& headerTitle, const std::vector<std::string>& options) const;

    // 安全輸入函式
    std::string readString(const std::string& prompt) const;
    int readInt(const std::string& prompt) const;
    double readDouble(const std::string& prompt) const;

    // 各選單流程
    void handleLogin(bool requireAdmin);
    void showAdminMenu();
    void showRegularMenu();

    // 功能子程序
    void listInventory(bool showAll = true) const; // true 顯示全部(Admin)，false 僅顯示目前使用者(Regular)
    void addCargoFlow();
    void removeCargoFlow();
    void showStatistics() const;
    void addUserFlow();

public:
    TUI(Warehouse& wh, Database& db);
    void run();
};

#endif // TUI_H
