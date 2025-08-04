#include "gtkmm.h"

class ModelColumns : public Gtk::TreeModel::ColumnRecord {
    public:
        ModelColumns() {
            add(col_id);
            add(col_name);
            add(col_comments);
            add(col_timeout0);
            add(col_timeout1);
            add(col_timeout2);
            add(col_timeout3);
            add(col_coeff_k);
            add(col_coeff_w);
            add(col_port);
            add(col_server_address1);
            add(col_server_address2);
            add(col_server_address3);
            add(col_client_address1);
            add(col_client_address2);
            add(col_client_address3);
        }

        Gtk::TreeModelColumn<unsigned int> col_id;
        Gtk::TreeModelColumn<Glib::ustring> col_name;
        Gtk::TreeModelColumn<Glib::ustring> col_comments;
        Gtk::TreeModelColumn<int> col_port;
        Gtk::TreeModelColumn<int> col_timeout0;
        Gtk::TreeModelColumn<int> col_timeout1;
        Gtk::TreeModelColumn<int> col_timeout2;
        Gtk::TreeModelColumn<int> col_timeout3;
        Gtk::TreeModelColumn<int> col_coeff_k;
        Gtk::TreeModelColumn<int> col_coeff_w;
        Gtk::TreeModelColumn<Glib::ustring> col_server_address1;
        Gtk::TreeModelColumn<Glib::ustring> col_server_address2;
        Gtk::TreeModelColumn<Glib::ustring> col_server_address3;
        Gtk::TreeModelColumn<Glib::ustring> col_client_address1;
        Gtk::TreeModelColumn<Glib::ustring> col_client_address2;
        Gtk::TreeModelColumn<Glib::ustring> col_client_address3;
};

bool validate_name(const Glib::ustring &name);
bool validate_ip(const Glib::ustring &ip);

void parse_config(const Gtk::TreeModel::Row &servers_row,
                  const Glib::RefPtr<Gtk::TreeStore> &tree_store,
                  const ModelColumns &model_columns,
                  unsigned int &newest_station_id);

void write_config(const Gtk::TreeModel::Row &servers_row,
                  const ModelColumns &model_columns, bool is_backup);
