#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <sstream>
#include <string>

#include "Entity/Components/CollisionComponent.h"
#include "Entity/Components/PhysicsComponent.h"
#include "Entity/Components/ScriptComponent.h"
#include "Entity/Components/TransformComponent.h"

namespace
{
template <typename T>
std::string SerializeToJson(T& value)
{
    std::ostringstream stream;
    {
        cereal::JSONOutputArchive archive(stream);
        value.serialize(archive);
    }
    return stream.str();
}

template <typename T>
T DeserializeFromJson(const std::string& json)
{
    T value;
    std::istringstream stream(json);
    cereal::JSONInputArchive archive(stream);
    value.serialize(archive);
    return value;
}
}

TEST_CASE("Transform component data survives a JSON round trip", "[serialization][component]")
{
    TransformComponent source;
    source.mPosition = {1.25f, -2.5f, 9.0f};
    source.mSize = {0.5f, 2.0f, 3.5f};

    const TransformComponent result = DeserializeFromJson<TransformComponent>(SerializeToJson(source));

    CHECK(result.mPosition.x == Catch::Approx(source.mPosition.x));
    CHECK(result.mPosition.y == Catch::Approx(source.mPosition.y));
    CHECK(result.mPosition.z == Catch::Approx(source.mPosition.z));
    CHECK(result.mSize.x == Catch::Approx(source.mSize.x));
    CHECK(result.mSize.y == Catch::Approx(source.mSize.y));
    CHECK(result.mSize.z == Catch::Approx(source.mSize.z));
}

TEST_CASE("Collision component preserves its custom collider shape", "[serialization][component]")
{
    CollisionComponent source;
    source.mColliderShape = ColliderShapes::CAPSULE;
    source.mSize = {1.0f, 2.0f, 3.0f};

    const std::string json = SerializeToJson(source);
    const CollisionComponent result = DeserializeFromJson<CollisionComponent>(json);

    CHECK(json.find("Capsule") != std::string::npos);
    CHECK(result.mColliderShape == ColliderShapes::CAPSULE);
    CHECK(result.mSize.x == Catch::Approx(1.0f));
    CHECK(result.mSize.y == Catch::Approx(2.0f));
    CHECK(result.mSize.z == Catch::Approx(3.0f));
}

TEST_CASE("Physics component data survives a JSON round trip", "[serialization][component]")
{
    PhysicsComponent source;
    source.mMass = 12.5f;
    source.mFriction = 0.35f;

    const PhysicsComponent result = DeserializeFromJson<PhysicsComponent>(SerializeToJson(source));

    CHECK(result.mMass == Catch::Approx(source.mMass));
    CHECK(result.mFriction == Catch::Approx(source.mFriction));
}

TEST_CASE("Script component accepts data written before is_trigger existed", "[serialization][compatibility]")
{
    const ScriptComponent result = DeserializeFromJson<ScriptComponent>(R"({"path":"player.lua"})");

    CHECK(result.mPath == "player.lua");
    CHECK_FALSE(result.mIsTrigger);
}

TEST_CASE("Script component writes and reads its optional trigger setting", "[serialization][component]")
{
    ScriptComponent source;
    source.mPath = "trigger.lua";
    source.mIsTrigger = true;

    const std::string json = SerializeToJson(source);
    const ScriptComponent result = DeserializeFromJson<ScriptComponent>(json);

    CHECK(json.find("is_trigger") != std::string::npos);
    CHECK(result.mPath == source.mPath);
    CHECK(result.mIsTrigger);
}
