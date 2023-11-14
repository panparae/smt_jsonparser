#include <iostream>

#include "smt_jsonparser.h"
#include "catch2/catch_session.hpp"

int main(int argc, char* argv[]) {
    // There must be exactly one instance
    Catch::Session session;

    // writing to session.configData() here sets defaults
    // this is the preferred way to set them

    int returnCode = session.applyCommandLine(argc, argv);
    if (returnCode != 0)  // Indicates a command line error
        return returnCode;

    // writing to session.configData() or session.Config() here
    // overrides command line args
    // only do this if you know you need to

    int numFailed = session.run();

    // numFailed is clamped to 255 as some unices only use the lower 8 bits.
    // This clamping has already been applied, so just return it here
    // You can also do any post run clean-up here
    return numFailed;
}

#include <catch.hpp>

TEMPLATE_TEST_CASE("vectors can be sized and resized", "[vector][template]",
    int, std::string, (std::tuple<int, float>)) {
    std::vector<TestType> v(5);

    REQUIRE(v.size() == 5);
    REQUIRE(v.capacity() >= 5);

    SECTION("resizing bigger changes size and capacity") {
        v.resize(10);

        REQUIRE(v.size() == 10);
        REQUIRE(v.capacity() >= 10);
    }
    SECTION("resizing smaller changes size but not capacity") {
        v.resize(0);

        REQUIRE(v.size() == 0);
        REQUIRE(v.capacity() >= 5);

        SECTION("We can use the 'swap trick' to reset the capacity") {
            std::vector<TestType> empty;
            empty.swap(v);

            REQUIRE(v.capacity() == 0);
        }
    }
    SECTION("reserving smaller does not change size or capacity") {
        v.reserve(0);

        REQUIRE(v.size() == 5);
        REQUIRE(v.capacity() >= 5);
    }
}