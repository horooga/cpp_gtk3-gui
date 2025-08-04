#include "cpp_gtk3-gui/utils.hpp"
#include <gdk/gdkkeysyms.h>
#include <gtkmm.h>

class NetworkInteractionSettings : public Gtk::Window {
    public:
        NetworkInteractionSettings()
            : newest_station_id(0), btn_new_station("Новая станция"),
              btn_save(""), btn_delete("") {

            setup_ui();
            setup_signals();
            setup_buttons();
            setup_accel_groups();
            setup_data();

            show_all_children();
        }

    protected:
        void add_form_row_to_box(const Glib::ustring &label_text,
                                 Gtk::Widget &widget, Gtk::Box &box) {
            auto hbox =
                Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 5));
            Gtk::Label *label = Gtk::manage(new Gtk::Label(label_text));
            label->set_xalign(0);
            label->set_margin_right(10);
            hbox->pack_start(*label, Gtk::PACK_SHRINK);
            hbox->pack_start(widget, Gtk::PACK_EXPAND_WIDGET);
            box.pack_start(*hbox, Gtk::PACK_SHRINK);
        }

        void on_tree_selection_changed() {
            auto sel = tree_view.get_selection();
            auto iter = sel->get_selected();
            if (!iter) {
                set_form_sensitive(false);
                return;
            }
            current_iter = iter;
            if (!is_station()) {
                set_form_sensitive(false);
                btn_save.set_sensitive(true);
                return;
            }

            set_form_sensitive(true);

            id_entry.set_text(std::to_string((*iter)[model_columns.col_id]));
            name_entry.set_text((*iter)[model_columns.col_name]);
            comments_entry.set_text((*iter)[model_columns.col_comments]);
            timeout0_spin.set_value((*iter)[model_columns.col_timeout0]);
            timeout1_spin.set_value((*iter)[model_columns.col_timeout1]);
            timeout2_spin.set_value((*iter)[model_columns.col_timeout2]);
            timeout3_spin.set_value((*iter)[model_columns.col_timeout3]);
            coeff_k_spin.set_value((*iter)[model_columns.col_coeff_k]);
            coeff_w_spin.set_value((*iter)[model_columns.col_coeff_w]);
            port_spin.set_value((*iter)[model_columns.col_port]);
            server_address1_entry.set_text(
                (*iter)[model_columns.col_server_address1]);
            server_address2_entry.set_text(
                (*iter)[model_columns.col_server_address2]);
            server_address3_entry.set_text(
                (*iter)[model_columns.col_server_address3]);
            client_address1_entry.set_text(
                (*iter)[model_columns.col_client_address1]);
            client_address2_entry.set_text(
                (*iter)[model_columns.col_client_address2]);
            client_address3_entry.set_text(
                (*iter)[model_columns.col_client_address3]);

            reset_field_styles();
        }

        void on_new_station_clicked() {
            id_entry.set_text(std::to_string(newest_station_id));
            name_entry.set_text("Новая станция");
            comments_entry.set_text("");
            port_spin.set_value(8000);
            timeout0_spin.set_value(5);
            timeout1_spin.set_value(15);
            timeout2_spin.set_value(10);
            timeout3_spin.set_value(10);
            coeff_k_spin.set_value(12);
            coeff_w_spin.set_value(8);
            server_address1_entry.set_text("127.0.0.1");
            server_address2_entry.set_text("");
            server_address3_entry.set_text("");
            client_address1_entry.set_text("127.0.0.1");
            client_address2_entry.set_text("");
            client_address3_entry.set_text("");

            set_form_sensitive(true);
            btn_delete.set_sensitive(false);

            reset_field_styles();
        }

        void on_save_clicked() {
            if (!is_station()) {
                return;
            }
            auto name = name_entry.get_text();
            if (!validate_name(name)) {
                show_error("Некорректное имя станции");
                return;
            }

            if (!validate_ip(server_address1_entry.get_text())) {
                show_error("Некорректный первый адрес сервера");
                return;
            } else if (!validate_ip(server_address2_entry.get_text()) &&
                       !server_address2_entry.get_text().empty()) {
                show_error("Некорректный второй адрес сервера");
                return;
            } else if (!validate_ip(server_address3_entry.get_text()) &&
                       !server_address3_entry.get_text().empty()) {
                show_error("Некорректный третий адрес сервера");
                return;
            }

            if (!validate_ip(client_address1_entry.get_text())) {
                show_error("Некорректный первый адрес клиента");
                return;
            } else if (!validate_ip(client_address2_entry.get_text()) &&
                       !client_address2_entry.get_text().empty()) {
                show_error("Некорректный второй адрес клиента");
                return;
            } else if (!validate_ip(client_address3_entry.get_text()) &&
                       !client_address3_entry.get_text().empty()) {
                show_error("Некорректный третий адрес клиента");
                return;
            }

            write_config(servers_row, model_columns, true);

            if (!current_iter) {
                auto row = *(tree_store->append(servers_row.children()));
                row[model_columns.col_id] = newest_station_id++;
                row[model_columns.col_name] = name_entry.get_text();
                row[model_columns.col_comments] = comments_entry.get_text();
                row[model_columns.col_timeout0] =
                    (int)timeout0_spin.get_value();
                row[model_columns.col_timeout1] =
                    (int)timeout1_spin.get_value();
                row[model_columns.col_timeout2] =
                    (int)timeout2_spin.get_value();
                row[model_columns.col_timeout3] =
                    (int)timeout3_spin.get_value();
                row[model_columns.col_coeff_k] = (int)coeff_k_spin.get_value();
                row[model_columns.col_coeff_w] = (int)coeff_w_spin.get_value();
                row[model_columns.col_port] = (int)port_spin.get_value();
                row[model_columns.col_server_address1] =
                    server_address1_entry.get_text();
                row[model_columns.col_server_address2] =
                    server_address2_entry.get_text();
                row[model_columns.col_server_address3] =
                    server_address3_entry.get_text();
                row[model_columns.col_client_address1] =
                    client_address1_entry.get_text();
                row[model_columns.col_client_address2] =
                    client_address2_entry.get_text();
                row[model_columns.col_client_address3] =
                    client_address3_entry.get_text();

                tree_view.get_selection()->select(row);
            } else {
                (*current_iter)[model_columns.col_name] = name_entry.get_text();
                (*current_iter)[model_columns.col_comments] =
                    comments_entry.get_text();
                (*current_iter)[model_columns.col_timeout0] =
                    (int)timeout0_spin.get_value();
                (*current_iter)[model_columns.col_timeout1] =
                    (int)timeout1_spin.get_value();
                (*current_iter)[model_columns.col_timeout2] =
                    (int)timeout2_spin.get_value();
                (*current_iter)[model_columns.col_timeout3] =
                    (int)timeout3_spin.get_value();
                (*current_iter)[model_columns.col_coeff_k] =
                    (int)coeff_k_spin.get_value();
                (*current_iter)[model_columns.col_coeff_w] =
                    (int)coeff_w_spin.get_value();
                (*current_iter)[model_columns.col_port] =
                    (int)port_spin.get_value();
                (*current_iter)[model_columns.col_server_address1] =
                    server_address1_entry.get_text();
                (*current_iter)[model_columns.col_server_address2] =
                    server_address2_entry.get_text();
                (*current_iter)[model_columns.col_server_address3] =
                    server_address3_entry.get_text();
                (*current_iter)[model_columns.col_client_address1] =
                    client_address1_entry.get_text();
                (*current_iter)[model_columns.col_client_address2] =
                    client_address2_entry.get_text();
                (*current_iter)[model_columns.col_client_address3] =
                    client_address3_entry.get_text();
            }

            write_config(servers_row, model_columns, false);
        }

        void on_delete_clicked() {
            if (!current_iter)
                return;

            tree_store->erase(current_iter);

            auto children = servers_row.children();
            if (!children.empty()) {
                tree_view.get_selection()->select(*(children.begin()));
            } else {
                set_form_sensitive(false);
                clear_form();
            }
        }

        void show_error(const Glib::ustring &message) {
            Gtk::MessageDialog dialog(*this, "Ошибка", false,
                                      Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK,
                                      true);
            dialog.set_secondary_text(message);
            dialog.run();
        }

        void set_widget_error(Gtk::Entry &entry) {
            entry.override_background_color(Gdk::RGBA("pink"));
        }

        void clear_widget_error(Gtk::Entry &entry) {
            entry.override_background_color(Gdk::RGBA("white"));
        }

        void reset_field_styles() {
            clear_widget_error(name_entry);
            clear_widget_error(server_address1_entry);
            clear_widget_error(server_address2_entry);
            clear_widget_error(server_address3_entry);
            clear_widget_error(client_address1_entry);
            clear_widget_error(client_address2_entry);
            clear_widget_error(client_address3_entry);
        }

        void set_form_sensitive(bool sensitive) {
            btn_save.set_sensitive(sensitive);
            btn_delete.set_sensitive(sensitive);
            vbox_form.set_sensitive(sensitive);
            if (!sensitive) {
                current_iter = Gtk::TreeModel::iterator();
            }
        }

        void clear_form() {
            id_entry.set_text("");
            name_entry.set_text("");
            port_spin.set_value(1);
            timeout0_spin.set_value(0);
            timeout1_spin.set_value(0);
            timeout2_spin.set_value(0);
            timeout3_spin.set_value(0);
            coeff_k_spin.set_value(0);
            coeff_w_spin.set_value(0);
            server_address1_entry.set_text("");
            server_address2_entry.set_text("");
            server_address3_entry.set_text("");
            client_address1_entry.set_text("");
            client_address2_entry.set_text("");
            client_address3_entry.set_text("");
        }

        bool is_station() {
            if (!current_iter)
                return false;
            return tree_store->get_path(current_iter).size() == 2;
        }

    private:
        void setup_ui() {
            set_title("Настройки сетевого взаимодействия");
            set_default_size(600, 400);
            add(vbox_main);
            top_bar.set_size_request(-1, 40);
            hbox_top.set_border_width(5);
            top_bar.add(hbox_top);
            hbox_top.pack_start(btn_new_station, Gtk::PACK_SHRINK);
            hbox_top.pack_start(btn_save, Gtk::PACK_SHRINK);
            hbox_top.pack_start(btn_delete, Gtk::PACK_SHRINK);
            hbox_top.pack_start(
                *Gtk::manage(new Gtk::Separator(Gtk::ORIENTATION_VERTICAL)),
                Gtk::PACK_EXPAND_WIDGET);
            vbox_main.pack_start(top_bar, Gtk::PACK_SHRINK);
            vbox_main.pack_start(hbox_main, Gtk::PACK_EXPAND_WIDGET);
            scrolled_tree.set_policy(Gtk::POLICY_AUTOMATIC,
                                     Gtk::POLICY_AUTOMATIC);
            scrolled_tree.set_min_content_width(200);
            hbox_main.pack_start(scrolled_tree, true, true);

            auto set_margin = [](Gtk::Widget &widget, int margin) {
                widget.set_margin_left(margin);
                widget.set_margin_top(margin);
                widget.set_margin_right(margin);
                widget.set_margin_bottom(margin);
            };

            tree_store = Gtk::TreeStore::create(model_columns);
            servers_row = *(tree_store)->append();
            servers_row[model_columns.col_name] = "Серверы";
            tree_view.set_model(tree_store);
            scrolled_tree.add(tree_view);
            tree_view.append_column("Название", model_columns.col_name);
            hbox_main.pack_start(vbox_form, true, true);
            frame_general_label.set_markup("<b>Общие</b>");
            frame_general.set_label_widget(frame_general_label);
            id_entry.set_editable(false);
            id_entry.set_hexpand(true);
            add_form_row_to_box("Идентификатор", id_entry, vbox_general);
            add_form_row_to_box("Название", name_entry, vbox_general);
            add_form_row_to_box("Комментарии", comments_entry, vbox_general);

            auto create_adjustment = [](double value, double lower,
                                        double upper, double step = 1) {
                return Gtk::Adjustment::create(value, lower, upper, step);
            };

            timeout0_spin.set_adjustment(
                create_adjustment(5, 0, std::numeric_limits<int>::max(), 1));
            add_form_row_to_box("Тайм-аут T0, сек", timeout0_spin, vbox_timers);
            timeout1_spin.set_adjustment(
                create_adjustment(15, 0, std::numeric_limits<int>::max(), 1));
            add_form_row_to_box("Тайм-аут T1, сек", timeout1_spin, vbox_timers);
            timeout2_spin.set_adjustment(
                create_adjustment(10, 0, std::numeric_limits<int>::max(), 1));
            add_form_row_to_box("Тайм-аут T2, сек", timeout2_spin, vbox_timers);
            timeout3_spin.set_adjustment(
                create_adjustment(10, 0, std::numeric_limits<int>::max(), 1));
            add_form_row_to_box("Тайм-аут T3, сек", timeout3_spin, vbox_timers);
            coeff_k_spin.set_adjustment(
                create_adjustment(12, 1, std::numeric_limits<int>::max(), 1));
            add_form_row_to_box("Коэффициент K", coeff_k_spin,
                                vbox_coeffs_port);
            coeff_w_spin.set_adjustment(
                create_adjustment(8, 1, std::numeric_limits<int>::max(), 1));
            add_form_row_to_box("Коэффициент W", coeff_w_spin,
                                vbox_coeffs_port);
            port_spin.set_adjustment(create_adjustment(8000, 1, 65535, 1));
            add_form_row_to_box("Порт", port_spin, vbox_coeffs_port);
            hbox_timers_coeffs_port.pack_start(vbox_timers,
                                               Gtk::PACK_EXPAND_WIDGET);
            hbox_timers_coeffs_port.pack_start(vbox_coeffs_port,
                                               Gtk::PACK_SHRINK);
            vbox_general.pack_start(hbox_timers_coeffs_port, Gtk::PACK_SHRINK);
            frame_general.add(vbox_general);
            vbox_form.pack_start(frame_general, Gtk::PACK_SHRINK);

            frame_server_label.set_markup("<b>Свойства TCP/IP для сервера</b>");
            frame_server.set_label_widget(frame_server_label);
            add_form_row_to_box("Адрес1", server_address1_entry, vbox_server);
            add_form_row_to_box("Адрес2", server_address2_entry, vbox_server);
            add_form_row_to_box("Адрес3", server_address3_entry, vbox_server);
            frame_server.add(vbox_server);
            vbox_form.pack_start(frame_server, Gtk::PACK_SHRINK);

            frame_client_label.set_markup("<b>Свойства TCP/IP для клиента</b>");
            frame_client.set_label_widget(frame_client_label);
            add_form_row_to_box("Адрес1", client_address1_entry, vbox_client);
            add_form_row_to_box("Адрес2", client_address2_entry, vbox_client);
            add_form_row_to_box("Адрес3", client_address3_entry, vbox_client);
            frame_client.add(vbox_client);
            vbox_form.pack_start(frame_client, Gtk::PACK_SHRINK);
            vbox_form.pack_start(spacer, Gtk::PACK_EXPAND_WIDGET);

            set_margin(frame_general, 10);
            set_margin(vbox_general, 10);
            set_margin(frame_timers_coeffs_port, 10);
            set_margin(hbox_timers_coeffs_port, 10);
            set_margin(vbox_timers, 10);
            set_margin(vbox_coeffs_port, 10);
            set_margin(frame_server, 10);
            set_margin(vbox_server, 10);
            set_margin(frame_client, 10);
            set_margin(vbox_client, 10);
        }

        void setup_signals() {
            tree_view.get_selection()->signal_changed().connect(sigc::mem_fun(
                *this, &NetworkInteractionSettings::on_tree_selection_changed));
            btn_new_station.signal_clicked().connect(sigc::mem_fun(
                *this, &NetworkInteractionSettings::on_new_station_clicked));
            btn_save.signal_clicked().connect(sigc::mem_fun(
                *this, &NetworkInteractionSettings::on_save_clicked));
            btn_delete.signal_clicked().connect(sigc::mem_fun(
                *this, &NetworkInteractionSettings::on_delete_clicked));
            name_entry.signal_changed().connect([this]() {
                auto text = name_entry.get_text();
                if (validate_name(text)) {
                    clear_widget_error(name_entry);
                } else {
                    set_widget_error(name_entry);
                }
            });
            server_address1_entry.signal_changed().connect([this]() {
                auto text = server_address1_entry.get_text();
                if (validate_ip(text)) {
                    clear_widget_error(server_address1_entry);
                } else {
                    set_widget_error(server_address1_entry);
                }
            });
            server_address2_entry.signal_changed().connect([this]() {
                auto text = server_address2_entry.get_text();
                if (validate_ip(text) && !text.empty()) {
                    clear_widget_error(server_address2_entry);
                } else {
                    set_widget_error(server_address2_entry);
                }
            });
            server_address3_entry.signal_changed().connect([this]() {
                auto text = server_address3_entry.get_text();
                if (validate_ip(text) && !text.empty()) {
                    clear_widget_error(server_address3_entry);
                } else {
                    set_widget_error(server_address3_entry);
                }
            });
            client_address1_entry.signal_changed().connect([this]() {
                auto text = client_address1_entry.get_text();
                if (validate_ip(text)) {
                    clear_widget_error(client_address1_entry);
                } else {
                    set_widget_error(client_address1_entry);
                }
            });
            client_address2_entry.signal_changed().connect([this]() {
                auto text = client_address2_entry.get_text();
                if (validate_ip(text) && !text.empty()) {
                    clear_widget_error(client_address2_entry);
                } else {
                    set_widget_error(client_address2_entry);
                }
            });
            client_address3_entry.signal_changed().connect([this]() {
                auto text = client_address3_entry.get_text();
                if (validate_ip(text) && !text.empty()) {
                    clear_widget_error(client_address3_entry);
                } else {
                    set_widget_error(client_address3_entry);
                }
            });
        }

        void setup_buttons() {
            btn_save.set_image_from_icon_name("media-floppy");
            btn_save.set_always_show_image(true);
            btn_delete.set_image_from_icon_name("edit-delete");
            btn_delete.set_always_show_image(true);
            btn_save.set_sensitive(false);
            btn_delete.set_sensitive(false);
            set_form_sensitive(false);
        }

        void setup_accel_groups() {
            auto accel_group = Gtk::AccelGroup::create();
            add_accel_group(accel_group);
            btn_save.add_accelerator("clicked", accel_group, GDK_KEY_s,
                                     Gdk::CONTROL_MASK, Gtk::ACCEL_VISIBLE);
            btn_delete.add_accelerator("clicked", accel_group, GDK_KEY_Delete,
                                       Gdk::ModifierType(0),
                                       Gtk::ACCEL_VISIBLE);
        }

        void setup_data() {
            parse_config(servers_row, tree_store, model_columns,
                         newest_station_id);
            if (newest_station_id == 0) {
                auto row = *(tree_store->append(servers_row.children()));
                row[model_columns.col_id] = newest_station_id++;
                row[model_columns.col_name] = "Новая станция";
                row[model_columns.col_comments] = "";
                row[model_columns.col_timeout0] =
                    (int)timeout0_spin.get_value();
                row[model_columns.col_timeout1] =
                    (int)timeout1_spin.get_value();
                row[model_columns.col_timeout2] =
                    (int)timeout2_spin.get_value();
                row[model_columns.col_timeout3] =
                    (int)timeout3_spin.get_value();
                row[model_columns.col_coeff_k] = (int)coeff_k_spin.get_value();
                row[model_columns.col_coeff_w] = (int)coeff_w_spin.get_value();
                row[model_columns.col_port] = (int)port_spin.get_value();
                row[model_columns.col_server_address1] = "127.0.0.1";
                row[model_columns.col_server_address2] = "";
                row[model_columns.col_server_address3] = "";
                row[model_columns.col_client_address1] = "127.0.0.1";
                row[model_columns.col_client_address2] = "";
                row[model_columns.col_client_address3] = "";

                tree_view.get_selection()->select(row);
            }
            tree_view.expand_row(tree_store->get_path(servers_row), false);
        }

        Gtk::Frame frame_general;
        Gtk::Label frame_general_label;
        Gtk::Box vbox_general{Gtk::ORIENTATION_VERTICAL, 10};
        Gtk::Frame frame_timers_coeffs_port;
        Gtk::Box hbox_timers_coeffs_port{Gtk::ORIENTATION_HORIZONTAL, 10};
        Gtk::Box vbox_timers{Gtk::ORIENTATION_VERTICAL, 10};
        Gtk::Box vbox_coeffs_port{Gtk::ORIENTATION_VERTICAL, 10};
        Gtk::Frame frame_server;
        Gtk::Label frame_server_label;
        Gtk::Box vbox_server{Gtk::ORIENTATION_VERTICAL, 10};
        Gtk::Frame frame_client;
        Gtk::Label frame_client_label;
        Gtk::Box vbox_client{Gtk::ORIENTATION_VERTICAL, 10};
        Gtk::Box vbox_main{Gtk::ORIENTATION_VERTICAL, 10};
        Gtk::Box hbox_top{Gtk::ORIENTATION_HORIZONTAL, 10};
        Gtk::Box vbox_form{Gtk::ORIENTATION_VERTICAL, 10};
        Gtk::Box hbox_main{Gtk::ORIENTATION_HORIZONTAL, 10};
        Gtk::Box top_bar{Gtk::ORIENTATION_HORIZONTAL, 10};

        Gtk::Button btn_new_station;
        Gtk::Button btn_save;
        Gtk::Button btn_delete;

        Gtk::Entry id_entry;
        Gtk::Entry name_entry;
        Gtk::Entry comments_entry;
        Gtk::SpinButton timeout0_spin;
        Gtk::SpinButton timeout1_spin;
        Gtk::SpinButton timeout2_spin;
        Gtk::SpinButton timeout3_spin;
        Gtk::SpinButton coeff_k_spin;
        Gtk::SpinButton coeff_w_spin;
        Gtk::SpinButton port_spin;
        Gtk::Entry server_address1_entry;
        Gtk::Entry server_address2_entry;
        Gtk::Entry server_address3_entry;
        Gtk::Entry client_address1_entry;
        Gtk::Entry client_address2_entry;
        Gtk::Entry client_address3_entry;

        Gtk::ScrolledWindow scrolled_tree;
        Gtk::TreeView tree_view;
        Glib::RefPtr<Gtk::TreeStore> tree_store;
        Gtk::TreeModel::Row servers_row;
        Gtk::TreeModel::iterator current_iter;
        unsigned int newest_station_id;
        Gtk::Box spacer{Gtk::ORIENTATION_VERTICAL};
        ModelColumns model_columns;
};

int main(int argc, char *argv[]) {
    auto app =
        Gtk::Application::create(argc, argv, "com.github.horooga.cpp_gtk3-gui");

    NetworkInteractionSettings window;

    return app->run(window);
}
