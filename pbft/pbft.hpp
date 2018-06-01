// Copyright (C) 2018 Bluzelle
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License, version 3,
// as published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <include/bluzelle.hpp>
#include <pbft/pbft_base.hpp>
#include <pbft/pbft_operation.hpp>

namespace bzn {

    class pbft final : public bzn::pbft_base {
    public:
        pbft(
                std::shared_ptr<bzn::node_base> node,
                const bzn::peers_list_t &peers
        );

        void start() override;

        bool is_primary() override;

        const peer_address_t &get_primary() override;

        void handle_message(const bzn::message &msg, std::shared_ptr<bzn::session_base> session) override;


    private:
        uint64_t view = 0;
        uint64_t next_issued_sequence_number = 0;

        std::shared_ptr<bzn::node_base> node;
        const bzn::peers_list_t peers;

        void create_operation(const uint64_t& view, const uint64_t& sequence, const bzn::message& request);

        std::map<bzn::operation_key_t, bzn::pbft_operation> operations;
    };

} // namespace bzn

