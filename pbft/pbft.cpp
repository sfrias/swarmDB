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

    switch(msg.type()){
        case PBFT_MSG_TYPE_REQUEST :
            this->handle_request(msg);
            break;
        case PBFT_MSG_TYPE_PREPREPARE :
            this->handle_preprepare(msg);
            break;
        default :
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

    this->do_preprepare(op);
}

void pbft::handle_preprepare(const pbft_msg& msg) {
    uint64_t request_view = msg.view();
    uint64_t request_sequence = msg.sequence();
    log_key_t log_key(request_view, request_sequence);

    pbft_operation& op = this->find_operation(request_view, request_sequence, msg.request());

    if (request_view != this->view) {
        LOG(debug) << "Rejecting preprepare because its view " << request_view << " does not match my view " << this->view << "\n";
        return;
    }

    // If we've already accepted a preprepare for this view+sequence, and it's not this one
    // Note that if we get the same preprepare more than once, we can still accept it
    auto lookup = this->accepted_preprepares.find(log_key);
    if (lookup != this->accepted_preprepares.end()
        && std::get<2>(lookup->second).SerializeAsString() != msg.request().SerializeAsString()) {

        LOG(debug) << "Rejecting preprepare because I've already accepted a conflicting one \n";
        return;
    }

    if (request_sequence < this->low_water_mark || request_sequence > this->high_water_mark) {
        LOG(debug) << "Rejecting preprepare because its sequence number " << request_sequence << " is unreasonable \n";
        return;
    }

    // At this point we've decided to accept the preprepare

    if (lookup != this->accepted_preprepares.end()) {
        accepted_preprepares[log_key] = operation_key_t(request_view, request_sequence, msg.request());
    }

    this->do_prepare(op);
}

void pbft::broadcast(const pbft_msg& msg) {
    auto json_ptr = std::make_shared<bzn::message>(this->wrap_message(msg));

    for (const auto &peer : this->peers) {
        this->node->send_message(make_endpoint(peer), json_ptr);
    }
}

void pbft::do_preprepare(pbft_operation &op) {
    LOG(debug) << "Doing preprepare for operation " << op.debug_string();
    pbft_msg msg;

    msg.set_type(PBFT_MSG_TYPE_PREPREPARE);
    msg.set_view(op.view);
    msg.set_sequence(op.sequence);
    msg.set_allocated_request(new pbft_request(op.request));

    this->broadcast(msg);
}

void pbft::do_prepare(pbft_operation &op) {
    LOG(debug) << "Doing prepare for operation " << op.debug_string();

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
pbft::wrap_message(const pbft_msg &msg) {
    bzn::message json;
    json["bzn-api"] = "pbft";
    json["pbft-data"] = msg.SerializeAsString();

    return json;
}
