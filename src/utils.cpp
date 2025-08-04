#include "cpp_gtk3-gui/utils.hpp"
#include <cstdlib>
#include <fstream>
#include <glibmm/ustring.h>
#include <gtkmm.h>
#include <iostream>
#include <regex>
#include <string>

std::string trim(const std::string &s) {
    auto start = s.begin();
    while (start != s.end() && std::isspace(*start)) {
        start++;
    }

    auto end = s.end();
    do {
        end--;
    } while (std::distance(start, end) > 0 && std::isspace(*end));

    return std::string(start, end + 1);
}

bool validate_name(const Glib::ustring &name) {
    const char *whitespace = " \t\n\r\f\v";
    auto start = name.find_first_not_of(whitespace);
    if (start == Glib::ustring::npos)
        return false;

    auto end = name.find_last_not_of(whitespace);
    Glib::ustring trimmed = name.substr(start, end - start + 1);
    return !trimmed.empty();
}

bool validate_ip(const Glib::ustring &ip) {

    const char *pattern =
        R"(^(25[0-5]|2[0-4]\d|1\d{2}|[1-9]\d?|0)\.(25[0-5]|2[0-4]\d|1\d{2}|[1-9]\d?|0)\.(25[0-5]|2[0-4]\d|1\d{2}|[1-9]\d?|0)\.(25[0-5]|2[0-4]\d|1\d{2}|[1-9]\d?|0)$)";
    std::regex ip_regex(pattern);
    return std::regex_match(ip.raw(), ip_regex);
}

std::pair<std::string, std::string> parse_value(const std::string &line) {
    int delimPos = line.find('=');
    return {line.substr(0, delimPos), line.substr(delimPos + 1)};
}

void parse_config(const Gtk::TreeModel::Row &servers_row,
                  const Glib::RefPtr<Gtk::TreeStore> &tree_store,
                  const ModelColumns &model_columns,
                  unsigned int &newest_station_id) {
    const char *home = std::getenv("HOME");
    if (!home) {
        std::cerr << "Переменная окружения $HOME не установлена.\n";
        std::exit(1);
    }

    std::string filePath = std::string(home) + "/.Stations.ini";
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return;
    }

    bool has_station = false;

    std::string name;
    std::string comments;
    int timeout0 = 5;
    int timeout1 = 15;
    int timeout2 = 10;
    int timeout3 = 20;
    int coeff_k = 8;
    int coeff_w = 12;
    int port = 8000;
    std::string server_address1 = "127.0.0.1";
    std::string server_address2 = "";
    std::string server_address3 = "";
    std::string client_address1 = "127.0.0.1";
    std::string client_address2 = "";
    std::string client_address3 = "";

    auto append_station = [&]() {
        auto row = *(tree_store->append(servers_row.children()));
        row[model_columns.col_id] = newest_station_id++;
        row[model_columns.col_name] = Glib::ustring(name);
        row[model_columns.col_comments] = Glib::ustring(comments);
        row[model_columns.col_timeout0] = timeout0;
        row[model_columns.col_timeout1] = timeout1;
        row[model_columns.col_timeout2] = timeout2;
        row[model_columns.col_timeout3] = timeout3;
        row[model_columns.col_coeff_k] = coeff_k;
        row[model_columns.col_coeff_w] = coeff_w;
        row[model_columns.col_port] = port;
        row[model_columns.col_server_address1] = Glib::ustring(server_address1);
        row[model_columns.col_server_address2] = Glib::ustring(server_address2);
        row[model_columns.col_server_address3] = Glib::ustring(server_address3);
        row[model_columns.col_client_address1] = Glib::ustring(client_address1);
        row[model_columns.col_client_address2] = Glib::ustring(client_address2);
        row[model_columns.col_client_address3] = Glib::ustring(client_address3);
    };

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#')
            continue;

        if (line.find('=') == std::string::npos) {
            if (has_station) {
                append_station();
            }
            name = line;
            comments = "";
            timeout0 = 5;
            timeout1 = 15;
            timeout2 = 10;
            timeout3 = 20;
            coeff_k = 8;
            coeff_w = 12;
            port = 8000;
            server_address1 = "127.0.0.1";
            client_address1 = "127.0.0.1";
            has_station = true;
        } else {
            auto [key, value] = parse_value(line);
            if (key == "comments") {
                comments = value;
            } else if (key == "timeout0") {
                timeout0 = std::stoi(value);
            } else if (key == "timeout1") {
                timeout1 = std::stoi(value);
            } else if (key == "timeout2") {
                timeout2 = std::stoi(value);
            } else if (key == "timeout3") {
                timeout3 = std::stoi(value);
            } else if (key == "coeff_k") {
                coeff_k = std::stoi(value);
            } else if (key == "coeff_w") {
                coeff_w = std::stoi(value);
            } else if (key == "port") {
                port = std::stoi(value);
            } else if (key == "server_address1") {
                server_address1 = value;
            } else if (key == "server_address2") {
                server_address2 = value;
            } else if (key == "server_address3") {
                server_address3 = value;
            } else if (key == "client_address1") {
                client_address1 = value;
            } else if (key == "client_address2") {
                client_address2 = value;
            } else if (key == "client_address3") {
                client_address3 = value;
            } else {
                std::cerr << "Внимание: неизвестный ключ '" << key
                          << "' в файле конфигурации.\n";
            }
        }
    }

    if (has_station) {
        append_station();
    }

    file.close();
}

