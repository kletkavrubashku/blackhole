#include "Mocks.hpp"

using namespace blackhole;

TEST(verbose_logger_t, Class) {
    enum level : std::uint64_t { debug, info, warn, error };
    verbose_logger_t<level> log;
    UNUSED(log);
}

TEST(verbose_logger_t, OpenRecordByDefault) {
    enum level : std::uint64_t { debug, info, warn, error };
    verbose_logger_t<level> log;
    log::record_t record = log.open_record(level::debug);
    EXPECT_TRUE(record.valid());
}

TEST(verbose_logger_t, OpenRecordForValidVerbosityLevel) {
    enum class level : std::uint64_t { debug, info, warn, error };
    verbose_logger_t<level> log;
    log.set_filter(keyword::severity<level>() >= level::info);
    EXPECT_FALSE(log.open_record(level::debug).valid());
    EXPECT_TRUE(log.open_record(level::info).valid());
    EXPECT_TRUE(log.open_record(level::warn).valid());
    EXPECT_TRUE(log.open_record(level::error).valid());
}

TEST(verbose_logger_t, Manual) {
    enum level : std::uint64_t { debug, info, warn, error };
    verbose_logger_t<level> log;

    //!@note: Factory starts here...
    auto formatter = std::make_unique<formatter::string_t>("[]: %(message)s");
    auto sink = std::make_unique<sink::file_t<>>("/dev/stdout");
    auto frontend = std::make_unique<frontend_t<formatter::string_t, sink::file_t<>>>(std::move(formatter), std::move(sink));
    //!@note ... till here.
    log.add_frontend(std::move(frontend));

    //!@note: Next lines can be hided via macro: LOG(log, debug, "Message %s", "Hell", keyword::answer = 42);
    log::record_t record = log.open_record(level::error);
    if (record.valid()) {
        record.attributes["message"] = utils::format("Some message from: '%s'!", "Hell");
        // Add another attributes.
        log.push(std::move(record));
    }
}
