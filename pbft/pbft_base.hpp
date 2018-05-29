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

#include <string>

#include "bootstrap/peer_address.hpp"

namespace bzn
{
    using client_t = std::string; //placeholder

    class pbft_base
    {
    public:
        virtual void receive_request(
                const std::string& operation, 
                const client_t& client,
                const uint64_t timestamp)
            = 0;

        virtual void receive_preprepare(
                const uint64_t view,
                const uint64_t sequence,
                const std::string& operation)
            = 0;

        virtual void recieve_prepare(
                const uint64_t view,
                const uint64_t sequence,
                const std::string& operation,
                const peer_address_t& sender)
            = 0;

        virtual void recieve_commit(
                const uint64_t view,
                const uint64_t sequence,
                const std::string& operation,
                const peer_address_t& sender)
            = 0;
                
    };
}