void write_config(const Gtk::TreeModel::Row &servers_row,
                  const ModelColumns &model_columns, bool is_backup) {
    const char *home = std::getenv("HOME");
    if (!home) {
        std::cerr << "Переменная окружения $HOME не установлена.\n";
        return;
    }

    std::string file_path =
        std::string(home) + (is_backup ? "/.Stations.bak" : "/.Stations.ini");
    std::ofstream file(file_path);
    if (!file.is_open()) {
        return;
    }

    auto stations = servers_row->children();
    for (auto iter = stations.begin(); iter != stations.end(); ++iter) {
        auto row = *iter;

        unsigned int id = row[model_columns.col_id];

        std::string name =
            static_cast<Glib::ustring>(row[model_columns.col_name]).raw();
        std::string comments =
            static_cast<Glib::ustring>(row[model_columns.col_comments]).raw();
        int timeout0 = row[model_columns.col_timeout0];
        int timeout1 = row[model_columns.col_timeout1];
        int timeout2 = row[model_columns.col_timeout2];
        int timeout3 = row[model_columns.col_timeout3];
        int coeff_k = row[model_columns.col_coeff_k];
        int coeff_w = row[model_columns.col_coeff_w];
        int port = row[model_columns.col_port];
        std::string server_address1 =
            static_cast<Glib::ustring>(row[model_columns.col_server_address1])
                .raw();
        std::string server_address2 =
            static_cast<Glib::ustring>(row[model_columns.col_server_address2])
                .raw();
        std::string server_address3 =
            static_cast<Glib::ustring>(row[model_columns.col_server_address3])
                .raw();
        std::string client_address1 =
            static_cast<Glib::ustring>(row[model_columns.col_client_address1])
                .raw();
        std::string client_address2 =
            static_cast<Glib::ustring>(row[model_columns.col_client_address2])
                .raw();
        std::string client_address3 =
            static_cast<Glib::ustring>(row[model_columns.col_client_address3])
                .raw();

        file << name << "\n";
        file << "comments=" << comments << "\n";
        file << "timeout0=" << timeout0 << "\n";
        file << "timeout1=" << timeout1 << "\n";
        file << "timeout2=" << timeout2 << "\n";
        file << "timeout3=" << timeout3 << "\n";
        file << "coeff_k=" << coeff_k << "\n";
        file << "coeff_w=" << coeff_w << "\n";
        file << "port=" << port << "\n";
        file << "server_address1=" << server_address1 << "\n";
        file << "server_address2=" << server_address2 << "\n";
        file << "server_address3=" << server_address3 << "\n";
        file << "client_address1=" << client_address1 << "\n";
        file << "client_address2=" << client_address2 << "\n";
        file << "client_address3=" << client_address3 << "\n";
    }

    file.close();
}
