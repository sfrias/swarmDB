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

#include "pbft_operation.hpp"
#include <boost/format.hpp>

using namespace bzn;

bool
operation_key_comparator::operator()(const operation_key_t& a, const operation_key_t& b) const
{
    if(std::get<0>(b) > std::get<0>(a)) return true;
    if(std::get<1>(b) > std::get<1>(a)) return true;
    if(std::get<2>(b).operation() > std::get<2>(a).operation()) return true;
    if(std::get<2>(b).timestamp() > std::get<2>(a).timestamp()) return true;
    if(std::get<2>(b).client() > std::get<2>(a).client()) return true;

    return false;
}

pbft_operation::pbft_operation(uint64_t view, uint64_t sequence, pbft_request request, const peers_list_t& peers)
        : view(std::move(view))
        , sequence(std::move(sequence))
        , request(std::move(request))
        , peers(peers) {
}

void pbft_operation::record_preprepare() {
    this->preprepare_seen = true;
}

bool pbft_operation::has_preprepare() {
    return this->preprepare_seen;
}

void pbft_operation::record_prepare(const pbft_msg& prepare) {
    this->prepares_seen.insert(prepare.sender());
}

bool pbft_operation::is_prepared() {
    size_t f = (this->peers.size() - 1) / 3;
    return this->has_preprepare() && this->prepares_seen.size() >= (2*f + 1);
}

void pbft_operation::record_commit(const pbft_msg& commit) {
    this->commits_seen.insert(commit.sender());
}

bool pbft_operation::is_committed() {
    size_t f = (this->peers.size() - 1) / 3;
    return this->is_prepared() && this->commits_seen.size() >= (2*f + 1);
}

void pbft_operation::begin_commit_phase() {
    if(!this->is_prepared() || this->state != pbft_operation_state::prepare) {
        throw "Illegaly tried to move to commit phase";
    }

    this->state = pbft_operation_state::commit;
}

void pbft_operation::end_commit_phase() {
    if(!this->is_committed() || this->state != pbft_operation_state::commit) {
        throw "Illegally tried to end the commit phase";
    }

    this->state = pbft_operation_state::committed;
}

operation_key_t pbft_operation::get_operation_key() {
    return std::tuple<uint64_t, uint64_t, pbft_request>(this->view, this->sequence, this->request);
}

pbft_operation_state pbft_operation::get_state() {
    return this->state;
}

std::string pbft_operation::debug_string() {
    return str(boost::format("(v%1%, s%2%) - %3%") % this->view % this->sequence % this->request.ShortDebugString());
}
