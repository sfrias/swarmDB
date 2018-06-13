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
#include <utils/make_endpoint.hpp>
#include <cstdint>
#include <memory>

using namespace bzn;

pbft::pbft(std::shared_ptr<bzn::node_base> node, const bzn::peers_list_t &peers)
        : node(std::move(node)), peers(peers) {
    if (this->peers.empty()) {
        throw std::runtime_error("No peers found!");
    }

}

void
pbft::start() {

}

void
pbft::handle_message(const pbft_msg &msg) {

    LOG(debug) << "Recieved message:\n" << msg.DebugString();

    //TODO: conditionally discard based on timestamp - KEP-328

    //TODO: keep track of what requests we've seen based on timestamp and only send preprepares once - KEP-329

    //TODO: pbft says messages (other than requests) should always have view/seq numbers, but byzantine clients
    // can fail to include them. Therefore, we need to validate each message - KEP-375


    // First, look up our data on the operation -- creating the record if this is the first time we've heard of it

    if (msg.has_request()){
        this->handle_request(msg);
    } else if (msg.has_preprepare()){
        this->handle_preprepare(msg);
    } else {
        throw "Unsupported message type";
    }
}

void pbft::handle_request(const pbft_msg& msg) {
    if (!this->is_primary()) {
        LOG(error) << "Ignoring client request because I am not the leader";
        // TODO - KEP-327
        return;
    }

    uint64_t request_view = this->view;
    uint64_t request_seq = this->next_issued_sequence_number++;
    pbft_operation &op = this->find_operation(request_view, request_seq, msg.request());

    do_preprepare(op);
}

void pbft::handle_preprepare(const pbft_msg& msg) {
    msg.has_preprepare();

    //uint64_t request_view = msg.view();
    //uint64_t request_sequence = msg.sequence();

    //pbft_operation &op = this->find_operation(msg.view(), msg.sequence(), msg.request());
}

void pbft::do_preprepare(pbft_operation &op) {
    pbft_msg msg;
    msg.set_allocated_preprepare(new pbft_preprepare());

    msg.set_view(op.view);
    msg.set_sequence(op.sequence);
    pbft_request* request = new pbft_request(op.request);
    msg.mutable_preprepare()->set_allocated_request(request);

    for (const auto &peer : this->peers) {
        this->node->send_message(make_endpoint(peer), std::make_shared<bzn::message>(this->wrap_message(msg)));
    }

}

size_t
pbft::outstanding_operations_count() const {
    return operations.size();
}

bool
pbft::is_primary() const {
    return true;
}

const peer_address_t &
pbft::get_primary() const {
    throw "not implemented";
}
// Find this node's record of an operation (creating it if this is the first time we've heard of it)

pbft_operation &
pbft::find_operation(const uint64_t &view, const uint64_t &sequence, const pbft_request &request) {
    bzn::operation_key_t key = std::tuple<uint64_t, uint64_t, pbft_request>(view, sequence, request);
    if (operations.count(key) == 0) {
        operations.emplace(std::piecewise_construct, std::forward_as_tuple(key),
                           std::forward_as_tuple(view, sequence, request));
    }

    pbft_operation& result_ptr = operations.find(key) -> second;

    return result_ptr;
}

bzn::message
pbft::wrap_message(pbft_msg &msg) {
    bzn::message json;
    json["bzn-api"] = "pbft";
    json["pbft-data"] = msg.SerializeAsString();

    return json;
}
