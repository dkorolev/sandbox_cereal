#include "../Bricks/3party/gtest/gtest-main.h"

#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
// #include <type_traits>

#include "../cereal/archives/binary.hpp"
#include "../cereal/archives/json.hpp"

#include "../cereal/types/string.hpp"
#include "../cereal/types/vector.hpp"
#include "../cereal/types/map.hpp"

#include "../cereal/types/polymorphic.hpp"

#ifndef _WIN32
enum class XID : int;
enum class XXID : int;
#else
typedef int XID;
typedef int XXID;
#endif

class SimpleType {
  public:
    SimpleType() = default;

    XID int_;
    std::string string_;
    std::vector<uint8_t> vector_;
    std::map<std::string, std::string> map_;

    template<class A> void serialize(A& ar) {
        ar(CEREAL_NVP(int_), CEREAL_NVP(string_), CEREAL_NVP(vector_), CEREAL_NVP(map_));
    }
};

struct BaseType {
    virtual std::string AsString() const = 0;
};

struct DerivedTypeInt : BaseType {
    XXID value;
    virtual std::string AsString() const {
        std::ostringstream os;
        os << "DerivedTypeInt: " << int(value);
        return os.str();
    }

	template<class A> void serialize(A& ar) {
        ar(CEREAL_NVP(value));
    }
};

struct DerivedTypeString : BaseType {
    std::string value;
    virtual std::string AsString() const {
        std::ostringstream os;
        os << "DerivedTypeString: " << value;
        return os.str();
    }

	template<class A> void serialize(A& ar) {
        ar(CEREAL_NVP(value));
    }
};

CEREAL_REGISTER_TYPE(DerivedTypeInt);
CEREAL_REGISTER_TYPE(DerivedTypeString);

TEST(CerealTest, SimpleTypeBinarySerialization) {
    std::string serialized;
    // Initialize and serialize the object.
    {
        SimpleType object;
        object.int_ = XID(42);
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
        EXPECT_EQ(42, int(result.int_));
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
        object.int_ = XID(42);
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
        EXPECT_EQ(42, int(result.int_));
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

TEST(CerealTest, PolymorphicTypeBinarySerialization) {
    std::string serialized;
    // Initialize and serialize two polymorphic objects.
    {
        std::shared_ptr<DerivedTypeInt> one(new DerivedTypeInt());
        one->value = XXID(42);
        std::shared_ptr<DerivedTypeString> two(new DerivedTypeString());
        two->value = "The Answer";
        // Need to instantiate std::shared_ptr-s externally instead of using std::make_shared.
        // Otherwise get `internal compiler error` on my `g++ (Ubuntu/Linaro 4.7.3-2ubuntu1~12.04) 4.7.3`.
        std::shared_ptr<BaseType> pone(one);
        std::shared_ptr<BaseType> ptwo(two);
        std::ostringstream os;
        (cereal::BinaryOutputArchive(os))(pone, ptwo);
        serialized = os.str();
    }
    // Test that binary serialization format has not changed.
    std::ifstream fi("polymorphic_object.bin");
    std::string golden((std::istreambuf_iterator<char>(fi)), std::istreambuf_iterator<char>());
    ASSERT_EQ(golden, serialized);
    // De-serialize two polymorphic objects and test their integrity.
    {
        std::shared_ptr<BaseType> one;
        std::shared_ptr<BaseType> two;
        std::istringstream is(serialized);
        cereal::BinaryInputArchive ar(is);
        ar(one, two);
        EXPECT_EQ("DerivedTypeInt: 42", one->AsString());
        EXPECT_EQ("DerivedTypeString: The Answer", two->AsString());
    }
    // Test that an exception is thrown if the input can not be deserialized.
    {
        std::shared_ptr<BaseType> one;
        std::shared_ptr<BaseType> two;
        std::istringstream is(serialized.substr(0, serialized.length() - 2));  // Minus the added newline as well.
        ASSERT_THROW((cereal::BinaryInputArchive(is))(one, two), cereal::Exception);
    }
}

TEST(CerealTest, PolymorphicTypeJSONSerialization) {
    std::string serialized;
    // Initialize and serialize two polymorphic objects.
    {
        std::shared_ptr<DerivedTypeInt> one(new DerivedTypeInt());
        one->value = XXID(42);
        std::shared_ptr<DerivedTypeString> two(new DerivedTypeString());
        two->value = "The Answer";
        // Need to instantiate std::shared_ptr-s externally instead of using std::make_shared.
        // Otherwise get `internal compiler error` on my `g++ (Ubuntu/Linaro 4.7.3-2ubuntu1~12.04) 4.7.3`.
        std::shared_ptr<BaseType> pone(one);
        std::shared_ptr<BaseType> ptwo(two);
        std::ostringstream os;
        (cereal::JSONOutputArchive(os))(pone, ptwo);
        os << std::endl;  // Add a newline to match the golden file.
        serialized = os.str();
    }
    // Test that JSON serialization format has not changed.
    {
        std::ifstream fi("polymorphic_object.json");
        std::string golden((std::istreambuf_iterator<char>(fi)), std::istreambuf_iterator<char>());
        ASSERT_EQ(golden, serialized);
    }
    // De-serialize two polymorphic objects and test their integrity.
    {
        std::shared_ptr<BaseType> one;
        std::shared_ptr<BaseType> two;
        std::istringstream is(serialized);
        cereal::JSONInputArchive ar(is);
        ar(one, two);
        EXPECT_EQ("DerivedTypeInt: 42", one->AsString());
        EXPECT_EQ("DerivedTypeString: The Answer", two->AsString());
    }
    // Test that an exception is thrown if the input can not be deserialized.
    {
        std::shared_ptr<BaseType> one;
        std::shared_ptr<BaseType> two;
        std::istringstream is(serialized.substr(0, serialized.length() - 2));  // Minus the added newline as well.
        ASSERT_THROW((cereal::JSONInputArchive(is))(one, two), cereal::Exception);
    }

    // Testing the theory that the IDs are stream-based. They are!
    {
        std::shared_ptr<DerivedTypeInt> one(new DerivedTypeInt());
        one->value = XXID(7);
        std::shared_ptr<DerivedTypeString> two(new DerivedTypeString());
        two->value = "Seven";
        std::shared_ptr<BaseType> pone(one);
        std::shared_ptr<BaseType> ptwo(two);
        std::ostringstream os;
        (cereal::JSONOutputArchive(os))(ptwo, pone);
        os << std::endl;  // Add a newline to match the golden file.
        serialized = os.str();
        {
            std::ifstream fi("polymorphic_object_2.json");
            std::string golden((std::istreambuf_iterator<char>(fi)), std::istreambuf_iterator<char>());
            ASSERT_EQ(golden, serialized);
        }
    }
}

struct NonSerializable {
};

// Helper compile-time test that certain type can be serialized via cereal.
template <typename T>
class is_cerealizeable {
private:
	struct meh {};
	template<typename WUT> static decltype(std::declval<WUT>().serialize(std::declval<cereal::JSONOutputArchive>())) huh();
	template<typename WUT> static meh huh(...);

public:
	constexpr static bool value = !std::is_same<decltype(huh<T>()), meh>::value;
};

TEST(CerealTest, IsCerealizableTest) {
  EXPECT_TRUE(is_cerealizeable<SimpleType>::value);
  EXPECT_TRUE(is_cerealizeable<DerivedTypeInt>::value);
  EXPECT_TRUE(is_cerealizeable<DerivedTypeString>::value);
  EXPECT_FALSE(is_cerealizeable<NonSerializable>::value);
}