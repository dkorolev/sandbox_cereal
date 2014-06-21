// TODO(dkorolev): Add a death test for serialization parsing error.
// TODO(dkorolev): Add JSON serialization test.
// TODO(dkorolev): Look into how Cereal serializes field names.
// TODO(dkorolev): Look into Cereal's polymorphism when serializing objects.

#include <gtest/gtest.h>
#include <glog/logging.h>

#include "cereal/archives/binary.hpp"

#include "cereal/types/string.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/map.hpp"

class SimpleType {
  public:
    SimpleType() = default;

    int int_;
    std::string string_;
    std::vector<uint8_t> vector_;
    std::map<std::string, std::string> map_;

  private:
    friend class cereal::access;
    template<class A> void serialize(A& ar) {
        ar(int_, string_, vector_, map_);
    }
};

TEST(CerealTest, SimpleSerializationTest) { 
    std::string serialized;
    {
        std::ostringstream os;
        cereal::BinaryOutputArchive ar(os);
        SimpleType object;
        object.int_ = 42;
        object.string_ = "The Answer";
        const char* const str = "Meh";
        object.vector_ = std::vector<uint8_t>(str, str + strlen(str));
        object.map_["foo"] = "index 1";
        object.map_["bar"] = "index 0";
        object.map_["meh"] = "index 2";
        ar(object);
        serialized = os.str();
    }
    EXPECT_GT(serialized.length(), 0);
    ASSERT_EQ(serialized.length(), 119);  // Assume the serialization format would not change.
    {
        std::istringstream is(serialized);
        cereal::BinaryInputArchive ar(is);
        SimpleType result;
        ar(result);
        EXPECT_EQ(42, result.int_);
        EXPECT_EQ("The Answer", result.string_);
        ASSERT_EQ(3, result.vector_.size());
        EXPECT_EQ('M', result.vector_[0]);
        EXPECT_EQ('e', result.vector_[1]);
        EXPECT_EQ('h', result.vector_[2]);
        ASSERT_EQ(3, result.map_.size());
        EXPECT_EQ("index 0", result.map_["bar"]);
        EXPECT_EQ("index 1", result.map_["foo"]);
        EXPECT_EQ("index 2", result.map_["meh"]);
    }
}
