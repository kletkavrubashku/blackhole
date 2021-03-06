#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <boost/array.hpp>

#include <blackhole/attribute.hpp>
#include <blackhole/record.hpp>
#include <blackhole/sink/socket/udp.hpp>

#include <src/sink/socket/udp.hpp>

#include "mocks/node.hpp"
#include "mocks/registry.hpp"

namespace blackhole {
inline namespace v1 {
namespace sink {
namespace socket {
namespace {

using ::testing::Return;
using ::testing::StrictMock;

TEST(udp_t, Endpoint) {
    udp_t sink("0.0.0.0", 20000);

    EXPECT_EQ(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 20000), sink.endpoint());
}

TEST(udp_t, SendsData) {
    boost::asio::io_service io_service;
    boost::asio::ip::udp::socket socket(io_service,
        boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0));
    const auto endpoint = socket.local_endpoint();

    udp_t sink(endpoint.address().to_string(), endpoint.port());

    const string_view message("");
    const attribute_pack pack;
    const record_t record(0, message, pack);

    sink.emit(record, "{}");

    boost::array<char, 2> buffer;
    boost::asio::ip::udp::endpoint remote;
    socket.receive_from(boost::asio::buffer(buffer), remote, 0);

    EXPECT_EQ('{', buffer[0]);
    EXPECT_EQ('}', buffer[1]);
}

TEST(udp_t, FactoryType) {
    EXPECT_EQ(std::string("udp"), factory<udp_t>(mock_registry_t()).type());
}

TEST(udp_t, FactoryThrowsIfHostParameterIsMissing) {
    StrictMock<config::testing::mock::node_t> config;

    EXPECT_CALL(config, subscript_key("host"))
        .Times(1)
        .WillOnce(Return(nullptr));

    try {
        factory<udp_t>(mock_registry_t()).from(config);
    } catch (const std::invalid_argument& err) {
        EXPECT_STREQ(R"(parameter "host" is required)", err.what());
    }
}

TEST(udp_t, FactoryThrowsIfPortParameterIsMissing) {
    using config::testing::mock::node_t;

    StrictMock<node_t> config;

    auto n1 = new node_t;
    EXPECT_CALL(config, subscript_key("host"))
        .Times(1)
        .WillOnce(Return(n1));

    EXPECT_CALL(*n1, to_string())
        .Times(1)
        .WillOnce(Return("localhost"));

    EXPECT_CALL(config, subscript_key("port"))
        .Times(1)
        .WillOnce(Return(nullptr));

    try {
        factory<udp_t>(mock_registry_t()).from(config);
    } catch (const std::invalid_argument& err) {
        EXPECT_STREQ(R"(parameter "port" is required)", err.what());
    }
}

TEST(udp_t, FactoryConfig) {
    using config::testing::mock::node_t;

    StrictMock<node_t> config;

    auto n1 = new node_t;
    EXPECT_CALL(config, subscript_key("host"))
        .Times(1)
        .WillOnce(Return(n1));

    EXPECT_CALL(*n1, to_string())
        .Times(1)
        .WillOnce(Return("0.0.0.0"));

    auto n2 = new node_t;
    EXPECT_CALL(config, subscript_key("port"))
        .Times(1)
        .WillOnce(Return(n2));

    EXPECT_CALL(*n2, to_uint64())
        .Times(1)
        .WillOnce(Return(20000));

    const auto sink = factory<udp_t>(mock_registry_t()).from(config);
    const auto& cast = dynamic_cast<const udp_t&>(*sink);

    EXPECT_EQ(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 20000), cast.endpoint());
}

}  // namespace
}  // namespace socket
}  // namespace sink
}  // namespace v1
}  // namespace blackhole
