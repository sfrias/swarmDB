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

#include <pbft/pbft.hpp>
#include <cstdint>

using namespace bzn;

pbft::pbft(std::shared_ptr<bzn::node_base> node, const bzn::peers_list_t &peers)
        : node(std::move(node)), peers(peers) {
    if (this->peers.empty()) {
        throw std::runtime_error("No peers found!");
    }

}

void
pbft::start(){

}

void
pbft::handle_message(const  pbft_msg& msg) {

    LOG(debug) << "Recieved message:\n" << msg.DebugString();

    if (!this->is_primary()) {
        LOG(error) << "Ignoring client request because I am not the leader";
        // TODO - KEP-327
        return;
    }

    //TODO: conditionally discard based on timestamp - KEP-328
    //TODO: keep track of what requests we've seen based on timestamp and only send preprepares once - KEP-329

    // For now, we assume messages are requests

    uint64_t request_view = this->view;
    uint64_t request_seq = this->next_issued_sequence_number++;

    this->create_operation(request_view, request_seq, msg.request());

}

size_t
pbft::outstanding_operations_count() const {
    return operations.size();
}

bool
pbft::is_primary() const {
    return true;
}

const peer_address_t&
pbft::get_primary() const {
    throw "not implemented";
}

void
pbft::create_operation(const uint64_t& view, const uint64_t& sequence, const pbft_request& request) {
    bzn::operation_key_t key = std::tuple<uint64_t, uint64_t, pbft_request>(view, sequence, request);
    if(operations.count(key) == 0){
        operations.emplace(std::piecewise_construct, std::forward_as_tuple(key), std::forward_as_tuple(view, sequence, request));
    }
}
