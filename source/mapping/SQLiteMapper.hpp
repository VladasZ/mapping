//
//  SQLiteMapper.hpp
//  mapping
//
//  Created by Vladas Zakrevskis on 15/01/2020.
//  Copyright © 2020 VladasZ. All rights reserved.
//

#pragma once

#include "Mapper.hpp"

namespace mapping {

    class is_sqlite_mapper_cheker_base { };

    template <auto& mapper>
    class SQLiteMapper : is_sqlite_mapper_cheker_base {

        using Mapper = cu::remove_all_t<decltype(mapper)>;

        static_assert(is_mapper_v<Mapper>);

    public:

        template <class Class>
        static std::string create_table_command() {
            static_assert(_exists<Class>());
            std::string command = "CREATE TABLE IF NOT EXISTS ";

            mapper.template get_class_info<Class>([&](const auto& class_info) {

                command += class_info.name;
                command += " (\n";

                class_info.iterate_properties([&](const auto& property) {
                    command += property.name() + " " + property.database_type_name();
                    if (property.is_unique) {
                        command += " UNIQUE";
                    }
                    command += ",\n";
                });
            });

            command.pop_back();
            command.pop_back();

            command += "\n);";

            return command;
        }

    private:

        template <class Class>
        static constexpr bool _exists() {
            return mapper.template exists<Class>();
        }

    };

    template <class T> constexpr bool is_sqlite_mapper_v = std::is_base_of_v<is_sqlite_mapper_cheker_base, cu::remove_all_t<T>>;

}
