// TODO(dkorolev): Look into Cereal's polymorphism when serializing objects.

#include <gtest/gtest.h>
#include <glog/logging.h>

#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <sstream>

#include "cereal/archives/binary.hpp"
#include "cereal/archives/json.hpp"

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
        ar(CEREAL_NVP(int_), CEREAL_NVP(string_), CEREAL_NVP(vector_), CEREAL_NVP(map_));
    }
};

TEST(CerealTest, SimpleTypeBinarySerialization) { 
    std::string serialized;
    // Initialize and serialize the object.
    {
        SimpleType object;
        object.int_ = 42;
        object.string_ = "The Answer";
        const char* const str = "Meh";
        object.vector_ = std::vector<uint8_t>(str, str + strlen(str));
        object.map_["foo"] = "index 1";
        object.map_["bar"] = "index 0";
        object.map_["meh"] = "index 2";

        std::ostringstream os;
        (cereal::BinaryOutputArchive(os))(object);
        serialized = os.str();
    }
    // Test that binary serialization format has not changed.
    std::ifstream fi("simple_object.bin");
    std::string golden((std::istreambuf_iterator<char>(fi)), std::istreambuf_iterator<char>());
    ASSERT_EQ(golden, serialized);
    // De-serialize the object and test its integrity.
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
    // Test that an exception is thrown if the input can not be deserialized.
    {
        std::istringstream is(serialized.substr(0, serialized.length() - 1));
        SimpleType tmp;
        ASSERT_THROW((cereal::BinaryInputArchive(is))(tmp), cereal::Exception);
    }
}

TEST(CerealTest, SimpleTypeJSONSerialization) { 
    std::string serialized;
    // Initialize and serialize the object.
    {
        SimpleType object;
        object.int_ = 42;
        object.string_ = "The Answer";
        const char* const str = "Meh";
        object.vector_ = std::vector<uint8_t>(str, str + strlen(str));
        object.map_["foo"] = "index 1";
        object.map_["bar"] = "index 0";
        object.map_["meh"] = "index 2";

        std::ostringstream os;
        (cereal::JSONOutputArchive(os))(object);
        os << std::endl;  // Add a newline to match the golden file.
        serialized = os.str();
    }
    // Test that JSON serialization format has not changed.
    std::ifstream fi("simple_object.json");
    std::string golden((std::istreambuf_iterator<char>(fi)), std::istreambuf_iterator<char>());
    ASSERT_EQ(golden, serialized);
    // De-serialize the object and test its integrity.
    {
        std::istringstream is(serialized);
        cereal::JSONInputArchive ar(is);
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
    // Test that an exception is thrown if the input can not be deserialized.
    {
        std::istringstream is(serialized.substr(0, serialized.length() - 2));  // Minus the added newline as well.
        SimpleType tmp;
        ASSERT_THROW((cereal::JSONInputArchive(is))(tmp), cereal::Exception);
    }
}
