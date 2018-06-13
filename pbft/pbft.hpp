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

namespace bzn {

    class pbft final : public bzn::pbft_base {
    public:
        pbft(
                std::shared_ptr<bzn::node_base> node,
                const bzn::peers_list_t &peers
        );

        void start() override;

        void handle_message(const pbft_msg& msg) override;

        size_t outstanding_operations_count() const;

        bool is_primary() const override;

        const peer_address_t &get_primary() const override;

    private:
        // Using 1 as first value here to distinguish from default value of 0 in protobuf
        uint64_t view = 1;
        uint64_t next_issued_sequence_number = 1;

        uint64_t low_water_mark = 1;
        uint64_t high_water_mark = UINT64_MAX;

        std::shared_ptr<bzn::node_base> node;
        const bzn::peers_list_t peers;

        std::map<bzn::operation_key_t, bzn::pbft_operation, bzn::operation_key_comparator> operations;
        std::map<bzn::log_key_t, bzn::operation_key_t> accepted_preprepares;

        pbft_operation & find_operation(const uint64_t &view, const uint64_t &sequence, const pbft_request &request);

        void handle_request(const pbft_msg& msg);
        void handle_preprepare(const pbft_msg& msg);

        void do_preprepare(pbft_operation& op);

        bzn::message wrap_message(pbft_msg& message);

    };

} // namespace bzn

