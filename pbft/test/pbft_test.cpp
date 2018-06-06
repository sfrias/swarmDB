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

#include <gtest/gtest.h>
#include <include/bluzelle.hpp>
#include <pbft/pbft_operation.hpp>
#include <pbft/pbft_base.hpp>
#include <pbft/pbft.hpp>
#include <bootstrap/bootstrap_peers.hpp>
#include <proto/bluzelle.pb.h>

using namespace ::testing;

TEST(pbft_test, test_requests_create_operations) {
    pbft_msg msg;
    pbft_request req;
    msg.set_allocated_request(&req);
    bzn::bootstrap_peers peers;
    bzn::pbft pbft(nullptr, peers.get_peers());

    ASSERT_EQ(0u, pbft.outstanding_operations().size());
    pbft.handle_message(msg);
    ASSERT_EQ(1u, pbft.outstanding_operations().size());
}
