#include "Mocks.hpp"

using namespace blackhole;

TEST(logger_base_t, CanBeEnabled) {
    logger_base_t log;
    log.enable();
    EXPECT_TRUE(log.enabled());
}

TEST(logger_base_t, CanBeDisabled) {
    logger_base_t log;
    log.disable();
    EXPECT_FALSE(log.enabled());
}

TEST(logger_base_t, EnabledByDefault) {
    logger_base_t log;
    EXPECT_TRUE(log.enabled());
}

TEST(logger_base_t, OpenRecordByDefault) {
    std::unique_ptr<mock::frontend_t> frontend;

    logger_base_t log;
    log.add_frontend(std::move(frontend));
    EXPECT_TRUE(log.open_record().valid());
}

TEST(logger_base_t, DoNotOpenRecordIfDisabled) {
    std::unique_ptr<mock::frontend_t> frontend;

    logger_base_t log;
    log.add_frontend(std::move(frontend));
    log.disable();
    EXPECT_FALSE(log.open_record().valid());
}

namespace blackhole { namespace keyword { DECLARE_KEYWORD(urgent, std::uint8_t) } }

TEST(logger_base_t, OpensRecordWhenAttributeFilterSucceed) {
    std::unique_ptr<mock::frontend_t> frontend;

    logger_base_t log;
    log.add_frontend(std::move(frontend));
    log.set_filter(expr::has_attr(keyword::urgent()));
    log.add_attribute(keyword::urgent() = 1);
    EXPECT_TRUE(log.open_record().valid());
}

TEST(logger_base_t, DoNotOpenRecordWhenAttributeFilterFailed) {
    std::unique_ptr<mock::frontend_t> frontend;

    logger_base_t log;
    log.add_frontend(std::move(frontend));
    log.set_filter(expr::has_attr(keyword::urgent()));
    EXPECT_FALSE(log.open_record().valid());
}

TEST(logger_base_t, OpenRecordWhenComplexFilterSucceed) {
    std::unique_ptr<mock::frontend_t> frontend;

    logger_base_t log;
    log.add_frontend(std::move(frontend));
    log.set_filter(expr::has_attr(keyword::urgent()) && keyword::urgent() == 1);
    log.add_attribute(keyword::urgent() = 1);
    EXPECT_TRUE(log.open_record().valid());
}

TEST(logger_base_t, DoNotOpenRecordWhenComplexFilterFailed) {
    std::unique_ptr<mock::frontend_t> frontend;

    logger_base_t log;
    log.add_frontend(std::move(frontend));
    log.set_filter(expr::has_attr(keyword::urgent()) && keyword::urgent() == 1);
    log.add_attribute(keyword::urgent() = 2);
    EXPECT_FALSE(log.open_record().valid());
}

TEST(logger_base_t, DoNotOpenRecordIfThereAreNoFrontends) {
    logger_base_t log;
    EXPECT_FALSE(log.open_record().valid());
}

//!@todo: TestCustomAttributes: setting,
//! filtering,
//! implement inspect::getattr function(attr) and (string),
//! overload inspect::has_attr(string)
//! make || operations in filtering
//! implement %(...A)s handling in formatter::string
//! implement %(...L)s handling for only local other attributes in format::string
